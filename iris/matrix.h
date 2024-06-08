#ifndef _IRIS_MATRIX_
#define _IRIS_MATRIX_

#include <array>
#include <expected>
#include <utility>

#include "iris/bounding_box.h"
#include "iris/float.h"
#include "iris/point.h"
#include "iris/position_error.h"
#include "iris/ray.h"
#include "iris/vector.h"

namespace iris {

struct Matrix final {
  Matrix(const Matrix& matrix) noexcept = default;

  static const Matrix& Identity();

  static std::expected<Matrix, const char*> Translation(geometric x,
                                                        geometric y,
                                                        geometric z);
  static std::expected<Matrix, const char*> Scalar(geometric x, geometric y,
                                                   geometric z);
  static std::expected<Matrix, const char*> Rotation(geometric theta,
                                                     geometric x, geometric y,
                                                     geometric z);
  static std::expected<Matrix, const char*> LookAt(
      geometric eye_x, geometric eye_y, geometric eye_z, geometric look_at_x,
      geometric look_at_y, geometric look_at_z, geometric up_x, geometric up_y,
      geometric up_z);
  static std::expected<Matrix, const char*> Orthographic(
      geometric left, geometric right, geometric bottom, geometric top,
      geometric near, geometric far);

  static std::expected<Matrix, const char*> Create(
      const std::array<std::array<geometric, 4>, 4>& m);

  BoundingBox Multiply(const BoundingBox& bounding_box) const;

  Point Multiply(const Point& point) const;
  Point InverseMultiply(const Point& point) const;

  std::pair<Point, PositionError> MultiplyWithError(
      const Point& point, const PositionError* existing_error = nullptr) const;
  std::pair<Point, PositionError> InverseMultiplyWithError(
      const Point& point, const PositionError* existing_error = nullptr) const;

  PositionError Multiply(const PositionError& existing_error) const;
  PositionError InverseMultiply(const PositionError& existing_error) const;

  Ray Multiply(const Ray& ray) const;
  Ray InverseMultiply(const Ray& ray) const;
  Ray MultiplyWithError(const Ray& ray) const;
  Ray InverseMultiplyWithError(const Ray& ray) const;

  Vector Multiply(const Vector& vector) const;
  Vector InverseMultiply(const Vector& vector) const;
  Vector InverseTransposeMultiply(const Vector& vector) const;
  Vector TransposeMultiply(const Vector& vector) const;

  static Matrix Multiply(const Matrix& left, const Matrix& right);
  Matrix Multiply(const Matrix& matrix) const;

  Matrix Inverse() const;

  const std::array<geometric, 4>& operator[](size_t index) const {
    return m[index];
  }

  bool operator==(const Matrix& matrix) const = default;

 private:
  Matrix(const std::array<std::array<geometric, 4>, 4>& m,
         const std::array<std::array<geometric, 4>, 4>& i);

