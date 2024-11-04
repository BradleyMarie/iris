#include "iris/bxdfs/ashikhmin_shirley_brdf.h"

#include <iostream>
#include <numbers>

#include "iris/bxdfs/internal/math.h"

namespace iris {
namespace bxdfs {
namespace internal {
namespace {

visual_t Pow5(visual_t value) { return value * value * value * value * value; }

const Reflector* SchlickFresnel(const Reflector* specular,
                                const Reflector* inverted_specular,
                                visual_t cos_theta,
                                SpectralAllocator& allocator) {
  visual_t attenuation = Pow5(static_cast<visual_t>(1.0) - cos_theta);
  inverted_specular = allocator.Scale(inverted_specular, attenuation);
  return allocator.Add(specular, inverted_specular);
}

}  // namespace

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

visual_t AshikhminShirleyBrdf::PdfDiffuse(const Vector& incoming,
                                          const Vector& outgoing,
                                          const Vector& surface_normal,
                                          Hemisphere hemisphere) const {
  if (hemisphere != Hemisphere::BRDF) {
    return static_cast<visual_t>(0.0);
  }

  visual_t diffuse_pdf =
      static_cast<visual_t>(0.5 * M_1_PI) * internal::AbsCosTheta(outgoing);

  if (incoming.z == static_cast<geometric>(0.0) ||
      outgoing.z == static_cast<geometric>(0.0) ||
      std::signbit(incoming.z) != std::signbit(outgoing.z)) {
    return diffuse_pdf;
  }

  Vector half_angle = *internal::HalfAngle(incoming, outgoing);
  visual_t specular_pdf =
      distribution_.Pdf(incoming, half_angle) /
      (static_cast<visual_t>(8.0) * ClampedDotProduct(incoming, half_angle));

  return diffuse_pdf + specular_pdf;
}

const Reflector* AshikhminShirleyBrdf::ReflectanceDiffuse(
    const Vector& incoming, const Vector& outgoing, Hemisphere hemisphere,
    SpectralAllocator& allocator) const {
  if (hemisphere != Hemisphere::BRDF) {
    return nullptr;
  }

  visual_t cos_theta_incoming = AbsCosTheta(incoming);
  visual_t cos_theta_outgoing = AbsCosTheta(outgoing);

  visual_t diffuse_attenuation =
      static_cast<visual_t>(28.0 * std::numbers::inv_pi / 23.0) *
      (static_cast<visual_t>(1.0) -
       Pow5(static_cast<visual_t>(1.0) -
            static_cast<visual_t>(0.5) * cos_theta_incoming)) *
      (static_cast<visual_t>(1.0) -
       Pow5(static_cast<visual_t>(1.0) -
            static_cast<visual_t>(0.5) * cos_theta_outgoing));

  const Reflector* inverted_specular = allocator.Invert(&specular_);
  const Reflector* unattenuated_diffuse =
      allocator.Scale(&diffuse_, inverted_specular);
  const Reflector* diffuse =
      allocator.Scale(unattenuated_diffuse, diffuse_attenuation);

  if (incoming.z == static_cast<geometric>(0.0) ||
      outgoing.z == static_cast<geometric>(0.0) ||
      std::signbit(incoming.z) != std::signbit(outgoing.z)) {
    return diffuse;
  }

  Vector half_angle = *internal::HalfAngle(incoming, outgoing);
  visual_t cos_theta_half_angle = ClampedDotProduct(outgoing, half_angle);

  visual_t specular_attenuation =
      distribution_.D(half_angle) /
      (static_cast<visual_t>(4.0) * cos_theta_half_angle *
       std::max(cos_theta_incoming, cos_theta_outgoing));
  const Reflector* unattenuated_specular = SchlickFresnel(
      &specular_, inverted_specular, cos_theta_half_angle, allocator);
  const Reflector* specular =
      allocator.UnboundedScale(unattenuated_specular, specular_attenuation);

  return allocator.UnboundedAdd(diffuse, specular);
}

}  // namespace internal
}  // namespace bxdfs
}  // namespace iris