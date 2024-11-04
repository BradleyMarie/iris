#include "iris/bxdfs/lambertian_bxdf.h"

#include <numbers>

#include "iris/bxdfs/internal/math.h"

namespace iris {
namespace bxdfs {

std::optional<Vector> LambertianBrdf::SampleDiffuse(
    const Vector& incoming, const Vector& surface_normal,
    Sampler& sampler) const {
  Vector outgoing = internal::CosineSampleHemisphere(incoming.z, sampler);
  return outgoing.AlignWith(surface_normal);
}

visual_t LambertianBrdf::PdfDiffuse(const Vector& incoming,
                                    const Vector& outgoing,
                                    const Vector& surface_normal,
                                    Hemisphere hemisphere) const {
  if (hemisphere != Hemisphere::BRDF) {
    return static_cast<visual_t>(0.0);
  }

  return std::abs(static_cast<visual_t>(outgoing.z) *
                  std::numbers::inv_pi_v<visual_t>);
}

const Reflector* LambertianBrdf::ReflectanceDiffuse(
    const Vector& incoming, const Vector& outgoing, Hemisphere hemisphere,
    SpectralAllocator& allocator) const {
  if (hemisphere != Hemisphere::BRDF) {
    return nullptr;
  }

  return allocator.Scale(&reflector_, std::numbers::inv_pi_v<visual_t>);
}

std::optional<Vector> LambertianBtdf::SampleDiffuse(
    const Vector& incoming, const Vector& surface_normal,
    Sampler& sampler) const {
  Vector outgoing = internal::CosineSampleHemisphere(-incoming.z, sampler);
  return outgoing.AlignAgainst(surface_normal);
}

visual_t LambertianBtdf::PdfDiffuse(const Vector& incoming,
                                    const Vector& outgoing,
                                    const Vector& surface_normal,
                                    Hemisphere hemisphere) const {
  if (hemisphere != Hemisphere::BTDF) {
    return static_cast<visual_t>(0.0);
  }

  return std::abs(static_cast<visual_t>(outgoing.z) *
                  std::numbers::inv_pi_v<visual_t>);
}

const Reflector* LambertianBtdf::ReflectanceDiffuse(
    const Vector& incoming, const Vector& outgoing, Hemisphere hemisphere,
    SpectralAllocator& allocator) const {
  if (hemisphere != Hemisphere::BTDF) {
    return nullptr;
  }

  return allocator.Scale(&transmittance_, std::numbers::inv_pi_v<visual_t>);
}

}  // namespace bxdfs
}  // namespace iris