 public:
  const std::array<std::array<geometric, 4>, 4> m;
  const std::array<std::array<geometric, 4>, 4> i;
};

bool operator<(const Matrix& left, const Matrix& right);

inline Point Matrix::Multiply(const Point& point) const {
  return Point(
      m[0][0] * point.x + m[0][1] * point.y + m[0][2] * point.z + m[0][3],
      m[1][0] * point.x + m[1][1] * point.y + m[1][2] * point.z + m[1][3],
      m[2][0] * point.x + m[2][1] * point.y + m[2][2] * point.z + m[2][3]);
}

inline Point Matrix::InverseMultiply(const Point& point) const {
  return Point(
      i[0][0] * point.x + i[0][1] * point.y + i[0][2] * point.z + i[0][3],
      i[1][0] * point.x + i[1][1] * point.y + i[1][2] * point.z + i[1][3],
      i[2][0] * point.x + i[2][1] * point.y + i[2][2] * point.z + i[2][3]);
}

inline std::pair<Point, PositionError> Matrix::MultiplyWithError(
    const Point& point, const PositionError* existing_error) const {
  Point transformed = Multiply(point);

  geometric_t x = (std::abs(m[0][0] * point.x) + std::abs(m[0][1] * point.y) +
                   std::abs(m[0][2] * point.z) + std::abs(m[0][3]));
  geometric_t y = (std::abs(m[1][0] * point.x) + std::abs(m[1][1] * point.y) +
                   std::abs(m[1][2] * point.z) + std::abs(m[1][3]));
  geometric_t z = (std::abs(m[2][0] * point.x) + std::abs(m[2][1] * point.y) +
                   std::abs(m[2][2] * point.z) + std::abs(m[2][3]));
  PositionError transform_error = PositionError(x, y, z) * RoundingError(3);

  if (existing_error == nullptr) {
    return {transformed, transform_error};
  }

  return {transformed, transform_error + Multiply(*existing_error)};
}

inline std::pair<Point, PositionError> Matrix::InverseMultiplyWithError(
    const Point& point, const PositionError* existing_error) const {
  Point transformed = InverseMultiply(point);

  geometric_t x = (std::abs(i[0][0] * point.x) + std::abs(i[0][1] * point.y) +
                   std::abs(i[0][2] * point.z) + std::abs(i[0][3]));
  geometric_t y = (std::abs(i[1][0] * point.x) + std::abs(i[1][1] * point.y) +
                   std::abs(i[1][2] * point.z) + std::abs(i[1][3]));
  geometric_t z = (std::abs(i[2][0] * point.x) + std::abs(i[2][1] * point.y) +
                   std::abs(i[2][2] * point.z) + std::abs(i[2][3]));
  PositionError transform_error = PositionError(x, y, z) * RoundingError(3);

  if (existing_error == nullptr) {
    return {transformed, transform_error};
  }

  return {transformed, transform_error + InverseMultiply(*existing_error)};
}

inline PositionError Matrix::Multiply(
    const PositionError& existing_error) const {
  geometric_t x = (std::abs(m[0][0] * existing_error.x) +
                   std::abs(m[0][1] * existing_error.y) +
                   std::abs(m[0][2] * existing_error.z));
  geometric_t y = (std::abs(m[1][0] * existing_error.x) +
                   std::abs(m[1][1] * existing_error.y) +
                   std::abs(m[1][2] * existing_error.z));
  geometric_t z = (std::abs(m[2][0] * existing_error.x) +
                   std::abs(m[2][1] * existing_error.y) +
                   std::abs(m[2][2] * existing_error.z));
  geometric_t relative_error = static_cast<geometric_t>(1.0) + RoundingError(3);
  return PositionError(x, y, z) * relative_error;
}

inline PositionError Matrix::InverseMultiply(
    const PositionError& existing_error) const {
  geometric_t x = (std::abs(i[0][0] * existing_error.x) +
                   std::abs(i[0][1] * existing_error.y) +
                   std::abs(i[0][2] * existing_error.z));
  geometric_t y = (std::abs(i[1][0] * existing_error.x) +
                   std::abs(i[1][1] * existing_error.y) +
                   std::abs(i[1][2] * existing_error.z));
  geometric_t z = (std::abs(i[2][0] * existing_error.x) +
                   std::abs(i[2][1] * existing_error.y) +
                   std::abs(i[2][2] * existing_error.z));
  geometric_t relative_error = static_cast<geometric_t>(1.0) + RoundingError(3);
  return PositionError(x, y, z) * relative_error;
}

inline Ray Matrix::Multiply(const Ray& ray) const {
  return Ray(Multiply(ray.origin), Multiply(ray.direction));
}

inline Ray Matrix::InverseMultiply(const Ray& ray) const {
  return Ray(InverseMultiply(ray.origin), InverseMultiply(ray.direction));
}

inline Ray Matrix::MultiplyWithError(const Ray& ray) const {
  auto [origin, error] = MultiplyWithError(ray.origin);
  Vector direction = Multiply(ray.direction);
  geometric_t absolute_error_squared = std::abs(direction.x * error.x) +
                                       std::abs(direction.y * error.y) +
                                       std::abs(direction.z * error.z);
  geometric_t length_squared = DotProduct(direction, direction);
  geometric_t relative_error = absolute_error_squared / length_squared;
  return Ray(origin + direction * relative_error, direction);
}

inline Ray Matrix::InverseMultiplyWithError(const Ray& ray) const {
  auto [origin, error] = InverseMultiplyWithError(ray.origin);
  Vector direction = InverseMultiply(ray.direction);
  geometric_t absolute_error_squared = std::abs(direction.x * error.x) +
                                       std::abs(direction.y * error.y) +
                                       std::abs(direction.z * error.z);
  geometric_t length_squared = DotProduct(direction, direction);
  geometric_t relative_error = absolute_error_squared / length_squared;
  return Ray(origin + direction * relative_error, direction);
}

inline Vector Matrix::Multiply(const Vector& vector) const {
  return Vector(m[0][0] * vector.x + m[0][1] * vector.y + m[0][2] * vector.z,
                m[1][0] * vector.x + m[1][1] * vector.y + m[1][2] * vector.z,
                m[2][0] * vector.x + m[2][1] * vector.y + m[2][2] * vector.z);
}

inline Vector Matrix::InverseMultiply(const Vector& vector) const {
  return Vector(i[0][0] * vector.x + i[0][1] * vector.y + i[0][2] * vector.z,
                i[1][0] * vector.x + i[1][1] * vector.y + i[1][2] * vector.z,
                i[2][0] * vector.x + i[2][1] * vector.y + i[2][2] * vector.z);
}

inline Vector Matrix::InverseTransposeMultiply(const Vector& vector) const {
  return Vector(i[0][0] * vector.x + i[1][0] * vector.y + i[2][0] * vector.z,
                i[0][1] * vector.x + i[1][1] * vector.y + i[2][1] * vector.z,
                i[0][2] * vector.x + i[1][2] * vector.y + i[2][2] * vector.z);
}

inline Vector Matrix::TransposeMultiply(const Vector& vector) const {
  return Vector(m[0][0] * vector.x + m[1][0] * vector.y + m[2][0] * vector.z,
                m[0][1] * vector.x + m[1][1] * vector.y + m[2][1] * vector.z,
                m[0][2] * vector.x + m[1][2] * vector.y + m[2][2] * vector.z);
}

}  // namespace iris

#endif  // _IRIS_MATRIX_