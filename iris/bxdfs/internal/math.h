#ifndef _IRIS_BXDFS_INTERNAL_MATH_
#define _IRIS_BXDFS_INTERNAL_MATH_

#include <algorithm>
#include <cassert>
#include <cmath>

#include "iris/float.h"

namespace iris {
namespace bxdfs {
namespace internal {

static inline visual_t FesnelDielectricReflectance(visual_t cos_theta_incident,
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

}  // namespace internal
}  // namespace bxdfs
}  // namespace iris

#endif  // _IRIS_BXDFS_INTERNAL_MATH_