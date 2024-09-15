#define _USE_MATH_DEFINES
#include "iris/bxdfs/ashikhmin_shirley_brdf.h"

#include <cmath>

#include "iris/bxdfs/internal/math.h"
#include "iris/bxdfs/math.h"

namespace iris {
namespace bxdfs {
namespace internal {
namespace {

visual_t Pow5(visual_t value) { return value * value * value * value * value; }

}  // namespace

bool AshikhminShirleyBrdf::IsDiffuse(visual_t* diffuse_pdf) const {
  if (diffuse_pdf) {
    *diffuse_pdf = static_cast<visual_t>(1.0);
  }

  return true;
}

std::optional<Vector> AshikhminShirleyBrdf::SampleDiffuse(
    const Vector& incoming, const Vector& surface_normal,
    Sampler& sampler) const {
  if (sampler.NextIndex(2u)) {
    Vector outgoing = internal::CosineSampleHemisphere(incoming.z, sampler);
    return outgoing.AlignWith(surface_normal);
  }

  Vector half_angle =
      distribution_.Sample(incoming, sampler.Next(), sampler.Next());
  return internal::Reflect(incoming, half_angle);
}

std::optional<Bxdf::SampleResult> AshikhminShirleyBrdf::Sample(
    const Vector& incoming,
    const std::optional<Bxdf::Differentials>& differentials,
    const Vector& surface_normal, Sampler& sampler) const {
  return SampleResult{*SampleDiffuse(incoming, surface_normal, sampler)};
}

visual_t AshikhminShirleyBrdf::Pdf(const Vector& incoming,
                                   const Vector& outgoing,
                                   const Vector& surface_normal,
                                   Hemisphere hemisphere) const {
  if (hemisphere != Hemisphere::BRDF) {
    return static_cast<visual_t>(0.0);
  }

  std::optional<Vector> half_angle = internal::HalfAngle(incoming, outgoing);
  if (!half_angle.has_value()) {
    return static_cast<visual_t>(0.0);
  }

  visual_t diffuse_pdf =
      static_cast<visual_t>(0.5 * M_1_PI) * internal::AbsCosTheta(outgoing);
  visual_t specular_pdf =
      distribution_.Pdf(incoming, *half_angle) /
      (static_cast<visual_t>(4.0) * PositiveDotProduct(incoming, outgoing));

  return diffuse_pdf + specular_pdf;
}

const Reflector* AshikhminShirleyBrdf::Reflectance(
    const Vector& incoming, const Vector& outgoing, Hemisphere hemisphere,
    SpectralAllocator& allocator) const {
  if (hemisphere != Hemisphere::BRDF) {
    return nullptr;
  }

  std::optional<Vector> half_angle = internal::HalfAngle(incoming, outgoing);
  if (!half_angle.has_value()) {
    return nullptr;
  }

  visual_t cos_theta_incoming = AbsCosTheta(incoming);
  visual_t cos_theta_outgoing = AbsCosTheta(outgoing);

  visual_t specular_attenuation =
      distribution_.D(*half_angle) /
      (static_cast<visual_t>(4.0) * AbsDotProduct(outgoing, *half_angle) *
       std::max(cos_theta_incoming, cos_theta_outgoing));

  const Reflector* inverted_specular = allocator.Invert(&specular_);
  const Reflector* attenuated_inversed_specular = allocator.Scale(
      inverted_specular, Pow5(static_cast<visual_t>(1.0) - cos_theta_outgoing));
  const Reflector* attenuated_specular =
      allocator.Add(attenuated_inversed_specular, &specular_);
  const Reflector* specular =
      allocator.Scale(attenuated_specular, specular_attenuation);

  visual_t diffuse_attenuation =
      static_cast<visual_t>(28.0 / (23.0 * M_PI)) *
      (static_cast<visual_t>(1.0) -
       Pow5(static_cast<visual_t>(1.0) -
            static_cast<visual_t>(0.5) * cos_theta_incoming)) *
      (static_cast<visual_t>(1.0) -
       Pow5(static_cast<visual_t>(1.0) -
            static_cast<visual_t>(0.5) * cos_theta_outgoing));

  const Reflector* attenuated_diffuse =
      allocator.Scale(&diffuse_, diffuse_attenuation);
  const Reflector* diffuse =
      allocator.Scale(attenuated_diffuse, inverted_specular);

  return allocator.Add(diffuse, specular);
}

}  // namespace internal
}  // namespace bxdfs
}  // namespace iris