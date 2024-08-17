#define _USE_MATH_DEFINES
#include "iris/bxdfs/oren_nayar_bxdf.h"

#include <algorithm>
#include <cassert>
#include <cmath>

#include "iris/bxdfs/internal/math.h"
#include "iris/bxdfs/math.h"

namespace iris {
namespace bxdfs {
namespace {

visual_t ComputeA(visual_t sigma) {
  visual_t sigma_radians = sigma * static_cast<visual_t>(M_PI / 180.0);
  visual_t sigma_squared = sigma_radians * sigma_radians;
  return static_cast<visual_t>(1.0) -
         (sigma_squared / (static_cast<visual_t>(2.0) *
                           (sigma_squared + static_cast<visual_t>(0.33))));
}

visual_t ComputeB(visual_t sigma) {
  visual_t sigma_radians = sigma * static_cast<visual_t>(M_PI / 180.0);
  visual_t sigma_squared = sigma_radians * sigma_radians;
  return static_cast<visual_t>(0.45) * sigma_squared /
         (sigma_squared + static_cast<visual_t>(0.09));
}

}  // namespace

OrenNayarBrdf::OrenNayarBrdf(const Reflector& reflector, visual_t sigma)
    : reflector_(reflector), a_(ComputeA(sigma)), b_(ComputeB(sigma)) {
  assert(sigma >= static_cast<visual_t>(0.0));
}

bool OrenNayarBrdf::IsDiffuse() const { return true; }

std::optional<Vector> OrenNayarBrdf::SampleDiffuse(const Vector& incoming,
                                                   const Vector& surface_normal,
                                                   Sampler& sampler) const {
  Vector outgoing = internal::CosineSampleHemisphere(incoming.z, sampler);
  return outgoing.AlignWith(surface_normal);
}

std::optional<Bxdf::SampleResult> OrenNayarBrdf::Sample(
    const Vector& incoming,
    const std::optional<Bxdf::Differentials>& differentials,
    const Vector& surface_normal, Sampler& sampler) const {
  return SampleResult{*SampleDiffuse(incoming, surface_normal, sampler)};
}

visual_t OrenNayarBrdf::Pdf(const Vector& incoming, const Vector& outgoing,
                            const Vector& surface_normal,
                            Hemisphere hemisphere) const {
  if (hemisphere != Hemisphere::BRDF) {
    return static_cast<visual_t>(0.0);
  }

  return static_cast<visual_t>(std::abs(outgoing.z * M_1_PI));
}

const Reflector* OrenNayarBrdf::Reflectance(
    const Vector& incoming, const Vector& outgoing, Hemisphere hemisphere,
    SpectralAllocator& allocator) const {
  if (hemisphere != Hemisphere::BRDF) {
    return nullptr;
  }

  auto [sin_phi_incoming, cos_phi_incoming] = internal::SinCosPhi(incoming);
  auto [sin_phi_outgoing, cos_phi_outgoing] = internal::SinCosPhi(outgoing);
  geometric_t cosine = std::max(static_cast<geometric_t>(0.0),
                                cos_phi_outgoing * cos_phi_incoming +
                                    sin_phi_outgoing * sin_phi_incoming);

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

  return allocator.Scale(&reflector_,
                         M_1_PI * (a_ + b_ * cosine * sin_alpha * tan_beta));
}

}  // namespace bxdfs
}  // namespace iris