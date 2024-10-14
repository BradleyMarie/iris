#ifndef _IRIS_BXDFS_INTERNAL_MATH_
#define _IRIS_BXDFS_INTERNAL_MATH_

#include <algorithm>
#include <cassert>
#include <optional>
#include <utility>

#include "iris/float.h"
#include "iris/sampler.h"
#include "iris/vector.h"

namespace iris {
namespace bxdfs {
namespace internal {

static inline geometric_t CosTheta(const Vector& vector) {
  return std::clamp(static_cast<geometric_t>(vector.z),
                    static_cast<geometric_t>(-1.0),
                    static_cast<geometric_t>(1.0));
}

static inline geometric_t AbsCosTheta(const Vector& vector) {
  return std::min(static_cast<geometric>(1.0), std::abs(vector.z));
}

static inline geometric_t CosSquaredTheta(const Vector& vector) {
  return std::min(static_cast<geometric_t>(1.0), vector.z * vector.z);
}

static inline geometric_t SinSquaredTheta(const Vector& vector) {
  return static_cast<geometric_t>(1.0) - CosSquaredTheta(vector);
}

static inline geometric_t SinTheta(const Vector& vector) {
  return std::sqrt(SinSquaredTheta(vector));
}

static inline geometric_t TanTheta(const Vector& vector) {
  return SinTheta(vector) / CosTheta(vector);
}

static inline geometric_t TanSquaredTheta(const Vector& vector) {
  return SinSquaredTheta(vector) / CosSquaredTheta(vector);
}

static inline std::pair<geometric_t, geometric_t> SinCosPhi(
    const Vector& vector) {
  geometric_t sin_theta = SinTheta(vector);
  if (sin_theta == 0.0) {
    return std::make_pair(static_cast<geometric_t>(0.0),
                          static_cast<geometric_t>(1.0));
  }

  geometric_t sin_phi =
      std::clamp(static_cast<geometric_t>(vector.y) / sin_theta,
                 static_cast<geometric_t>(-1.0), static_cast<geometric_t>(1.0));
  geometric_t cos_phi =
      std::clamp(static_cast<geometric_t>(vector.x) / sin_theta,
                 static_cast<geometric_t>(-1.0), static_cast<geometric_t>(1.0));

  return std::make_pair(sin_phi, cos_phi);
}

static inline std::pair<geometric_t, geometric_t> SinCosSquaredPhi(
    const Vector& vector) {
  auto result = SinCosPhi(vector);
  result.first *= result.first;
  result.second *= result.second;
  return result;
}

static inline std::optional<Vector> HalfAngle(const Vector& v0,
                                              const Vector& v1) {
  geometric_t half_angle_x = v0.x + v1.x;
  geometric_t half_angle_y = v0.y + v1.y;
  geometric_t half_angle_z = v0.z + v1.z;

  if (half_angle_x == static_cast<geometric>(0.0) &&
      half_angle_y == static_cast<geometric>(0.0) &&
      half_angle_z == static_cast<geometric>(0.0)) {
    return std::nullopt;
  }

  return Normalize(Vector(half_angle_x, half_angle_y, half_angle_z));
}

static inline std::optional<Vector> Reflect(const Vector& vector,
                                            const Vector& normal) {
  if (DotProduct(vector, normal) < static_cast<geometric_t>(0.0)) {
    return std::nullopt;
  }

  geometric_t scalar =
      static_cast<geometric_t>(2.0) * DotProduct(vector, normal);
  return scalar * normal - vector;
}

static inline std::optional<Vector> Refract(
    const Vector& vector, const Vector& normal,
    geometric_t relative_refractive_index) {
  geometric_t cos_theta_incident = ClampedDotProduct(vector, normal);
  geometric_t sin_theta_squared_incident =
      static_cast<geometric_t>(1.0) - cos_theta_incident * cos_theta_incident;
  geometric_t sin_theta_squared_transmitted = relative_refractive_index *
                                              relative_refractive_index *
                                              sin_theta_squared_incident;

  if (sin_theta_squared_transmitted >= static_cast<geometric_t>(1.0)) {
    return std::nullopt;
  }

  geometric_t cos_theta_transmitted =
      std::sqrt(static_cast<geometric_t>(1.0) - sin_theta_squared_transmitted);

  return (normal * (relative_refractive_index * cos_theta_incident -
                    cos_theta_transmitted)) -
         (vector * relative_refractive_index);
}

visual_t FesnelDielectricReflectance(visual_t cos_theta_incident,
                                     visual_t eta_incident,
                                     visual_t eta_transmission);

Vector CosineSampleHemisphere(geometric incoming_z, Sampler& sampler);

}  // namespace internal
}  // namespace bxdfs
}  // namespace iris

#endif  // _IRIS_BXDFS_INTERNAL_MATH_