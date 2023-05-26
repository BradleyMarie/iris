#ifndef _IRIS_BXDFS_MICROFACET_BXDF_
#define _IRIS_BXDFS_MICROFACET_BXDF_

#include <concepts>

#include "iris/bxdf.h"
#include "iris/bxdfs/fresnel.h"
#include "iris/bxdfs/math.h"
#include "iris/float.h"
#include "iris/reflector.h"
#include "iris/sampler.h"
#include "iris/spectral_allocator.h"
#include "iris/vector.h"

namespace iris {
namespace bxdfs {

class MicrofacetDistribution {
 public:
  visual_t G(const Vector& incoming, const Vector& outgoing) const;
  visual_t G1(const Vector& vector) const;
  visual_t Pdf(const Vector& outgoing, const Vector& half_angle) const;

  virtual visual_t D(const Vector& vector) const = 0;
  virtual visual_t Lambda(const Vector& vector) const = 0;
  virtual Vector Sample(const Vector& incoming, geometric_t u,
                        geometric_t v) const = 0;
};

class TrowbridgeReitzDistribution : public MicrofacetDistribution {
 public:
  TrowbridgeReitzDistribution(visual alpha_x, visual alpha_y)
      : alpha_x_(alpha_x), alpha_y_(alpha_y) {}

  virtual visual_t D(const Vector& vector) const override;
  virtual visual_t Lambda(const Vector& vector) const override;
  virtual Vector Sample(const Vector& incoming, geometric_t u,
                        geometric_t v) const override;

  static visual_t RoughnessToAlpha(visual_t roughness);

 private:
  visual alpha_x_, alpha_y_;
};

template <typename M, typename F>
  requires std::derived_from<M, MicrofacetDistribution> &&
           std::derived_from<F, Fresnel>
class MicrofacetBrdf final : public Bxdf {
 public:
  MicrofacetBrdf(const Reflector& reflectance, const M& distribution,
                 const F& fresnel) noexcept
      : reflectance_(reflectance),
        distribution_(distribution),
        fresnel_(fresnel) {}

  std::optional<SampleResult> Sample(
      const Vector& incoming, const std::optional<Differentials>& differentials,
      Sampler& sampler) const override {
    if (incoming.z == static_cast<geometric_t>(0.0)) {
      return std::nullopt;
    }

    Vector half_angle =
        distribution_.Sample(incoming, sampler.Next(), sampler.Next());
    if (DotProduct(incoming, half_angle) <= static_cast<geometric_t>(0.0)) {
      return std::nullopt;
    }

    Vector outgoing = internal::Reflect(incoming, half_angle);
    if ((incoming.z < static_cast<geometric_t>(0.0)) !=
        (outgoing.z < static_cast<geometric_t>(0.0))) {
      return std::nullopt;
    }

    return {{outgoing}};
  }

  std::optional<visual_t> Pdf(const Vector& incoming, const Vector& outgoing,
                              SampleSource sample_source) const override {
    if ((incoming.z < static_cast<geometric_t>(0.0)) !=
        (outgoing.z < static_cast<geometric_t>(0.0))) {
      return static_cast<geometric_t>(0.0);
    }

    Vector half_angle = Normalize(outgoing + incoming);
    return distribution_.Pdf(outgoing, half_angle) /
           (static_cast<visual_t>(4.0) *
            static_cast<visual_t>(DotProduct(outgoing, half_angle)));
  }

  const Reflector* Reflectance(const Vector& incoming, const Vector& outgoing,
                               SampleSource sample_source,
                               Hemisphere hemisphere,
                               SpectralAllocator& allocator) const override {
    if (hemisphere != Hemisphere::BRDF) {
      return nullptr;
    }

    if ((incoming.z == static_cast<geometric_t>(0.0)) ||
        (outgoing.z == static_cast<geometric_t>(0.0))) {
      return nullptr;
    }

    if ((incoming.z < static_cast<geometric_t>(0.0)) !=
        (outgoing.z < static_cast<geometric_t>(0.0))) {
      return nullptr;
    }

    Vector half_angle = Normalize(outgoing + incoming);
    visual_t cos_theta_i = internal::AbsCosTheta(incoming);
    visual_t cos_theta_o = internal::AbsCosTheta(outgoing);

    const Reflector* reflectance = allocator.Scale(
        fresnel_.Evaluate(DotProduct(incoming, half_angle), allocator),
        &reflectance_);

    return allocator.Scale(
        reflectance,
        distribution_.D(half_angle) * distribution_.G(incoming, outgoing) /
            (static_cast<visual_t>(4.0) * cos_theta_i * cos_theta_o));
  }

 private:
  const Reflector& reflectance_;
  const M distribution_;
  const F fresnel_;
};

}  // namespace bxdfs
}  // namespace iris

#endif  // _IRIS_BXDFS_MICROFACET_BXDF_