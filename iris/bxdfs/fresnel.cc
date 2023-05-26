#include "iris/bxdfs/fresnel.h"

#include <algorithm>
#include <cmath>

#include "iris/reflectors/uniform_reflector.h"

namespace iris {
namespace bxdfs {
namespace {

visual_t FrDielectric(visual_t cos_theta_incident, visual_t eta_incident,
                      visual_t eta_transmission) {
  cos_theta_incident =
      std::max(static_cast<visual_t>(-1.0),
               std::min(cos_theta_incident, static_cast<visual_t>(1.0)));

  if (cos_theta_incident <= static_cast<visual_t>(0.0)) {
    std::swap(eta_incident, eta_transmission);
    cos_theta_incident = std::abs(cos_theta_incident);
  }

  visual_t sin_theta_incident = std::sqrt(std::max(
      static_cast<visual_t>(0.0),
      static_cast<visual_t>(1.0) - cos_theta_incident * cos_theta_incident));
  visual_t sin_theta_transmission =
      eta_incident / eta_transmission * sin_theta_incident;

  if (sin_theta_transmission >= static_cast<visual_t>(1.0)) {
    return static_cast<visual_t>(1.0);
  }

  visual_t cosTheta_transmission =
      std::sqrt(std::max(static_cast<visual_t>(0.0),
                         static_cast<visual_t>(1.0) -
                             sin_theta_transmission * sin_theta_transmission));

  visual_t r_parallel = ((eta_transmission * cos_theta_incident) -
                         (eta_incident * cosTheta_transmission)) /
                        ((eta_transmission * cos_theta_incident) +
                         (eta_incident * cosTheta_transmission));
  visual_t r_perpendicular = ((eta_incident * cos_theta_incident) -
                              (eta_transmission * cosTheta_transmission)) /
                             ((eta_incident * cos_theta_incident) +
                              (eta_transmission * cosTheta_transmission));

  return (r_parallel * r_parallel + r_perpendicular * r_perpendicular) *
         static_cast<visual_t>(0.5);
}

}  // namespace

const Reflector* FresnelDielectric::Evaluate(
    visual_t cos_theta_incidentncident, SpectralAllocator& allocator) const {
  static const reflectors::UniformReflector perfect_reflector(1.0);
  return allocator.Scale(
      &perfect_reflector,
      FrDielectric(cos_theta_incidentncident, eta_incident_, eta_transmitted_));
}

}  // namespace bxdfs
}  // namespace iris