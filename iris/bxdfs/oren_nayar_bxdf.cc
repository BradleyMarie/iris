#include "iris/bxdfs/oren_nayar_bxdf.h"

#include <algorithm>
#include <cassert>
#include <numbers>

#include "iris/bxdf.h"
#include "iris/bxdf_allocator.h"
#include "iris/bxdfs/helpers/diffuse_bxdf.h"
#include "iris/bxdfs/internal/math.h"
#include "iris/float.h"
#include "iris/reflector.h"
#include "iris/sampler.h"
#include "iris/spectral_allocator.h"
#include "iris/vector.h"

namespace iris {
namespace bxdfs {
namespace {

class OrenNayarBrdf final : public helpers::DiffuseBxdf {
 public:
  OrenNayarBrdf(const Reflector& reflector, visual_t sigma);

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
  const visual_t a_, b_;
};

visual_t ComputeA(visual_t sigma) {
  visual_t sigma_radians =
      sigma * static_cast<visual_t>(std::numbers::pi / 180.0);
  visual_t sigma_squared = sigma_radians * sigma_radians;
  return static_cast<visual_t>(1.0) -
         (sigma_squared / (static_cast<visual_t>(2.0) *
                           (sigma_squared + static_cast<visual_t>(0.33))));
}

visual_t ComputeB(visual_t sigma) {
  visual_t sigma_radians =
      sigma * static_cast<visual_t>(std::numbers::pi / 180.0);
  visual_t sigma_squared = sigma_radians * sigma_radians;
  return static_cast<visual_t>(0.45) * sigma_squared /
         (sigma_squared + static_cast<visual_t>(0.09));
}

OrenNayarBrdf::OrenNayarBrdf(const Reflector& reflector, visual_t sigma)
    : reflector_(reflector), a_(ComputeA(sigma)), b_(ComputeB(sigma)) {
  assert(sigma >= static_cast<visual_t>(0.0));
}

std::optional<Vector> OrenNayarBrdf::SampleDiffuse(const Vector& incoming,
                                                   const Vector& surface_normal,
                                                   Sampler& sampler) const {
  Vector outgoing = internal::CosineSampleHemisphere(incoming.z, sampler);
  return outgoing.AlignWith(surface_normal);
}

visual_t OrenNayarBrdf::PdfDiffuse(const Vector& incoming,
                                   const Vector& outgoing,
                                   const Vector& surface_normal,
                                   Hemisphere hemisphere) const {
  if (hemisphere != Hemisphere::BRDF) {
    return static_cast<visual_t>(0.0);
  }

  return std::abs(static_cast<visual_t>(outgoing.z) *
                  std::numbers::inv_pi_v<visual_t>);
}

const Reflector* OrenNayarBrdf::ReflectanceDiffuse(
    const Vector& incoming, const Vector& outgoing, Hemisphere hemisphere,
    SpectralAllocator& allocator) const {
  if (hemisphere != Hemisphere::BRDF) {
    return nullptr;
  }

  auto [sin_phi_incoming, cos_phi_incoming] = internal::SinCosPhi(incoming);
  auto [sin_phi_outgoing, cos_phi_outgoing] = internal::SinCosPhi(outgoing);
  visual_t cosine =
      std::max(static_cast<visual_t>(0.0),
               static_cast<visual_t>(cos_phi_outgoing * cos_phi_incoming +
                                     sin_phi_outgoing * sin_phi_incoming));

  geometric_t sin_theta_incoming = internal::SinTheta(incoming);
  geometric_t sin_theta_outgoing = internal::SinTheta(outgoing);
  geometric_t abs_cos_theta_incoming = internal::AbsCosTheta(incoming);
  geometric_t abs_cos_theta_outgoing = internal::AbsCosTheta(outgoing);

  visual_t sin_alpha, tan_beta;
  if (abs_cos_theta_outgoing > abs_cos_theta_incoming) {
    sin_alpha = sin_theta_outgoing;
    tan_beta = sin_theta_incoming / abs_cos_theta_outgoing;
  } else {
    sin_alpha = sin_theta_incoming;
    tan_beta = sin_theta_outgoing / abs_cos_theta_incoming;
  }

  return allocator.UnboundedScale(
      &reflector_, std::numbers::inv_pi_v<visual_t> *
                       (a_ + b_ * cosine * sin_alpha * tan_beta));
}

}  // namespace

const Bxdf* MakeOrenNayarBrdf(BxdfAllocator& bxdf_allocator,
                              const Reflector* reflector, visual_t sigma) {
  if (!reflector) {
    return nullptr;
  }

  return &bxdf_allocator.Allocate<OrenNayarBrdf>(*reflector, sigma);
}

}  // namespace bxdfs
}  // namespace iris
