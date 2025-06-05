#include "iris/bxdfs/ashikhmin_shirley_brdf.h"

#include <cmath>
#include <numbers>

#include "iris/bxdf.h"
#include "iris/bxdf_allocator.h"
#include "iris/bxdfs/internal/diffuse_bxdf.h"
#include "iris/bxdfs/internal/math.h"
#include "iris/bxdfs/internal/microfacet.h"
#include "iris/float.h"
#include "iris/reflector.h"
#include "iris/sampler.h"
#include "iris/spectral_allocator.h"
#include "iris/vector.h"

namespace iris {
namespace bxdfs {
namespace {

using ::iris::bxdfs::internal::AbsCosTheta;
using ::iris::bxdfs::internal::CosineSampleHemisphere;
using ::iris::bxdfs::internal::HalfAngle;
using ::iris::bxdfs::internal::Reflect;
using ::iris::bxdfs::internal::TrowbridgeReitzDistribution;

class AshikhminShirleyBrdf final : public internal::DiffuseBxdf {
 public:
  AshikhminShirleyBrdf(const Reflector* diffuse, const Reflector* specular,
                       const TrowbridgeReitzDistribution& distribution) noexcept
      : diffuse_(diffuse), specular_(specular), distribution_(distribution) {}

  std::optional<Vector> SampleDiffuse(const Vector& incoming,
                                      const Vector& surface_normal,
                                      Sampler& sampler) const override;

  visual_t PdfDiffuse(const Vector& incoming, const Vector& outgoing,
                      const Vector& surface_normal,
                      Hemisphere hemisphere) const override;

  const Reflector* ReflectanceDiffuse(
      const Vector& incoming, const Vector& outgoing, Hemisphere hemisphere,
      SpectralAllocator& allocator) const override;

 private:
  const Reflector* diffuse_;
  const Reflector* specular_;
  TrowbridgeReitzDistribution distribution_;
};

visual_t Pow5(visual_t value) { return value * value * value * value * value; }

const Reflector* SchlickFresnel(const Reflector* specular,
                                const Reflector* inverted_specular,
                                visual_t cos_theta,
                                SpectralAllocator& allocator) {
  visual_t attenuation = Pow5(static_cast<visual_t>(1.0) - cos_theta);
  inverted_specular = allocator.Scale(inverted_specular, attenuation);
  return allocator.Add(specular, inverted_specular);
}

std::optional<Vector> AshikhminShirleyBrdf::SampleDiffuse(
    const Vector& incoming, const Vector& surface_normal,
    Sampler& sampler) const {
  if (sampler.NextIndex(2u)) {
    Vector outgoing = CosineSampleHemisphere(incoming.z, sampler);
    return outgoing.AlignWith(surface_normal);
  }

  Vector half_angle =
      distribution_.Sample(incoming, sampler.Next(), sampler.Next());
  return Reflect(incoming, half_angle);
}

visual_t AshikhminShirleyBrdf::PdfDiffuse(const Vector& incoming,
                                          const Vector& outgoing,
                                          const Vector& surface_normal,
                                          Hemisphere hemisphere) const {
  if (hemisphere != Hemisphere::BRDF) {
    return static_cast<visual_t>(0.0);
  }

  visual_t diffuse_pdf =
      static_cast<visual_t>(0.5 * std::numbers::inv_pi) * AbsCosTheta(outgoing);

  if (incoming.z == static_cast<geometric>(0.0) ||
      outgoing.z == static_cast<geometric>(0.0) ||
      std::signbit(incoming.z) != std::signbit(outgoing.z)) {
    return diffuse_pdf;
  }

  Vector half_angle = *HalfAngle(incoming, outgoing);
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

  const Reflector* inverted_specular = allocator.Invert(specular_);
  const Reflector* unattenuated_diffuse =
      allocator.Scale(diffuse_, inverted_specular);
  const Reflector* diffuse =
      allocator.Scale(unattenuated_diffuse, diffuse_attenuation);

  if (incoming.z == static_cast<geometric>(0.0) ||
      outgoing.z == static_cast<geometric>(0.0) ||
      std::signbit(incoming.z) != std::signbit(outgoing.z)) {
    return diffuse;
  }

  Vector half_angle = *HalfAngle(incoming, outgoing);
  visual_t cos_theta_half_angle = ClampedDotProduct(outgoing, half_angle);

  visual_t specular_attenuation =
      distribution_.D(half_angle) /
      (static_cast<visual_t>(4.0) * cos_theta_half_angle *
       std::max(cos_theta_incoming, cos_theta_outgoing));
  const Reflector* unattenuated_specular = SchlickFresnel(
      specular_, inverted_specular, cos_theta_half_angle, allocator);
  const Reflector* specular =
      allocator.UnboundedScale(unattenuated_specular, specular_attenuation);

  return allocator.UnboundedAdd(diffuse, specular);
}

}  // namespace

const Bxdf* MakeAshikhminShirleyBrdf(BxdfAllocator& bxdf_allocator,
                                     const Reflector* diffuse,
                                     const Reflector* specular,
                                     geometric_t roughness_x,
                                     geometric_t roughness_y,
                                     bool remap_roughness_to_alpha) {
  if (diffuse == nullptr && specular == nullptr) {
    return nullptr;
  }

  return &bxdf_allocator.Allocate<AshikhminShirleyBrdf>(
      diffuse, specular,
      TrowbridgeReitzDistribution(roughness_x, roughness_y,
                                  remap_roughness_to_alpha));
}

}  // namespace bxdfs
}  // namespace iris
