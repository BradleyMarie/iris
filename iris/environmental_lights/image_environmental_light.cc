#define _USE_MATH_DEFINES
#include "iris/environmental_lights/image_environmental_light.h"

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

static const geometric_t kLimit = std::nextafter(static_cast<geometric_t>(1.0),
                                                 static_cast<geometric_t>(0.0));

std::pair<geometric_t, geometric_t> DirectionToUV(const Vector& direction) {
  geometric_t cos_phi = std::clamp(direction.z, static_cast<geometric_t>(-1.0),
                                   static_cast<geometric_t>(1.0));
  geometric_t phi = std::acos(cos_phi);

  geometric_t theta = std::atan2(direction.y, direction.x);
  if (theta < static_cast<geometric_t>(0.0)) {
    theta += kTwoPi;
  }

  return std::make_pair(
      std::clamp(theta / kTwoPi, static_cast<geometric_t>(0.0), kLimit),
      std::clamp(phi / kPi, static_cast<geometric_t>(0.0), kLimit));
}

std::vector<visual> ScaleLuma(std::span<const visual> luma,
                              std::pair<size_t, size_t> size, visual_t& power) {
  assert(size.first * size.second == luma.size());

  std::vector<visual> scaled_values;
  scaled_values.reserve(luma.size());

  visual_t total_weight = 0.0;
  power = static_cast<visual_t>(0.0);
  for (size_t index = 0; index < luma.size(); index += 1) {
    size_t y = index / size.second;
    visual_t v = (static_cast<visual_t>(y) + static_cast<visual_t>(0.5)) /
                 static_cast<visual_t>(size.first);
    visual_t weight = std::sin(kPi * v);
    scaled_values.push_back(luma[index] * weight);
    power += scaled_values.back();
    total_weight += weight;
  }

  power *= static_cast<visual_t>(4.0 * M_PI) / total_weight;

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
    std::vector<ReferenceCounted<Spectrum>> spectra,
    std::span<const visual> luma, std::pair<size_t, size_t> size,
    ReferenceCounted<Spectrum> scalar, const Matrix& model_to_world)
    : spectra_(std::move(spectra)),
      distribution_(ScaleLuma(luma, size, power_), size),
      size_(size),
      scalar_(std::move(scalar)),
      model_to_world_(model_to_world) {
  assert(!spectra_.empty());
  assert(spectra_.size() == size.first * size.second);
  assert(!luma.empty());
  assert(luma.size() == size.first * size.second);
}

std::optional<EnvironmentalLight::SampleResult> ImageEnvironmentalLight::Sample(
    Sampler& sampler, SpectralAllocator& allocator) const {
  visual_t pdf;
  size_t offset;
  auto [u, v] = distribution_.Sample(sampler, &pdf, &offset);

  const Spectrum* spectrum =
      allocator.Scale(spectra_[offset].Get(), scalar_.Get());
  if (!spectrum) {
    return std::nullopt;
  }

  geometric_t theta = u * kTwoPi;
  geometric_t sin_theta = std::sin(theta);
  geometric_t cos_theta = std::cos(theta);

  geometric_t phi = v * kPi;
  geometric_t sin_phi = std::sin(phi);
  geometric_t cos_phi = std::cos(phi);

  Vector model_to_light(cos_theta * sin_phi, sin_theta * sin_phi, cos_phi);

  return EnvironmentalLight::SampleResult{
      *spectrum, model_to_world_.Multiply(model_to_light),
      SafeDivide(pdf, kTwoPiSquared * sin_phi)};
}

const Spectrum* ImageEnvironmentalLight::Emission(const Vector& to_light,
                                                  SpectralAllocator& allocator,
                                                  visual_t* pdf) const {
  Vector model_to_light = Normalize(model_to_world_.InverseMultiply(to_light));
  auto [u, v] = DirectionToUV(model_to_light);

  if (pdf) {
    geometric_t cos_phi = model_to_light.z;
    geometric_t sin_phi = std::sqrt(kOne - std::min(kOne, cos_phi * cos_phi));
    *pdf = SafeDivide(distribution_.Pdf(u, v), kTwoPiSquared * sin_phi);
  }

  size_t x = u * static_cast<geometric_t>(size_.second);
  size_t y = v * static_cast<geometric_t>(size_.first);
  size_t index = y * size_.second + x;

  return allocator.Scale(spectra_[index].Get(), scalar_.Get());
}

visual_t ImageEnvironmentalLight::Power(const PowerMatcher& power_matcher,
                                        visual_t world_radius_squared) const {
  return world_radius_squared * power_;
}

}  // namespace environmental_lights
}  // namespace iris