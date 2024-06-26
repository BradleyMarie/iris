#define _USE_MATH_DEFINES
#include "iris/bxdfs/internal/math.h"

#include <algorithm>
#include <cassert>
#include <cmath>

namespace iris {
namespace bxdfs {
namespace internal {
namespace {

const static geometric_t kMinValue = std::nextafter(
    static_cast<geometric_t>(-1.0), static_cast<geometric_t>(1.0));
const static geometric_t kMaxValue = std::nextafter(
    static_cast<geometric_t>(1.0), static_cast<geometric_t>(-1.0));

}  // namespace

visual_t FesnelDielectricReflectance(visual_t cos_theta_incident,
                                     visual_t eta_incident,
                                     visual_t eta_transmission) {
  assert(cos_theta_incident >= static_cast<visual_t>(-1.0));
  assert(cos_theta_incident <= static_cast<visual_t>(1.0));
  assert(eta_incident >= static_cast<visual_t>(1.0));
  assert(eta_transmission >= static_cast<visual_t>(1.0));

  cos_theta_incident = std::abs(cos_theta_incident);

  visual_t sin_theta_incident = std::sqrt(std::max(
      static_cast<visual_t>(0.0),
      static_cast<visual_t>(1.0) - cos_theta_incident * cos_theta_incident));
  visual_t sin_theta_transmission =
      eta_incident / eta_transmission * sin_theta_incident;

  if (sin_theta_transmission >= static_cast<visual_t>(1.0)) {
    return static_cast<visual_t>(1.0);
  }

  visual_t cos_theta_transmission =
      std::sqrt(std::max(static_cast<visual_t>(0.0),
                         static_cast<visual_t>(1.0) -
                             sin_theta_transmission * sin_theta_transmission));

  visual_t r_parallel = ((eta_transmission * cos_theta_incident) -
                         (eta_incident * cos_theta_transmission)) /
                        ((eta_transmission * cos_theta_incident) +
                         (eta_incident * cos_theta_transmission));
  visual_t r_perpendicular = ((eta_incident * cos_theta_incident) -
                              (eta_transmission * cos_theta_transmission)) /
                             ((eta_incident * cos_theta_incident) +
                              (eta_transmission * cos_theta_transmission));

  return (r_parallel * r_parallel + r_perpendicular * r_perpendicular) *
         static_cast<visual_t>(0.5);
}

Vector CosineSampleHemisphere(geometric incoming_z, Sampler& sampler) {
  geometric_t u = static_cast<geometric_t>(2.0) * sampler.Next() -
                  static_cast<geometric_t>(1.0);
  u = std::clamp(u, kMinValue, kMaxValue);

  geometric_t v = static_cast<geometric_t>(2.0) * sampler.Next() -
                  static_cast<geometric_t>(1.0);
  v = std::clamp(v, kMinValue, kMaxValue);

  if (u == static_cast<geometric_t>(0.0) &&
      v == static_cast<geometric_t>(0.0)) {
    return Vector(static_cast<geometric>(0.0), static_cast<geometric>(0.0),
                  std::copysign(static_cast<geometric>(1.0), incoming_z));
  }

  geometric_t theta, radius;
  if (std::abs(u) > std::abs(v)) {
    radius = u;
    theta = M_PI_4 * (v / u);
  } else {
    radius = v;
    theta = M_PI_2 - M_PI_4 * (u / v);
  }

  geometric_t x = radius * std::cos(theta);
  geometric_t y = radius * std::sin(theta);
  geometric_t z = std::sqrt(static_cast<geometric_t>(1.0) - radius * radius);

  return Vector(x, y, std::copysign(z, incoming_z));
}

}  // namespace internal
}  // namespace bxdfs
}  // namespace iris