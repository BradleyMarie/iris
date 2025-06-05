#include "iris/bxdfs/lambertian_bxdf.h"

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

class LambertianBrdf final : public internal::DiffuseBxdf {
 public:
  LambertianBrdf(const Reflector& reflector) noexcept : reflector_(reflector) {}

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
  const Reflector& reflector_;
};

class LambertianBtdf final : public internal::DiffuseBxdf {
 public:
  LambertianBtdf(const Reflector& transmittance) noexcept
      : transmittance_(transmittance) {}

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
  const Reflector& transmittance_;
};

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

}  // namespace

const Bxdf* MakeLambertianBrdf(BxdfAllocator& bxdf_allocator,
                               const Reflector* reflector) {
  if (!reflector) {
    return nullptr;
  }

  return &bxdf_allocator.Allocate<LambertianBrdf>(*reflector);
}

const Bxdf* MakeLambertianBtdf(BxdfAllocator& bxdf_allocator,
                               const Reflector* reflector) {
  if (!reflector) {
    return nullptr;
  }

  return &bxdf_allocator.Allocate<LambertianBtdf>(*reflector);
}

}  // namespace bxdfs
}  // namespace iris
