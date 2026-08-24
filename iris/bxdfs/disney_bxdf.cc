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
#include "iris/color.h"
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
using ::iris::bxdfs::internal::SphericalDirection;
using ::iris::bxdfs::internal::TrowbridgeReitzDistribution;
using ::iris::reflectors::CreateUniformReflector;

static const ReferenceCounted<Reflector> kPerfectReflector =
    CreateUniformReflector(static_cast<visual_t>(1.0));

static inline visual_t Gtr1(visual_t cos_theta, visual_t alpha) {
  visual_t alpha_squared = alpha * alpha;
  visual_t inverse_alpha_squared = alpha_squared - static_cast<visual_t>(1.0);
  visual_t cos_theta_squared = cos_theta * cos_theta;
  return inverse_alpha_squared /
         ((static_cast<visual_t>(1.0) +
           inverse_alpha_squared * cos_theta_squared) *
          std::numbers::pi_v<visual_t> * std::log(alpha_squared));
}

static inline visual_t SmithGGgxG1(visual_t cos_theta, visual_t alpha) {
  visual_t alpha_squared = alpha * alpha;
  visual_t cos_theta_squared = cos_theta * cos_theta;
  // Numerator is set to 1 instead of 2 to save a divide by 4 in the caller
  return static_cast<visual_t>(1.0) /
         (cos_theta + sqrt(alpha_squared + cos_theta_squared -
                           alpha_squared * cos_theta_squared));
}

class DisneyBrdfBase : public internal::DiffuseBxdf {
 public:
  DisneyBrdfBase() {}

  std::optional<Vector> SampleDiffuse(const Vector& incoming,
                                      const Vector& surface_normal,
                                      Sampler& sampler) const override final;

  visual_t PdfDiffuse(const Vector& incoming, const Vector& outgoing,
                      const Vector& surface_normal,
                      Hemisphere hemisphere) const override final;
};

std::optional<Vector> DisneyBrdfBase::SampleDiffuse(
    const Vector& incoming, const Vector& surface_normal,
    Sampler& sampler) const {
  Vector outgoing = CosineSampleHemisphere(incoming.z, sampler);
  return outgoing.AlignWith(surface_normal);
}

visual_t DisneyBrdfBase::PdfDiffuse(const Vector& incoming,
                                    const Vector& outgoing,
                                    const Vector& surface_normal,
                                    Hemisphere hemisphere) const {
  if (hemisphere != Hemisphere::BRDF) {
    return static_cast<visual_t>(0.0);
  }

  return std::abs(static_cast<visual_t>(outgoing.z) *
                  std::numbers::inv_pi_v<visual_t>);
}

class DisneyClearcoatBrdf final : public internal::DiffuseBxdf {
 public:
  DisneyClearcoatBrdf(visual_t clearcoat, visual_t alpha) noexcept
      : clearcoat_(clearcoat), alpha_(alpha) {}

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
  visual_t clearcoat_;
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

  visual_t dp = DotProduct(incoming, *half_angle);
  if (dp == static_cast<visual_t>(0.0)) {
    return static_cast<visual_t>(0.0);
  }

  visual_t cos_theta = AbsCosTheta(*half_angle);
  return Gtr1(cos_theta, alpha_) * cos_theta /
         (static_cast<visual_t>(4.0) * dp);
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

  visual_t weight = Gtr1(AbsCosTheta(*half_angle), alpha_);
  weight *= std::lerp(static_cast<visual_t>(0.04), static_cast<visual_t>(1.0),
                      SchlickWeight(DotProduct(*half_angle, outgoing)));
  weight *= SmithGGgxG1(AbsCosTheta(incoming), static_cast<visual_t>(0.25));
  weight *= SmithGGgxG1(AbsCosTheta(outgoing), static_cast<visual_t>(0.25));
  weight *= clearcoat_;

  return allocator.UnboundedScale(allocator.Invert(nullptr), weight);
}

class DisneyDiffuseBrdf final : public DisneyBrdfBase {
 public:
  DisneyDiffuseBrdf(const Reflector& color) noexcept : color_(color) {}

  const Reflector* ReflectanceDiffuse(
      const Vector& incoming, const Vector& outgoing, Hemisphere hemisphere,
      SpectralAllocator& allocator) const override;

