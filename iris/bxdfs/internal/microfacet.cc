#include "iris/bxdfs/internal/microfacet.h"

#include <algorithm>
#include <cmath>
#include <numbers>

#include "iris/bxdfs/internal/math.h"

namespace iris {
namespace bxdfs {
namespace internal {
namespace {

geometric_t ComputeTrowbridgeReitzAlpha(geometric_t roughness_or_alpha,
                                        bool is_roughness) {
  if (is_roughness) {
    geometric_t val =
        std::log(std::max(static_cast<geometric_t>(0.001), roughness_or_alpha));

    roughness_or_alpha =
        static_cast<geometric_t>(1.62142) +
        static_cast<geometric_t>(0.819955) * val +
        static_cast<geometric_t>(0.1734) * val * val +
        static_cast<geometric_t>(0.0171201) * val * val * val +
        static_cast<geometric_t>(0.000640711) * val * val * val * val;
  }

  roughness_or_alpha =
      std::max(static_cast<geometric_t>(0.001), roughness_or_alpha);
  if (!std::isfinite(roughness_or_alpha)) {
    roughness_or_alpha = static_cast<geometric_t>(0.001);
  }

  return roughness_or_alpha;
}

}  // namespace

visual_t MicrofacetDistribution::G(const Vector& incoming,
                                   const Vector& outgoing) const {
  return static_cast<visual_t>(1.0) /
         (static_cast<visual_t>(1.0) + Lambda(incoming) + Lambda(outgoing));
}

visual_t MicrofacetDistribution::G1(const Vector& vector) const {
  return static_cast<visual_t>(1.0) /
         (static_cast<visual_t>(1.0) + Lambda(vector));
}

visual_t MicrofacetDistribution::Pdf(const Vector& incoming,
                                     const Vector& half_angle) const {
  return D(half_angle) * G1(incoming) *
         ClampedAbsDotProduct(incoming, half_angle) / AbsCosTheta(incoming);
}

TrowbridgeReitzDistribution::TrowbridgeReitzDistribution(
    geometric_t roughness_or_alpha_x, geometric_t roughness_or_alpha_y,
    bool is_roughness)
    : alpha_x_(ComputeTrowbridgeReitzAlpha(roughness_or_alpha_x, is_roughness)),
      alpha_y_(
          ComputeTrowbridgeReitzAlpha(roughness_or_alpha_y, is_roughness)) {}

TrowbridgeReitzDistribution::TrowbridgeReitzDistribution(
    geometric_t roughness_or_alpha, bool is_roughness)
    : TrowbridgeReitzDistribution(roughness_or_alpha, roughness_or_alpha,
                                  is_roughness) {}

visual_t TrowbridgeReitzDistribution::D(const Vector& vector) const {
  geometric_t tan_squared_theta = TanSquaredTheta(vector);
  if (std::isinf(tan_squared_theta)) {
    return 0.0;
  }

  geometric_t cos_squared_theta = CosSquaredTheta(vector);
  geometric_t cos_4_theta = cos_squared_theta * cos_squared_theta;
  if (cos_4_theta < static_cast<geometric_t>(1e-16)) {
    return 0.0;
  }

  auto [sin_squared_phi, cos_squared_phi] = SinCosSquaredPhi(vector);
  geometric_t e = tan_squared_theta * (cos_squared_phi / (alpha_x_ * alpha_x_) +
                                       sin_squared_phi / (alpha_y_ * alpha_y_));
  geometric_t e_plus_1 = static_cast<geometric_t>(1.0) + e;

  return std::numbers::inv_pi_v<visual_t> /
         static_cast<visual_t>(alpha_x_ * alpha_y_ * cos_4_theta * e_plus_1 *
                               e_plus_1);
}

visual_t TrowbridgeReitzDistribution::Lambda(const Vector& vector) const {
  auto tan_squared_theta = TanSquaredTheta(vector);
  if (!std::isfinite(tan_squared_theta)) {
    return static_cast<visual_t>(0.0);
  }

  auto [sin_squared_phi, cos_squared_phi] = SinCosSquaredPhi(vector);
  geometric_t alpha_squared = cos_squared_phi * alpha_x_ * alpha_x_ +
                              sin_squared_phi * alpha_y_ * alpha_y_;

  return (std::sqrt(static_cast<visual_t>(1.0) +
                    alpha_squared * tan_squared_theta) -
          static_cast<visual_t>(1.0)) *
         static_cast<visual_t>(0.5);
}

// Adapted from Heitz 2017 (http://hal.archives-ouvertes.fr/hal-01509746)
Vector TrowbridgeReitzDistribution::Sample(const Vector& incoming,
                                           geometric_t u, geometric_t v) const {
  // Stretch incoming
  Vector stretched_incoming = Normalize(Vector(
      alpha_x_ * incoming.x, alpha_y_ * incoming.y, std::abs(incoming.z)));

  // Compute orthonormal basis vectors
  Vector axis0 =
      (CosTheta(stretched_incoming) < static_cast<geometric_t>(0.9999))
          ? Normalize(CrossProduct(
                stretched_incoming,
                Vector(static_cast<geometric>(0.0), static_cast<geometric>(0.0),
                       static_cast<geometric>(1.0))))
          : Vector(static_cast<geometric>(1.0), static_cast<geometric>(0.0),
                   static_cast<geometric>(0.0));
  Vector axis1 = CrossProduct(axis0, stretched_incoming);

  // Sample point with polar coordinates
  geometric_t a = static_cast<geometric_t>(1.0) /
                  (static_cast<geometric_t>(1.0) + stretched_incoming.z);
  geometric_t r = std::sqrt(u);
  geometric_t phi = (v < a)
                        ? v / a * static_cast<geometric_t>(std::numbers::pi)
                        : static_cast<geometric_t>(std::numbers::pi) +
                              (v - a) / (static_cast<geometric_t>(1.0) - a) *
                                  static_cast<geometric_t>(std::numbers::pi);
  geometric_t p0 = r * std::cos(phi);
  geometric_t p1 =
      r * std::sin(phi) *
      ((v < a) ? static_cast<geometric_t>(1.0) : stretched_incoming.z);

  // Compute stretched normal
  Vector stretched_normal =
      p0 * axis0 + p1 * axis1 +
      std::sqrt(std::max(static_cast<geometric_t>(0.0),
                         static_cast<geometric_t>(1.0) - p0 * p0 - p1 * p1)) *
          stretched_incoming;

  // Unstretch normal
  Vector normal =
      Vector(alpha_x_ * stretched_normal.x, alpha_y_ * stretched_normal.y,
             std::copysign(stretched_normal.z, incoming.z));

  return Normalize(normal);
}

}  // namespace internal
}  // namespace bxdfs
}  // namespace iris
