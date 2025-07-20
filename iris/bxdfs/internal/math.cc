#include "iris/bxdfs/internal/math.h"

#include <algorithm>
#include <cassert>
#include <numbers>
#include <span>

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
      (eta_incident / eta_transmission) * sin_theta_incident;

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
    theta = static_cast<geometric_t>(std::numbers::pi / 4.0) * (v / u);
  } else {
    radius = v;
    theta = static_cast<geometric_t>(std::numbers::pi / 2.0) -
            static_cast<geometric_t>(std::numbers::pi / 4.0) * (u / v);
  }

  geometric_t x = radius * std::cos(theta);
  geometric_t y = radius * std::sin(theta);
  geometric_t z = std::sqrt(static_cast<geometric_t>(1.0) - radius * radius);

  return Vector(x, y, std::copysign(z, incoming_z));
}

std::optional<CatmullRomWeights> ComputeCatmullRomWeights(
    std::span<const geometric> control_point_locations,
    geometric sample_location) {
  if (control_point_locations.empty() ||
      sample_location < control_point_locations.front()) {
    return std::nullopt;
  }

  std::span<const geometric>::iterator iterator =
      std::lower_bound(control_point_locations.begin(),
                       control_point_locations.end(), sample_location);
  if (iterator == control_point_locations.end()) {
    return std::nullopt;
  }

  CatmullRomWeights result;
  if (*iterator == sample_location) {
    result.num_weights = 1;
    result.control_point_indices[0] =
        iterator - control_point_locations.begin();
    result.control_point_weights[0] = static_cast<geometric_t>(1.0);
    return result;
  }

  iterator -= 1;

  visual_t x0 = iterator[0];
  visual_t x1 = iterator[1];

  visual_t interval_width = x1 - x0;
  visual_t t = (sample_location - x0) / interval_width;
  visual_t t2 = t * t;
  visual_t t3 = t2 * t;

  visual_t weights[4];
  visual_t w0 = t3 - static_cast<visual_t>(2.0) * t2 + t;
  weights[1] = static_cast<visual_t>(2.0) * t3 -
               static_cast<visual_t>(3.0) * t2 + static_cast<visual_t>(1.0);
  weights[2] =
      static_cast<visual_t>(-2.0) * t3 + static_cast<visual_t>(3.0) * t2;
  visual_t w3 = t3 - t2;

  size_t start;
  if (iterator == control_point_locations.begin() || iterator[-1] == x0) {
    weights[1] -= w0;
    weights[2] += w0;
    start = 1;
  } else {
    w0 *= (x1 - x0) / (x1 - iterator[-1]);
    weights[0] = -w0;
    weights[2] += w0;
    start = 0;
  }

  size_t end;
  if (iterator + 2 == control_point_locations.end() || iterator[2] == x1) {
    weights[1] -= w3;
    weights[2] += w3;
    end = 3;
  } else {
    w3 *= (x1 - x0) / (iterator[2] - x0);
    weights[1] -= w3;
    weights[3] = w3;
    end = 4;
  }

  result.num_weights = 0;
  for (size_t i = start; i < end; i++) {
    result.control_point_indices[result.num_weights] =
        iterator - control_point_locations.begin() - 1 + i;
    result.control_point_weights[result.num_weights] = weights[i];
    result.num_weights += 1;
  }

  return result;
}

}  // namespace internal
}  // namespace bxdfs
}  // namespace iris