 private:
  const Reflector& color_;
};

const Reflector* DisneyDiffuseBrdf::ReflectanceDiffuse(
    const Vector& incoming, const Vector& outgoing, Hemisphere hemisphere,
    SpectralAllocator& allocator) const {
  if (hemisphere != Hemisphere::BRDF) {
    return nullptr;
  }

  visual_t fl = SchlickWeight(AbsCosTheta(outgoing));
  visual_t fv = SchlickWeight(AbsCosTheta(incoming));
  visual_t fd = std::numbers::inv_pi_v<visual_t> *
                (static_cast<visual_t>(1.0) - static_cast<visual_t>(0.5) * fl) *
                (static_cast<visual_t>(1.0) - static_cast<visual_t>(0.5) * fv);

  return allocator.UnboundedScale(&color_, fd);
}

class DisneyDiffuseRetroBrdf final : public DisneyBrdfBase {
 public:
  DisneyDiffuseRetroBrdf(const Reflector& color, visual_t roughness) noexcept
      : color_(color), roughness_(roughness) {}

  const Reflector* ReflectanceDiffuse(
      const Vector& incoming, const Vector& outgoing, Hemisphere hemisphere,
      SpectralAllocator& allocator) const override;

 private:
  const Reflector& color_;
  visual_t roughness_;
};

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

  visual_t fv = SchlickWeight(AbsCosTheta(incoming));
  visual_t fl = SchlickWeight(AbsCosTheta(outgoing));
  visual_t cos_theta_d =
      static_cast<visual_t>(DotProduct(outgoing, *half_angle));
  visual_t rr =
      static_cast<visual_t>(2.0) * roughness_ * cos_theta_d * cos_theta_d;
  rr *= fl + fv + fl * fv * (rr - static_cast<visual_t>(1.0));

  return allocator.UnboundedScale(&color_,
                                  std::numbers::inv_pi_v<visual_t> * rr);
}

class DisneySheenBrdf final : public DisneyBrdfBase {
 public:
  DisneySheenBrdf(const Reflector* color, visual_t sheen,
                  visual_t sheen_tint) noexcept
      : color_(color), sheen_(sheen), sheen_tint_(sheen_tint) {}

  const Reflector* ReflectanceDiffuse(
      const Vector& incoming, const Vector& outgoing, Hemisphere hemisphere,
      SpectralAllocator& allocator) const override;

 private:
  const Reflector* color_;
  visual_t sheen_;
  visual_t sheen_tint_;
};

const Reflector* DisneySheenBrdf::ReflectanceDiffuse(
    const Vector& incoming, const Vector& outgoing, Hemisphere hemisphere,
    SpectralAllocator& allocator) const {
  if (hemisphere != Hemisphere::BRDF) {
    return nullptr;
  }

  // TODO: Make this something SpectralAllocator can do
  visual_t color_luma = static_cast<visual_t>(0.0);
  if (color_) {
    visual_t r = color_->Reflectance(static_cast<visual_t>(0.5));
    visual_t g = color_->Reflectance(static_cast<visual_t>(1.5));
    visual_t b = color_->Reflectance(static_cast<visual_t>(2.5));
    color_luma = Color(r, g, b, Color::LINEAR_SRGB).Luma();
  }

  const Reflector* color_tint = allocator.Invert(nullptr);
  if (color_luma > static_cast<visual_t>(0.0)) {
    color_tint = allocator.UnboundedScale(
        color_, static_cast<visual_t>(1.0) / color_luma);
  }

  const Reflector* color_sheen =
      allocator.Lerp(allocator.Invert(nullptr), color_tint, sheen_tint_);

  std::optional<Vector> half_angle = HalfAngle(incoming, outgoing);
  if (!half_angle) {
    return nullptr;
  }

  visual_t cos_theta_d =
      static_cast<visual_t>(DotProduct(outgoing, *half_angle));
  visual_t fh = SchlickWeight(cos_theta_d);

  return allocator.UnboundedScale(color_sheen, sheen_ * fh);
}

class DisneySubsurfaceBrdf final : public DisneyBrdfBase {
 public:
  DisneySubsurfaceBrdf(const Reflector& color, visual_t roughness) noexcept
      : color_(color), roughness_(roughness) {}

