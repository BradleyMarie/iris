#define _USE_MATH_DEFINES
#include "iris/bxdfs/lambertian_bxdf.h"

#include <cmath>

#include "iris/bxdfs/internal/math.h"

namespace iris {
namespace bxdfs {

bool LambertianBrdf::IsDiffuse() const { return true; }

std::optional<Bxdf::SampleResult> LambertianBrdf::Sample(
    const Vector& incoming,
    const std::optional<Bxdf::Differentials>& differentials,
    const Vector& surface_normal, Sampler& sampler) const {
  Vector outgoing = internal::CosineSampleHemisphere(incoming.z, sampler);
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

bool LambertianBtdf::IsDiffuse() const { return true; }

std::optional<Bxdf::SampleResult> LambertianBtdf::Sample(
    const Vector& incoming,
    const std::optional<Bxdf::Differentials>& differentials,
    const Vector& surface_normal, Sampler& sampler) const {
  Vector outgoing = internal::CosineSampleHemisphere(-incoming.z, sampler);
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