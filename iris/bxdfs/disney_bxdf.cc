#include "iris/bxdfs/disney_bxdf.h"

#include <algorithm>
#include <numbers>
#include <numeric>

#include "iris/bxdf.h"
#include "iris/bxdf_allocator.h"
#include "iris/bxdfs/internal/diffuse_bxdf.h"
#include "iris/bxdfs/internal/fresnel.h"
#include "iris/bxdfs/internal/math.h"
#include "iris/bxdfs/internal/microfacet_bxdf.h"
#include "iris/bxdfs/internal/microfacet_distribution.h"
#include "iris/float.h"
#include "iris/reference_counted.h"
#include "iris/reflector.h"
#include "iris/reflectors/uniform_reflector.h"
#include "iris/sampler.h"
#include "iris/spectral_allocator.h"
#include "iris/vector.h"

namespace iris {
namespace bxdfs {
namespace {

using ::iris::bxdfs::internal::AbsCosTheta;
using ::iris::bxdfs::internal::CosineSampleHemisphere;
using ::iris::bxdfs::internal::DisneyDistribution;
using ::iris::bxdfs::internal::DisneyFresnel;
using ::iris::bxdfs::internal::FresnelDielectric;
using ::iris::bxdfs::internal::HalfAngle;
using ::iris::bxdfs::internal::MicrofacetBrdf;
using ::iris::bxdfs::internal::MicrofacetBtdf;
using ::iris::bxdfs::internal::Reflect;
using ::iris::bxdfs::internal::SchlickWeight;
using ::iris::bxdfs::internal::TrowbridgeReitzDistribution;
using ::iris::reflectors::CreateUniformReflector;

static const ReferenceCounted<Reflector> kPerfectReflector =
    CreateUniformReflector(static_cast<visual_t>(1.0));

static inline visual_t FresnelSchlick(visual_t r0, visual_t cos_theta) {
  return std::lerp(r0, static_cast<visual_t>(1.0), SchlickWeight(cos_theta));
}

static inline visual_t Gtr1(visual_t cos_theta, visual_t alpha) {
  visual_t alpha_squared = alpha * alpha;
  visual_t inverse_alpha_squared = alpha_squared - static_cast<visual_t>(1.0);
  visual_t cos_theta_squared = cos_theta * cos_theta;
  return inverse_alpha_squared /
         ((static_cast<visual_t>(1.0) +
           inverse_alpha_squared * cos_theta_squared) *
          std::numbers::pi_v<visual_t> * std::log(alpha_squared));
}

static inline visual_t SmithGgxG1(visual_t cos_theta, visual_t alpha) {
  visual_t alpha_squared = alpha * alpha;
  visual_t cos_theta_squared = cos_theta * cos_theta;
  return static_cast<visual_t>(2.0) /
         (cos_theta + sqrt(alpha_squared + cos_theta_squared -
                           alpha_squared * cos_theta_squared));
}

static inline Vector SphericalDirection(geometric_t sin_theta,
                                        geometric_t cos_theta,
                                        geometric_t phi) {
  return Vector(sin_theta * std::cos(phi), cos_theta * std::sin(phi),
                cos_theta);
}

class DisneyClearcoatBrdf final : public internal::DiffuseBxdf {
 public:
  DisneyClearcoatBrdf(visual_t weight, visual_t alpha) noexcept
      : weight_(weight), alpha_(alpha) {}

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
  visual_t weight_;
  visual_t alpha_;
};

std::optional<Vector> DisneyClearcoatBrdf::SampleDiffuse(
    const Vector& incoming, const Vector& surface_normal,
    Sampler& sampler) const {
  // TODO: Move this to VNDF sampling
  if (incoming.z == static_cast<geometric>(0.0)) {
    return std::nullopt;
  }

  visual_t alpha_squared = alpha_ * alpha_;
  visual_t cos_theta = std::sqrt(std::max(
      static_cast<visual_t>(0.0),
      (static_cast<visual_t>(1.0) -
       std::pow(alpha_squared, static_cast<visual_t>(1.0) - sampler.Next())) /
          (static_cast<visual_t>(1.0) - alpha_squared)));
  visual_t sin_theta =
      std::sqrt(std::max(static_cast<visual_t>(0.0),
                         static_cast<visual_t>(1.0) - cos_theta * cos_theta));
  visual_t phi = static_cast<visual_t>(2.0) * std::numbers::pi_v<visual_t> *
                 sampler.Next();

  Vector sampled = SphericalDirection(sin_theta, cos_theta, phi);
  Vector half_angle =
      std::signbit(incoming.z) == std::signbit(sampled.z) ? sampled : -sampled;

  return Reflect(incoming, half_angle);
}

visual_t DisneyClearcoatBrdf::PdfDiffuse(const Vector& incoming,
                                         const Vector& outgoing,
                                         const Vector& surface_normal,
                                         Hemisphere hemisphere) const {
  if (hemisphere != Hemisphere::BRDF) {
    return static_cast<visual_t>(0.0);
  }

  std::optional<Vector> half_angle = HalfAngle(incoming, outgoing);
  if (!half_angle) {
    return static_cast<visual_t>(0.0);
  }

  visual_t cos_theta = AbsCosTheta(*half_angle);
  return Gtr1(cos_theta, alpha_) * cos_theta /
         (static_cast<visual_t>(4.0) * DotProduct(incoming, *half_angle));
}

const Reflector* DisneyClearcoatBrdf::ReflectanceDiffuse(
    const Vector& incoming, const Vector& outgoing, Hemisphere hemisphere,
    SpectralAllocator& allocator) const {
  if (hemisphere != Hemisphere::BRDF) {
    return nullptr;
  }

  std::optional<Vector> half_angle = HalfAngle(incoming, outgoing);
  if (!half_angle) {
    return nullptr;
  }

  visual_t weight = static_cast<visual_t>(0.25) * weight_;
  weight *= Gtr1(AbsCosTheta(*half_angle), alpha_);
  weight *= FresnelSchlick(static_cast<visual_t>(0.04),
                           DotProduct(incoming, outgoing));
  weight *= SmithGgxG1(AbsCosTheta(incoming), static_cast<visual_t>(0.25));
  weight *= SmithGgxG1(AbsCosTheta(outgoing), static_cast<visual_t>(0.25));

  return allocator.Scale(allocator.Invert(nullptr), weight);
}

class DisneyDiffuseBrdf final : public internal::DiffuseBxdf {
 public:
  DisneyDiffuseBrdf(const Reflector& color) noexcept : color_(color) {}

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
  const Reflector& color_;
};

std::optional<Vector> DisneyDiffuseBrdf::SampleDiffuse(
    const Vector& incoming, const Vector& surface_normal,
    Sampler& sampler) const {
  Vector outgoing = CosineSampleHemisphere(incoming.z, sampler);
  return outgoing.AlignWith(surface_normal);
}

visual_t DisneyDiffuseBrdf::PdfDiffuse(const Vector& incoming,
                                       const Vector& outgoing,
                                       const Vector& surface_normal,
                                       Hemisphere hemisphere) const {
  if (hemisphere != Hemisphere::BRDF) {
    return static_cast<visual_t>(0.0);
  }

  return std::abs(static_cast<visual_t>(outgoing.z) *
                  std::numbers::inv_pi_v<visual_t>);
}

const Reflector* DisneyDiffuseBrdf::ReflectanceDiffuse(
    const Vector& incoming, const Vector& outgoing, Hemisphere hemisphere,
    SpectralAllocator& allocator) const {
  if (hemisphere != Hemisphere::BRDF) {
    return nullptr;
  }

  visual_t weight =
      (static_cast<visual_t>(1.0) -
       static_cast<visual_t>(0.5) * SchlickWeight(AbsCosTheta(incoming))) *
      (static_cast<visual_t>(1.0) -
       static_cast<visual_t>(0.5) * SchlickWeight(AbsCosTheta(outgoing)));

  return allocator.Scale(&color_, std::numbers::inv_pi_v<visual_t> * weight);
}

class DisneyDiffuseRetroBrdf final : public internal::DiffuseBxdf {
 public:
  DisneyDiffuseRetroBrdf(const Reflector& color, visual_t roughness) noexcept
      : color_(color), roughness_(roughness) {}

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
  const Reflector& color_;
  visual_t roughness_;
};

std::optional<Vector> DisneyDiffuseRetroBrdf::SampleDiffuse(
    const Vector& incoming, const Vector& surface_normal,
    Sampler& sampler) const {
  Vector outgoing = CosineSampleHemisphere(incoming.z, sampler);
  return outgoing.AlignWith(surface_normal);
}

visual_t DisneyDiffuseRetroBrdf::PdfDiffuse(const Vector& incoming,
                                            const Vector& outgoing,
                                            const Vector& surface_normal,
                                            Hemisphere hemisphere) const {
  if (hemisphere != Hemisphere::BRDF) {
    return static_cast<visual_t>(0.0);
  }

  return std::abs(static_cast<visual_t>(outgoing.z) *
                  std::numbers::inv_pi_v<visual_t>);
}

const Reflector* DisneyDiffuseRetroBrdf::ReflectanceDiffuse(
    const Vector& incoming, const Vector& outgoing, Hemisphere hemisphere,
    SpectralAllocator& allocator) const {
  if (hemisphere != Hemisphere::BRDF) {
    return nullptr;
  }

  std::optional<Vector> half_angle = HalfAngle(incoming, outgoing);
  if (!half_angle) {
    return nullptr;
  }

  visual_t f_incoming = SchlickWeight(AbsCosTheta(incoming));
  visual_t f_outgoing = SchlickWeight(AbsCosTheta(outgoing));

  visual_t cos_theta_half_angle =
      static_cast<visual_t>(DotProduct(outgoing, *half_angle));
  visual_t retro_weight = static_cast<visual_t>(2.0) * roughness_ *
                          cos_theta_half_angle * cos_theta_half_angle;

  retro_weight *=
      (retro_weight - static_cast<visual_t>(1.0)) * f_outgoing * f_incoming +
      f_outgoing + f_incoming;

  return allocator.Scale(&color_,
                         std::numbers::inv_pi_v<visual_t> * retro_weight);
}

class DisneySheenBrdf final : public internal::DiffuseBxdf {
 public:
  DisneySheenBrdf(const Reflector& color, visual_t sheen) noexcept
      : color_(color), sheen_(sheen) {}

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
  const Reflector& color_;
  visual_t sheen_;
};

std::optional<Vector> DisneySheenBrdf::SampleDiffuse(
    const Vector& incoming, const Vector& surface_normal,
    Sampler& sampler) const {
  Vector outgoing = CosineSampleHemisphere(incoming.z, sampler);
  return outgoing.AlignWith(surface_normal);
}

visual_t DisneySheenBrdf::PdfDiffuse(const Vector& incoming,
                                     const Vector& outgoing,
                                     const Vector& surface_normal,
                                     Hemisphere hemisphere) const {
  if (hemisphere != Hemisphere::BRDF) {
    return static_cast<visual_t>(0.0);
  }

  return std::abs(static_cast<visual_t>(outgoing.z) *
                  std::numbers::inv_pi_v<visual_t>);
}

const Reflector* DisneySheenBrdf::ReflectanceDiffuse(
    const Vector& incoming, const Vector& outgoing, Hemisphere hemisphere,
    SpectralAllocator& allocator) const {
  if (hemisphere != Hemisphere::BRDF) {
    return nullptr;
  }

  std::optional<Vector> half_angle = HalfAngle(incoming, outgoing);
  if (!half_angle) {
    return nullptr;
  }

  visual_t cos_theta_half_angle =
      static_cast<visual_t>(DotProduct(outgoing, *half_angle));

  return allocator.Scale(&color_, sheen_ * SchlickWeight(cos_theta_half_angle));
}

class DisneySubsurfaceBrdf final : public internal::DiffuseBxdf {
 public:
  DisneySubsurfaceBrdf(const Reflector& color, visual_t roughness) noexcept
      : color_(color), roughness_squared_(roughness * roughness) {}

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
  const Reflector& color_;
  visual_t roughness_squared_;
};

std::optional<Vector> DisneySubsurfaceBrdf::SampleDiffuse(
    const Vector& incoming, const Vector& surface_normal,
    Sampler& sampler) const {
  Vector outgoing = CosineSampleHemisphere(incoming.z, sampler);
  return outgoing.AlignWith(surface_normal);
}

visual_t DisneySubsurfaceBrdf::PdfDiffuse(const Vector& incoming,
                                          const Vector& outgoing,
                                          const Vector& surface_normal,
                                          Hemisphere hemisphere) const {
  if (hemisphere != Hemisphere::BRDF) {
    return static_cast<visual_t>(0.0);
  }

  return std::abs(static_cast<visual_t>(outgoing.z) *
                  std::numbers::inv_pi_v<visual_t>);
}

const Reflector* DisneySubsurfaceBrdf::ReflectanceDiffuse(
    const Vector& incoming, const Vector& outgoing, Hemisphere hemisphere,
    SpectralAllocator& allocator) const {
  if (hemisphere != Hemisphere::BRDF) {
    return nullptr;
  }

  std::optional<Vector> half_angle = HalfAngle(incoming, outgoing);
  if (!half_angle) {
    return nullptr;
  }

  visual_t cos_theta_half_angle =
      static_cast<visual_t>(DotProduct(outgoing, *half_angle));
  visual_t f_ss90 =
      cos_theta_half_angle * cos_theta_half_angle * roughness_squared_;
  visual_t f_ss = std::lerp(SchlickWeight(AbsCosTheta(incoming)),
                            static_cast<visual_t>(1.0), f_ss90) *
                  std::lerp(SchlickWeight(AbsCosTheta(outgoing)),
                            static_cast<visual_t>(1.0), f_ss90);
  visual_t ss_weight =
      static_cast<visual_t>(1.25) *
      (f_ss * (static_cast<visual_t>(1.0) /
                   (AbsCosTheta(outgoing) + AbsCosTheta(incoming)) -
               static_cast<visual_t>(0.5)) +
       static_cast<visual_t>(0.5));

  return allocator.Scale(&color_, std::numbers::inv_pi_v<visual_t> * ss_weight);
}

geometric_t ComputeAspectRatio(geometric_t anisotropic) {
  return std::sqrt(static_cast<geometric_t>(1.0) -
                   std::min(static_cast<geometric_t>(1.0), anisotropic) *
                       static_cast<geometric_t>(0.9));
}

DisneyDistribution MakeDisneyDistribution(geometric_t anisotropic,
                                          geometric_t roughness) {
  geometric_t aspect_ratio = ComputeAspectRatio(anisotropic);
  geometric_t alpha_x = std::max(static_cast<geometric_t>(0.001),
                                 (roughness * roughness) / aspect_ratio);
  geometric_t alpha_y = std::max(static_cast<geometric_t>(0.001),
                                 (roughness * roughness) * aspect_ratio);
  return DisneyDistribution(alpha_x, alpha_y);
}

}  // namespace

const Bxdf* MakeDisneyClearcoatBrdf(BxdfAllocator& bxdf_allocator,
                                    visual_t weight, visual_t gloss) {
  return &bxdf_allocator.Allocate<DisneyClearcoatBrdf>(
      weight, std::lerp(static_cast<visual_t>(0.1),
                        static_cast<visual_t>(0.001), gloss));
}

const Bxdf* MakeDisneyDiffuseBrdf(BxdfAllocator& bxdf_allocator,
                                  const Reflector* color) {
  if (!color) {
    return nullptr;
  }

  return &bxdf_allocator.Allocate<DisneyDiffuseBrdf>(*color);
}

const Bxdf* MakeDisneyDiffuseRetroBrdf(BxdfAllocator& bxdf_allocator,
                                       const Reflector* color,
                                       visual_t roughness) {
  if (!color) {
    return nullptr;
  }

  return &bxdf_allocator.Allocate<DisneyDiffuseRetroBrdf>(*color, roughness);
}

const Bxdf* MakeDisneyMicrofacetBtdf(BxdfAllocator& bxdf_allocator,
                                     const Reflector* color,
                                     geometric_t eta_incident,
                                     geometric_t eta_transmitted,
                                     geometric_t anisotropic,
                                     geometric_t roughness) {
  if (!color || !FresnelDielectric(eta_incident, eta_transmitted).IsValid()) {
    return nullptr;
  }

  if (!std::isfinite(anisotropic) ||
      anisotropic < static_cast<geometric_t>(0.0) ||
      !std::isfinite(roughness) || roughness < static_cast<geometric_t>(0.0)) {
    return nullptr;
  }

  return &bxdf_allocator.Allocate<MicrofacetBtdf<DisneyDistribution>>(
      *color, MakeDisneyDistribution(anisotropic, roughness), eta_incident,
      eta_transmitted);
}

const Bxdf* MakeDisneyThinMicrofacetBtdf(BxdfAllocator& bxdf_allocator,
                                         const Reflector* color,
                                         geometric_t eta_incident,
                                         geometric_t eta_transmitted,
                                         geometric_t anisotropic,
                                         geometric_t roughness) {
  if (!color || !FresnelDielectric(eta_incident, eta_transmitted).IsValid()) {
    return nullptr;
  }

  if (!std::isfinite(anisotropic) ||
      anisotropic < static_cast<geometric_t>(0.0) ||
      !std::isfinite(roughness) || roughness < static_cast<geometric_t>(0.0)) {
    return nullptr;
  }

  geometric_t aspect_ratio = ComputeAspectRatio(anisotropic);
  roughness *= static_cast<geometric_t>(0.65) * eta_transmitted -
               static_cast<geometric_t>(0.35);
  geometric_t alpha_x = std::max(static_cast<geometric_t>(0.001),
                                 (roughness * roughness) / aspect_ratio);
  geometric_t alpha_y = std::max(static_cast<geometric_t>(0.001),
                                 (roughness * roughness) * aspect_ratio);

  return &bxdf_allocator.Allocate<MicrofacetBtdf<TrowbridgeReitzDistribution>>(
      *color,
      TrowbridgeReitzDistribution(alpha_x, alpha_y, /*is_roughness=*/false),
      eta_incident, eta_transmitted);
}

const Bxdf* MakeDisneyMicrofacetBrdf(BxdfAllocator& bxdf_allocator,
                                     const Reflector* color,
                                     visual_t specular_tint, visual_t metallic,
                                     geometric_t eta_incident,
                                     geometric_t eta_transmitted,
                                     geometric_t anisotropic,
                                     geometric_t roughness) {
  // TODO: Handle specular_tint
  DisneyFresnel fresnel(nullptr, metallic, eta_incident, eta_transmitted);
  if (!fresnel.IsValid()) {
    return nullptr;
  }

  if (!std::isfinite(anisotropic) ||
      anisotropic < static_cast<geometric_t>(0.0) ||
      !std::isfinite(roughness) || roughness < static_cast<geometric_t>(0.0)) {
    return nullptr;
  }

  return &bxdf_allocator
              .Allocate<MicrofacetBrdf<DisneyDistribution, DisneyFresnel>>(
                  *kPerfectReflector,
                  MakeDisneyDistribution(anisotropic, roughness), fresnel);
}

const Bxdf* MakeDisneySheenBrdf(BxdfAllocator& bxdf_allocator,
                                const Reflector* color, visual_t sheen,
                                visual_t sheen_tint) {
  // TODO: Handle sheen_tint
  if (!color || !std::isfinite(sheen) || sheen <= static_cast<visual_t>(0.0) ||
      !std::isfinite(sheen_tint) || sheen_tint <= static_cast<visual_t>(0.0)) {
    return nullptr;
  }

  return &bxdf_allocator.Allocate<DisneySheenBrdf>(*kPerfectReflector, sheen);
}

const Bxdf* MakeDisneySubsurfaceBrdf(BxdfAllocator& bxdf_allocator,
                                     const Reflector* color,
                                     visual_t roughness) {
  if (!color) {
    return nullptr;
  }

  return &bxdf_allocator.Allocate<DisneySubsurfaceBrdf>(*color, roughness);
}

}  // namespace bxdfs
}  // namespace iris
