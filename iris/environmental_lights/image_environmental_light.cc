#include "iris/environmental_lights/image_environmental_light.h"

#define _USE_MATH_CONSTANTS
#include <algorithm>
#include <cassert>
#include <cmath>
#include <functional>
#include <map>

namespace iris {
namespace environmental_lights {
namespace {

std::pair<geometric_t, geometric_t> DirectionToUV(const Vector& direction) {
  Vector normalized = Normalize(direction);

  geometric_t cos_theta =
      std::clamp(normalized.z, static_cast<geometric_t>(-1.0),
                 static_cast<geometric_t>(1.0));
  geometric_t theta = std::acos(cos_theta);

  geometric_t phi = std::atan2(normalized.y, normalized.x);
  if (phi < static_cast<geometric_t>(0.0)) {
    phi += static_cast<geometric_t>(2.0 * M_PI);
  }

  return std::make_pair(
      std::clamp(phi / static_cast<geometric_t>(2.0 * M_PI),
                 static_cast<geometric_t>(0.0), static_cast<geometric_t>(1.0)),
      std::clamp(theta / static_cast<geometric_t>(M_PI),
                 static_cast<geometric_t>(0.0), static_cast<geometric_t>(1.0)));
}

}  // namespace

ImageEnvironmentalLight::ImageEnvironmentalLight(
    std::span<const std::pair<ReferenceCounted<Spectrum>, visual>>
        spectra_and_luminance,
    std::pair<size_t, size_t> size, ReferenceCounted<Spectrum> scalar,
    const Matrix& model_to_world)
    : model_to_world_(model_to_world), scalar_(std::move(scalar)), size_(size) {
  assert(!spectra_and_luminance.empty());
  assert(spectra_and_luminance.size() == size.first * size.second);

  spectra_.reserve(spectra_and_luminance.size());
  pdf_.reserve(spectra_and_luminance.size());
  cdf_.reserve(spectra_and_luminance.size());
  spectra_indices_.reserve(spectra_and_luminance.size());

  visual_t cumulative_luma;
  for (size_t index = 0; index < spectra_and_luminance.size(); index++) {
    visual_t v = (static_cast<visual_t>(index / size.second) +
                  static_cast<visual_t>(0.5)) /
                 static_cast<visual_t>(size.first);
    visual_t sin_theta = std::sin(static_cast<visual_t>(M_PI) * v);

    spectra_.push_back(std::move(spectra_and_luminance[index].first));
    pdf_.push_back(std::max(static_cast<visual>(0.0),
                            sin_theta * spectra_and_luminance[index].second));
    cumulative_luma += pdf_.back();
  }

  std::multimap<visual, size_t> sorted_pixels;
  for (size_t index = 0u; index < spectra_.size(); index++) {
    pdf_[index] /= cumulative_luma;
    sorted_pixels.emplace(pdf_[index], index);
  }

  visual cdf = 0.0;
  for (const auto& [pdf, index] : sorted_pixels) {
    if (pdf <= 0.0) {
      continue;
    }

    cdf = std::min(static_cast<visual>(1.0), cdf + pdf);
    cdf_.push_back(cdf);
    spectra_indices_.push_back(index);
  }

  cdf_.back() = 1.0;
}

std::optional<EnvironmentalLight::SampleResult> ImageEnvironmentalLight::Sample(
    Sampler& sampler, SpectralAllocator& allocator) const {
  geometric_t sample = sampler.Next();
  auto iter =
      std::lower_bound(cdf_.begin(), cdf_.end(), static_cast<visual>(sample));
  assert(iter != cdf_.end());

  size_t sample_index = iter - cdf_.begin();
  size_t pixel_index = spectra_indices_[*iter];

  const Spectrum* spectrum =
      allocator.Scale(spectra_[pixel_index].Get(), scalar_.Get());
  if (!spectrum) {
    return std::nullopt;
  }

  size_t x = pixel_index % size_.second;
  size_t y = pixel_index / size_.second;

  geometric_t pixel_u = (sample - *iter) / cdf_[sample_index];
  geometric_t pixel_v = sampler.Next();

  geometric_t phi = static_cast<geometric_t>(2.0 * M_PI) *
                    (static_cast<geometric_t>(x) + pixel_u) /
                    static_cast<geometric_t>(size_.second);
  geometric_t theta = static_cast<geometric_t>(M_PI) *
                      (static_cast<geometric_t>(y) + pixel_v) /
                      static_cast<geometric_t>(size_.first);

  geometric_t cos_phi = std::cos(phi);
  geometric_t sin_phi = std::sin(phi);
  geometric_t cos_theta = std::cos(theta);
  geometric_t sin_theta = std::sin(theta);

  Vector model_to_light(sin_theta * cos_phi, sin_theta * sin_phi, cos_theta);
  visual_t pdf = pdf_[pixel_index] /
                 static_cast<visual_t>(
                     static_cast<geometric_t>(2.0 * M_PI * M_PI) * sin_theta);

  return EnvironmentalLight::SampleResult{
      *spectrum, model_to_world_.Multiply(model_to_light), pdf};
}

const Spectrum* ImageEnvironmentalLight::Emission(const Vector& to_light,
                                                  SpectralAllocator& allocator,
                                                  visual_t* pdf) const {
  Vector model_to_light = model_to_world_.InverseMultiply(to_light);
  auto uv = DirectionToUV(model_to_light);

  size_t x = uv.first * static_cast<geometric_t>(size_.second);
  size_t y = uv.second * static_cast<geometric_t>(size_.first);
  size_t index = x + y * size_.second;

  if (pdf) {
    geometric_t cos_theta = model_to_light.z;
    geometric_t sin_theta =
        sqrt(static_cast<geometric_t>(1.0) -
             std::min(static_cast<geometric_t>(1.0), cos_theta * cos_theta));
    *pdf =
        pdf_[index] / static_cast<geometric_t>(2.0 * M_PI * M_PI) * sin_theta;
  }

  return allocator.Scale(spectra_[index].Get(), scalar_.Get());
}

}  // namespace environmental_lights
}  // namespace iris