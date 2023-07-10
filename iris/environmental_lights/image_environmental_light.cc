#include "iris/environmental_lights/image_environmental_light.h"

#define _USE_MATH_CONSTANTS
#include <algorithm>
#include <cassert>
#include <cmath>

namespace iris {
namespace environmental_lights {
namespace {

constexpr visual_t kTwoPiSquared = 2.0 * M_PI * M_PI;
constexpr geometric_t kTwoPi = 2.0 * M_PI;
constexpr geometric_t kPi = M_PI;
constexpr geometric_t kOne = 1.0;

std::pair<geometric_t, geometric_t> DirectionToUV(const Vector& direction) {
  geometric_t cos_theta =
      std::clamp(direction.z, static_cast<geometric_t>(-1.0),
                 static_cast<geometric_t>(1.0));
  geometric_t theta = std::acos(cos_theta);

  geometric_t phi = std::atan2(direction.y, direction.x);
  if (phi < static_cast<geometric_t>(0.0)) {
    phi += kTwoPi;
  }

  static constexpr geometric_t limit = std::nextafter(
      static_cast<geometric_t>(1.0), static_cast<geometric_t>(0.0));
  return std::make_pair(std::clamp(phi / static_cast<geometric_t>(2.0 * M_PI),
                                   static_cast<geometric_t>(0.0), limit),
                        std::clamp(theta / static_cast<geometric_t>(M_PI),
                                   static_cast<geometric_t>(0.0), limit));
}

std::vector<visual> ScaleLuma(std::span<const visual> luma,
                              std::pair<size_t, size_t> size) {
  assert(size.first * size.second == luma.size());

  std::vector<visual> scaled_values;
  scaled_values.reserve(luma.size());
  for (size_t index = 0; index < luma.size(); index += 1) {
    size_t y = index / size.second;
    visual_t v = (static_cast<visual_t>(y) + static_cast<visual_t>(0.5)) /
                 static_cast<visual_t>(size.first);
    scaled_values.push_back(luma[index] * std::sin(kPi * v));
  }

  return scaled_values;
}

visual_t SafeDivide(visual_t numerator, visual_t denominator) {
  if (denominator == static_cast<visual_t>(0.0)) {
    return static_cast<visual_t>(0.0);
  }

  return numerator / denominator;
}

}  // namespace

ImageEnvironmentalLight::ImageEnvironmentalLight(
    std::span<const ReferenceCounted<Spectrum>> spectra,
    std::span<const visual> luma, std::pair<size_t, size_t> size,
    ReferenceCounted<Spectrum> scalar, const Matrix& model_to_world)
    : spectra_(spectra.begin(), spectra.end()),
      distribution_(ScaleLuma(luma, size), size),
      size_(size),
      scalar_(std::move(scalar)),
      model_to_world_(model_to_world) {
  assert(!spectra.empty());
  assert(spectra.size() == size.first * size.second);
  assert(!luma.empty());
  assert(luma.size() == size.first * size.second);
}

std::optional<EnvironmentalLight::SampleResult> ImageEnvironmentalLight::Sample(
    Sampler& sampler, SpectralAllocator& allocator) const {
  visual_t pdf;
  size_t offset;
  auto [u, v] = distribution_.Sample(sampler, &pdf, &offset);

  const Spectrum* spectrum = spectra_[offset].Get();
  if (!spectrum) {
    return std::nullopt;
  }

  geometric_t phi = u * kTwoPi;
  geometric_t cos_phi = std::cos(phi);
  geometric_t sin_phi = std::sin(phi);

  geometric_t theta = v * kPi;
  geometric_t cos_theta = std::cos(theta);
  geometric_t sin_theta = std::sin(theta);

  Vector model_to_light(sin_theta * cos_phi, sin_theta * sin_phi, cos_theta);

  return EnvironmentalLight::SampleResult{
      *spectrum, model_to_world_.Multiply(model_to_light),
      SafeDivide(pdf, kTwoPiSquared * sin_theta)};
}

const Spectrum* ImageEnvironmentalLight::Emission(const Vector& to_light,
                                                  SpectralAllocator& allocator,
                                                  visual_t* pdf) const {
  Vector model_to_light = Normalize(model_to_world_.InverseMultiply(to_light));
  auto [u, v] = DirectionToUV(model_to_light);

  if (pdf) {
    geometric_t cos_theta = model_to_light.z;
    geometric_t sin_theta =
        std::sqrt(kOne - std::min(kOne, cos_theta * cos_theta));
    *pdf = SafeDivide(distribution_.Pdf(u, v), kTwoPiSquared * sin_theta);
  }

  size_t x = u * static_cast<geometric_t>(size_.second);
  size_t y = v * static_cast<geometric_t>(size_.first);
  size_t index = y * size_.second + x;

  return allocator.Scale(spectra_[index].Get(), scalar_.Get());
}

}  // namespace environmental_lights
}  // namespace iris