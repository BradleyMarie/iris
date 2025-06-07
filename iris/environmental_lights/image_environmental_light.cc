#include "iris/environmental_lights/image_environmental_light.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <numbers>
#include <utility>
#include <vector>

#include "iris/environmental_light.h"
#include "iris/environmental_lights/internal/distribution_2d.h"
#include "iris/float.h"
#include "iris/matrix.h"
#include "iris/power_matcher.h"
#include "iris/reference_counted.h"
#include "iris/sampler.h"
#include "iris/spectral_allocator.h"
#include "iris/spectrum.h"
#include "iris/vector.h"

namespace iris {
namespace environmental_lights {
namespace {

static const geometric_t kLimit = std::nextafter(static_cast<geometric_t>(1.0),
                                                 static_cast<geometric_t>(0.0));

std::vector<visual> ScaleLuma(std::vector<visual> luma,
                              std::pair<size_t, size_t> size, visual_t& power) {
  assert(size.first * size.second == luma.size());

  visual_t total_weight = 0.0;
  power = static_cast<visual_t>(0.0);
  for (size_t index = 0; index < luma.size(); index += 1) {
    size_t y = index / size.second;
    visual_t v = (static_cast<visual_t>(y) + static_cast<visual_t>(0.5)) /
                 static_cast<visual_t>(size.first);
    visual_t weight = std::sin(std::numbers::pi_v<geometric_t> * v);
    luma[index] *= weight;
    power += luma[index];
    total_weight += weight;
  }

  power *= static_cast<visual_t>(4.0 * std::numbers::pi) / total_weight;

  return luma;
}

visual_t SafeDivide(visual_t numerator, visual_t denominator) {
  if (denominator == static_cast<visual_t>(0.0)) {
    return static_cast<visual_t>(0.0);
  }

  return numerator / denominator;
}

class ImageEnvironmentalLight final : public EnvironmentalLight {
 public:
  ImageEnvironmentalLight(std::vector<ReferenceCounted<Spectrum>> spectra,
                          std::vector<visual> luma,
                          std::pair<size_t, size_t> size,
                          const Matrix& model_to_world);

  std::optional<SampleResult> Sample(
      Sampler& sampler, SpectralAllocator& allocator) const override;

  const Spectrum* Emission(const Vector& to_light, SpectralAllocator& allocator,
                           visual_t* pdf) const override;

  visual_t Power(const PowerMatcher& power_matcher,
                 visual_t world_radius_squared) const override;

 private:
  std::vector<ReferenceCounted<Spectrum>> spectra_;
  internal::Distribution2D distribution_;
  std::pair<size_t, size_t> size_;
  Matrix model_to_world_;
  visual_t power_;
};

ImageEnvironmentalLight::ImageEnvironmentalLight(
    std::vector<ReferenceCounted<Spectrum>> spectra, std::vector<visual> luma,
    std::pair<size_t, size_t> size, const Matrix& model_to_world)
    : spectra_(std::move(spectra)),
      distribution_(ScaleLuma(std::move(luma), size, power_), size),
      size_(size),
      model_to_world_(model_to_world) {
  assert(!spectra_.empty());
  assert(spectra_.size() == size.first * size.second);
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

  geometric_t theta = u * static_cast<geometric_t>(2.0 * std::numbers::pi);
  geometric_t sin_theta = std::sin(theta);
  geometric_t cos_theta = std::cos(theta);

  geometric_t phi = v * std::numbers::pi_v<geometric_t>;
  geometric_t sin_phi = std::sin(phi);
  geometric_t cos_phi = std::cos(phi);

  Vector model_to_light(cos_theta * sin_phi, sin_theta * sin_phi, cos_phi);

  return EnvironmentalLight::SampleResult{
      *spectrum, model_to_world_.Multiply(model_to_light),
      SafeDivide(pdf, static_cast<geometric_t>(2.0 * std::numbers::pi *
                                               std::numbers::pi) *
                          sin_phi)};
}

const Spectrum* ImageEnvironmentalLight::Emission(const Vector& to_light,
                                                  SpectralAllocator& allocator,
                                                  visual_t* pdf) const {
  Vector model_to_light = Normalize(model_to_world_.InverseMultiply(to_light));

  geometric_t cos_phi =
      std::clamp(model_to_light.z, static_cast<geometric_t>(-1.0),
                 static_cast<geometric_t>(1.0));
  geometric_t phi = std::acos(cos_phi);

  geometric_t theta = std::atan2(model_to_light.y, model_to_light.x);
  if (theta < static_cast<geometric_t>(0.0)) {
    theta += static_cast<geometric_t>(2.0 * std::numbers::pi);
  }

  geometric_t u =
      std::clamp(theta / static_cast<geometric_t>(2.0 * std::numbers::pi),
                 static_cast<geometric_t>(0.0), kLimit);
  geometric_t v = std::clamp(phi / std::numbers::pi_v<geometric_t>,
                             static_cast<geometric_t>(0.0), kLimit);

  if (pdf) {
    geometric_t sin_phi =
        std::sqrt(static_cast<geometric_t>(1.0) - cos_phi * cos_phi);
    *pdf = SafeDivide(
        distribution_.Pdf(u, v),
        static_cast<geometric_t>(2.0 * std::numbers::pi * std::numbers::pi) *
            sin_phi);
  }

  size_t x = u * static_cast<geometric_t>(size_.second);
  size_t y = v * static_cast<geometric_t>(size_.first);
  size_t index = y * size_.second + x;

  return spectra_[index].Get();
}

visual_t ImageEnvironmentalLight::Power(const PowerMatcher& power_matcher,
                                        visual_t world_radius_squared) const {
  return world_radius_squared * power_;
}

}  // namespace

ReferenceCounted<EnvironmentalLight> MakeImageEnvironmentalLight(
    const std::vector<std::pair<ReferenceCounted<Spectrum>, visual>>&
        spectra_and_luma,
    std::pair<size_t, size_t> dimensions, const Matrix& model_to_world) {
  if (spectra_and_luma.size() / dimensions.first != dimensions.second ||
      spectra_and_luma.size() % dimensions.first != 0) {
    // This will go away once support for mdspan is available in libstdc++
    return ReferenceCounted<EnvironmentalLight>();
  }

  std::vector<ReferenceCounted<Spectrum>> spectra;
  std::vector<visual> luma;
  bool is_black = true;
  for (size_t i = 0; i < dimensions.first; i++) {
    for (size_t j = 0; j < dimensions.second; j++) {
      spectra.push_back(spectra_and_luma[dimensions.second * i + j].first);

      if (std::isfinite(spectra_and_luma[dimensions.second * i + j].second) &&
          spectra_and_luma[dimensions.second * i + j].second >
              static_cast<visual>(0.0)) {
        luma.push_back(spectra_and_luma[dimensions.second * i + j].second);
      } else {
        luma.push_back(static_cast<visual>(0.0));
      }

      if (spectra.back() && luma.back() != static_cast<visual>(0.0)) {
        is_black = false;
      }
    }
  }

  if (is_black) {
    return ReferenceCounted<EnvironmentalLight>();
  }

  return MakeReferenceCounted<ImageEnvironmentalLight>(
      std::move(spectra), std::move(luma), dimensions, model_to_world);
}

}  // namespace environmental_lights
}  // namespace iris
