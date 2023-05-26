#ifndef _IRIS_BXDFS_MATH_
#define _IRIS_BXDFS_MATH_

#include <algorithm>
#include <cassert>
#include <utility>

#include "iris/float.h"
#include "iris/vector.h"

namespace iris {
namespace bxdfs {
namespace internal {

static inline geometric_t CosTheta(const Vector& vector) {
  return static_cast<geometric_t>(vector.z);
}

static inline geometric_t AbsCosTheta(const Vector& vector) {
  return std::abs(CosTheta(vector));
}

static inline geometric_t CosSquaredTheta(const Vector& vector) {
  return CosTheta(vector) * CosTheta(vector);
}

static inline geometric_t SinSquaredTheta(const Vector& vector) {
  return static_cast<geometric_t>(1.0) -
         std::min(static_cast<geometric_t>(1.0), CosSquaredTheta(vector));
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
    return std::make_pair(0.0, 1.0);
  }

  geometric_t sin_phi =
      std::max(static_cast<geometric_t>(-1.0),
               std::min(static_cast<geometric_t>(vector.y) / sin_theta,
                        static_cast<geometric_t>(1.0)));
  geometric_t cos_phi =
      std::max(static_cast<geometric_t>(-1.0),
               std::min(static_cast<geometric_t>(vector.x) / sin_theta,
                        static_cast<geometric_t>(1.0)));

  return std::make_pair(sin_phi, cos_phi);
}

static inline std::pair<geometric_t, geometric_t> SinCosSquaredPhi(
    const Vector& vector) {
  auto result = SinCosPhi(vector);
  result.first *= result.first;
  result.second *= result.second;
  return result;
}

static inline Vector Reflect(const Vector& vector, const Vector& normal) {
  return static_cast<geometric_t>(2.0) * DotProduct(vector, normal) * normal -
         vector;
}

}  // namespace internal
}  // namespace bxdfs
}  // namespace iris

#endif  // _IRIS_BXDFS_MATH_