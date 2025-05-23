#include "iris/bxdfs/microfacet_distributions/trowbridge_reitz_distribution.h"

#include <numbers>

#include "iris/bxdfs/internal/math.h"

namespace iris {
namespace bxdfs {
namespace microfacet_distributions {
namespace {

using ::iris::bxdfs::internal::CosSquaredTheta;
using ::iris::bxdfs::internal::CosTheta;
using ::iris::bxdfs::internal::SinCosPhi;
using ::iris::bxdfs::internal::SinCosSquaredPhi;
using ::iris::bxdfs::internal::TanSquaredTheta;
using ::iris::bxdfs::internal::TanTheta;

std::pair<geometric_t, geometric_t> TrowbridgeReitzSample11(
    const Vector& incoming, geometric_t u, geometric_t v) {
  if (CosTheta(incoming) > static_cast<geometric_t>(0.9999)) {
    geometric_t r = std::sqrt(u / (static_cast<geometric_t>(1.0) - u));
    geometric_t phi = static_cast<geometric_t>(2.0 * std::numbers::pi) * v;
    return std::make_pair(r * std::cos(phi), r * std::sin(phi));
  }

  geometric_t tan_theta = TanTheta(incoming);
  geometric_t a = static_cast<geometric_t>(1.0) / tan_theta;
  geometric_t G1 = static_cast<geometric_t>(2.0) /
                   (static_cast<geometric_t>(1.0) +
                    std::sqrt(static_cast<geometric_t>(1.0) +
                              static_cast<geometric_t>(1.0) / (a * a)));

  geometric_t A =
      static_cast<geometric_t>(2.0) * u / G1 - static_cast<geometric_t>(1.0);
  geometric_t tmp = std::min(
      static_cast<geometric_t>(1e10),
      static_cast<geometric_t>(1.0) / (A * A - static_cast<geometric_t>(1.0)));

  geometric_t B = tan_theta;
  geometric_t D =
      std::sqrt(std::max(geometric_t(B * B * tmp * tmp - (A * A - B * B) * tmp),
                         static_cast<geometric_t>(0.0)));
  geometric_t slope_x_2 = B * tmp + D;

  geometric_t slope_x;
  if (A < static_cast<geometric_t>(0.0) ||
      slope_x_2 > static_cast<geometric_t>(1.0) / tan_theta) {
    slope_x = B * tmp - D;
  } else {
    slope_x = slope_x_2;
  }

  geometric_t S;
  if (v > static_cast<geometric_t>(0.5)) {
    S = static_cast<geometric_t>(1.0);
    v = static_cast<geometric_t>(2.0) * (v - static_cast<geometric_t>(0.5));
  } else {
    S = static_cast<geometric_t>(-1.0);
    v = static_cast<geometric_t>(2.0) * (static_cast<geometric_t>(0.5) - v);
  }

  geometric_t z = (v * (v * (v * static_cast<geometric_t>(0.27385) -
                             static_cast<geometric_t>(0.73369)) +
                        static_cast<geometric_t>(0.46341))) /
                  (v * (v * (v * static_cast<geometric_t>(0.093073) +
                             static_cast<geometric_t>(0.309420)) -
                        static_cast<geometric_t>(1.0)) +
                   static_cast<geometric_t>(0.597999));

  geometric_t slope_y =
      S * z * std::sqrt(static_cast<geometric_t>(1.0) + slope_x * slope_x);

  return std::make_pair(slope_x, slope_y);
}

}  // namespace

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
  if (std::isinf(tan_squared_theta)) {
    return 0.0;
  }

  auto [sin_squared_phi, cos_squared_phi] = SinCosSquaredPhi(vector);
  geometric_t alpha_squared = cos_squared_phi * alpha_x_ * alpha_x_ +
                              sin_squared_phi * alpha_y_ * alpha_y_;

  return (std::sqrt(static_cast<visual_t>(1.0) +
                    alpha_squared * tan_squared_theta) -
          static_cast<visual_t>(1.0)) *
         static_cast<visual_t>(0.5);
}

Vector TrowbridgeReitzDistribution::Sample(const Vector& incoming,
                                           geometric_t u, geometric_t v) const {
  Vector stretched_incoming = Vector(
      alpha_x_ * incoming.x, alpha_y_ * incoming.y, std::abs(incoming.z));
  Vector normalized_stretched_incoming = Normalize(stretched_incoming);

  auto [slope_x, slope_y] =
      TrowbridgeReitzSample11(normalized_stretched_incoming, u, v);
  auto [sin_phi, cos_phi] = SinCosPhi(normalized_stretched_incoming);

  geometric_t tmp = cos_phi * slope_x - sin_phi * slope_y;
  slope_y = sin_phi * slope_x + cos_phi * slope_y;
  slope_x = tmp;

  slope_x *= alpha_x_;
  slope_y *= alpha_y_;

  return Normalize(
      Vector(-slope_x, -slope_y,
             std::copysign(static_cast<geometric>(1.0), incoming.z)));
}

visual_t TrowbridgeReitzDistribution::RoughnessToAlpha(visual_t roughness) {
  assert(std::isfinite(roughness));

  visual_t x = std::log(std::max(static_cast<visual_t>(0.001), roughness));
  visual_t x2 = x * x;
  visual_t x3 = x2 * x;
  visual_t x4 = x2 * x2;

  return static_cast<visual_t>(1.62142) + static_cast<visual_t>(0.819955) * x +
         static_cast<visual_t>(0.1734) * x2 +
         static_cast<visual_t>(0.0171201) * x3 +
         static_cast<visual_t>(0.000640711) * x4;
}

}  // namespace microfacet_distributions
}  // namespace bxdfs
}  // namespace iris