  const Reflector* ReflectanceDiffuse(
      const Vector& incoming, const Vector& outgoing, Hemisphere hemisphere,
      SpectralAllocator& allocator) const override;

 private:
  const Reflector& color_;
  visual_t roughness_;
};

const Reflector* DisneySubsurfaceBrdf::ReflectanceDiffuse(
    const Vector& incoming, const Vector& outgoing, Hemisphere hemisphere,
    SpectralAllocator& allocator) const {
  if (hemisphere != Hemisphere::BRDF) {
    return nullptr;
  }

  visual_t cos_theta_incoming = AbsCosTheta(incoming);
  visual_t cos_theta_outgoing = AbsCosTheta(outgoing);
  if (cos_theta_incoming == static_cast<visual_t>(0.0) &&
      cos_theta_outgoing == static_cast<visual_t>(0.0)) {
    return nullptr;
  }

  std::optional<Vector> half_angle = HalfAngle(incoming, outgoing);
  if (!half_angle) {
    return nullptr;
  }

  visual_t cos_theta_d =
      static_cast<visual_t>(DotProduct(outgoing, *half_angle));
  visual_t f_ss90 = cos_theta_d * cos_theta_d * roughness_;
  visual_t f_ss = std::lerp(static_cast<visual_t>(1.0), f_ss90,
                            SchlickWeight(AbsCosTheta(incoming))) *
                  std::lerp(static_cast<visual_t>(1.0), f_ss90,
                            SchlickWeight(AbsCosTheta(outgoing)));
  visual_t ss = static_cast<visual_t>(1.25) *
                (f_ss * (static_cast<visual_t>(1.0) /
                             (cos_theta_incoming + cos_theta_outgoing) -
                         static_cast<visual_t>(0.5)) +
                 static_cast<visual_t>(0.5));

  return allocator.UnboundedScale(&color_,
                                  std::numbers::inv_pi_v<visual_t> * ss);
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
                                    visual_t clearcoat, visual_t gloss) {
  return &bxdf_allocator.Allocate<DisneyClearcoatBrdf>(
      static_cast<visual_t>(0.25) *
          clearcoat,  // clearcoat is normalized to [0, 0.25]
      std::lerp(static_cast<visual_t>(0.1), static_cast<visual_t>(0.001),
                gloss));
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

const Bxdf* MakeDisneySpecularBrdf(BxdfAllocator& bxdf_allocator,
                                   const Reflector* color, visual_t metallic,
                                   visual_t specular_tint,
                                   geometric_t eta_incident,
                                   geometric_t eta_transmitted,
                                   geometric_t anisotropic,
                                   geometric_t roughness) {
  if (!std::isfinite(specular_tint) ||
      specular_tint < static_cast<geometric_t>(0.0) ||
      !std::isfinite(anisotropic) ||
      anisotropic < static_cast<geometric_t>(0.0) ||
      !std::isfinite(roughness) || roughness < static_cast<geometric_t>(0.0)) {
    return nullptr;
  }

  DisneyFresnel fresnel(color, metallic, specular_tint, eta_incident,
                        eta_transmitted);
  if (!fresnel.IsValid()) {
    return nullptr;
  }

  return &bxdf_allocator
              .Allocate<MicrofacetBrdf<DisneyDistribution, DisneyFresnel>>(
                  *kPerfectReflector,
                  MakeDisneyDistribution(anisotropic, roughness), fresnel);
}

const Bxdf* MakeDisneySpecularBtdf(BxdfAllocator& bxdf_allocator,
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

const Bxdf* MakeDisneyThinSpecularBtdf(BxdfAllocator& bxdf_allocator,
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

const Bxdf* MakeDisneySheenBrdf(BxdfAllocator& bxdf_allocator,
                                const Reflector* color, visual_t sheen,
                                visual_t sheen_tint) {
  if (!std::isfinite(sheen) || sheen <= static_cast<visual_t>(0.0) ||
      !std::isfinite(sheen_tint) || sheen_tint < static_cast<visual_t>(0.0)) {
    return nullptr;
  }

  return &bxdf_allocator.Allocate<DisneySheenBrdf>(color, sheen, sheen_tint);
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
