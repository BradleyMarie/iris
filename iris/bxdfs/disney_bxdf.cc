#include "iris/bxdfs/disney_bxdf.h"

#include <numbers>

#include "iris/bxdf.h"
#include "iris/bxdf_allocator.h"
#include "iris/bxdfs/internal/diffuse_bxdf.h"
#include "iris/bxdfs/internal/math.h"
#include "iris/float.h"
#include "iris/reflector.h"
#include "iris/sampler.h"
#include "iris/spectral_allocator.h"
#include "iris/vector.h"

namespace iris {
namespace bxdfs {
namespace {

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
  Vector outgoing = internal::CosineSampleHemisphere(incoming.z, sampler);
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
       static_cast<visual_t>(0.5) *
           internal::SchlickWeight(internal::AbsCosTheta(incoming))) *
      (static_cast<visual_t>(1.0) -
       static_cast<visual_t>(0.5) *
           internal::SchlickWeight(internal::AbsCosTheta(outgoing)));

  return allocator.Scale(&color_, std::numbers::inv_pi_v<visual_t> * weight);
}

class DisneySheenBrdf final : public internal::DiffuseBxdf {
 public:
  DisneySheenBrdf(const Reflector& color) noexcept : color_(color) {}

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

std::optional<Vector> DisneySheenBrdf::SampleDiffuse(
    const Vector& incoming, const Vector& surface_normal,
    Sampler& sampler) const {
  Vector outgoing = internal::CosineSampleHemisphere(incoming.z, sampler);
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

  std::optional<Vector> half_angle = internal::HalfAngle(incoming, outgoing);
  if (!half_angle) {
    return nullptr;
  }

  visual_t cos_theta_half_angle =
      static_cast<visual_t>(DotProduct(outgoing, *half_angle));

  return allocator.Scale(&color_,
                         internal::SchlickWeight(cos_theta_half_angle));
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
  Vector outgoing = internal::CosineSampleHemisphere(incoming.z, sampler);
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

  std::optional<Vector> half_angle = internal::HalfAngle(incoming, outgoing);
  if (!half_angle) {
    return nullptr;
  }

  visual_t cos_theta_half_angle =
      static_cast<visual_t>(DotProduct(outgoing, *half_angle));
  visual_t f_ss90 =
      cos_theta_half_angle * cos_theta_half_angle * roughness_squared_;
  visual_t f_ss =
      std::lerp(internal::SchlickWeight(internal::AbsCosTheta(incoming)),
                static_cast<visual_t>(1.0), f_ss90) *
      std::lerp(internal::SchlickWeight(internal::AbsCosTheta(outgoing)),
                static_cast<visual_t>(1.0), f_ss90);
  visual_t ss_weight =
      static_cast<visual_t>(1.25) *
      (f_ss * (static_cast<visual_t>(1.0) / (internal::AbsCosTheta(outgoing) +
                                             internal::AbsCosTheta(incoming)) -
               static_cast<visual_t>(0.5)) +
       static_cast<visual_t>(0.5));

  return allocator.Scale(&color_, std::numbers::inv_pi_v<visual_t> * ss_weight);
}

}  // namespace

const Bxdf* MakeDisneyDiffuseBrdf(BxdfAllocator& bxdf_allocator,
                                  const Reflector* color) {
  if (!color) {
    return nullptr;
  }

  return &bxdf_allocator.Allocate<DisneyDiffuseBrdf>(*color);
}

const Bxdf* MakeDisneySheenBrdf(BxdfAllocator& bxdf_allocator,
                                const Reflector* color) {
  if (!color) {
    return nullptr;
  }

  return &bxdf_allocator.Allocate<DisneySheenBrdf>(*color);
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
