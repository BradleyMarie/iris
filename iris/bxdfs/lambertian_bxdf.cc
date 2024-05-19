#define _USE_MATH_DEFINES
#include "iris/bxdfs/lambertian_bxdf.h"

#include <cassert>
#include <cmath>

namespace iris {
namespace bxdfs {
namespace {

Vector SampleHemisphere(geometric incoming_z, Sampler& sampler) {
  geometric_t radius_squared = sampler.Next();
  geometric_t radius = std::sqrt(radius_squared);

  geometric_t theta = sampler.Next() * static_cast<geometric_t>(2.0 * M_PI);
  geometric_t sin_theta = std::sin(theta);
  geometric_t cos_theta = std::cos(theta);

  geometric_t x = radius * cos_theta;
  geometric_t y = radius * sin_theta;
  geometric_t z = std::sqrt(static_cast<geometric_t>(1.0) - radius_squared);

  return Vector(x, y, std::copysign(z, incoming_z));
}

}  // namespace

std::optional<Bxdf::SampleResult> LambertianBrdf::Sample(
    const Vector& incoming,
    const std::optional<Bxdf::Differentials>& differentials,
    const Vector& surface_normal, Sampler& sampler) const {
  Vector outgoing = SampleHemisphere(incoming.z, sampler);
  return SampleResult{outgoing.AlignWith(surface_normal), std::nullopt, this};
}

std::optional<visual_t> LambertianBrdf::Pdf(const Vector& incoming,
                                            const Vector& outgoing,
                                            const Vector& surface_normal,
                                            const Bxdf* sample_source,
                                            Hemisphere hemisphere) const {
  if (hemisphere != Hemisphere::BRDF) {
    return static_cast<visual_t>(0.0);
  }

  return static_cast<visual_t>(std::abs(outgoing.z * M_1_PI));
}

const Reflector* LambertianBrdf::Reflectance(
    const Vector& incoming, const Vector& outgoing, const Bxdf* sample_source,
    Hemisphere hemisphere, SpectralAllocator& allocator) const {
  if (hemisphere != Hemisphere::BRDF) {
    return nullptr;
  }

  return allocator.Scale(&reflector_, M_1_PI);
}

std::optional<Bxdf::SampleResult> LambertianBtdf::Sample(
    const Vector& incoming,
    const std::optional<Bxdf::Differentials>& differentials,
    const Vector& surface_normal, Sampler& sampler) const {
  Vector outgoing = SampleHemisphere(-incoming.z, sampler);
  return SampleResult{outgoing.AlignAgainst(surface_normal), std::nullopt,
                      this};
}

std::optional<visual_t> LambertianBtdf::Pdf(const Vector& incoming,
                                            const Vector& outgoing,
                                            const Vector& surface_normal,
                                            const Bxdf* sample_source,
                                            Hemisphere hemisphere) const {
  if (hemisphere != Hemisphere::BTDF) {
    return static_cast<visual_t>(0.0);
  }

  return static_cast<visual_t>(std::abs(outgoing.z * M_1_PI));
}

const Reflector* LambertianBtdf::Reflectance(
    const Vector& incoming, const Vector& outgoing, const Bxdf* sample_source,
    Hemisphere hemisphere, SpectralAllocator& allocator) const {
  if (hemisphere != Hemisphere::BTDF) {
    return nullptr;
  }

  return allocator.Scale(&transmittance_, M_1_PI);
}

}  // namespace bxdfs
}  // namespace iris