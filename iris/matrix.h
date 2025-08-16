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
 public:
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

  bool SwapsHandedness() const { return swaps_handedness_; }

  const std::array<geometric, 4>& operator[](size_t index) const {
    return m[index];
  }

 private:
  Matrix(const std::array<std::array<geometric, 4>, 4>& m,
         const std::array<std::array<geometric, 4>, 4>& i);

 public:
  const std::array<std::array<geometric, 4>, 4> m;
  const std::array<std::array<geometric, 4>, 4> i;

 private:
  static const geometric_t rounding_error_[4];
  const bool swaps_handedness_;
};

bool operator==(const Matrix& left, const Matrix& right);
bool operator<(const Matrix& left, const Matrix& right);

inline Point Matrix::Multiply(const Point& point) const {
  geometric_t x =
      (m[0][0] * point.x + m[0][1] * point.y) + (m[0][2] * point.z + m[0][3]);
  geometric_t y =
      (m[1][0] * point.x + m[1][1] * point.y) + (m[1][2] * point.z + m[1][3]);
  geometric_t z =
      (m[2][0] * point.x + m[2][1] * point.y) + (m[2][2] * point.z + m[2][3]);

  if (geometric_t w = (m[3][0] * point.x + m[3][1] * point.y) +
                      (m[3][2] * point.z + m[3][3]);
      w != static_cast<geometric_t>(1.0)) {
    x /= w;
    y /= w;
    z /= w;
  }

  return Point(x, y, z);
}

inline Point Matrix::InverseMultiply(const Point& point) const {
  geometric_t x =
      (i[0][0] * point.x + i[0][1] * point.y) + (i[0][2] * point.z + i[0][3]);
  geometric_t y =
      (i[1][0] * point.x + i[1][1] * point.y) + (i[1][2] * point.z + i[1][3]);
  geometric_t z =
      (i[2][0] * point.x + i[2][1] * point.y) + (i[2][2] * point.z + i[2][3]);

  if (geometric_t w = (i[3][0] * point.x + i[3][1] * point.y) +
                      (i[3][2] * point.z + i[3][3]);
      w != static_cast<geometric_t>(1.0)) {
    x /= w;
    y /= w;
    z /= w;
  }

  return Point(x, y, z);
}

inline std::pair<Point, PositionError> Matrix::MultiplyWithError(
    const Point& point, const PositionError* existing_error) const {
  geometric_t x =
      (m[0][0] * point.x + m[0][1] * point.y) + (m[0][2] * point.z + m[0][3]);
  geometric_t y =
      (m[1][0] * point.x + m[1][1] * point.y) + (m[1][2] * point.z + m[1][3]);
  geometric_t z =
      (m[2][0] * point.x + m[2][1] * point.y) + (m[2][2] * point.z + m[2][3]);
  geometric_t w =
      (m[3][0] * point.x + m[3][1] * point.y) + (m[3][2] * point.z + m[3][3]);

  geometric_t err_x = std::abs(m[0][0] * point.x) +
                      std::abs(m[0][1] * point.y) +
                      std::abs(m[0][2] * point.z) + std::abs(m[0][3]);
  err_x *= rounding_error_[3];

  geometric_t err_y = std::abs(m[1][0] * point.x) +
                      std::abs(m[1][1] * point.y) +
                      std::abs(m[1][2] * point.z) + std::abs(m[1][3]);
  err_y *= rounding_error_[3];

  geometric_t err_z = std::abs(m[2][0] * point.x) +
                      std::abs(m[2][1] * point.y) +
                      std::abs(m[2][2] * point.z) + std::abs(m[2][3]);
  err_z *= rounding_error_[3];

  geometric_t err_w = 0.0;
  if (w != static_cast<geometric_t>(1.0)) {
    geometric_t new_x = x / w;
    geometric_t new_y = y / w;
    geometric_t new_z = z / w;

    err_w = std::abs(m[3][0] * point.x) + std::abs(m[3][1] * point.y) +
            std::abs(m[3][2] * point.z) + std::abs(m[3][3]);
    err_w *= rounding_error_[3];

    geometric_t safe_err_x = static_cast<iris::geometric>(0.0);
    if (x != static_cast<iris::geometric>(0.0)) {
      safe_err_x = std::abs(err_x / x);
    }

    geometric_t safe_err_y = static_cast<iris::geometric>(0.0);
    if (y != static_cast<iris::geometric>(0.0)) {
      safe_err_y = std::abs(err_y / y);
    }

    geometric_t safe_err_z = static_cast<iris::geometric>(0.0);
    if (z != static_cast<iris::geometric>(0.0)) {
      safe_err_z = std::abs(err_z / z);
    }

    err_x = std::abs(new_x) * (safe_err_x + std::abs(err_w / w)) +
            std::abs(new_x) * rounding_error_[1];
    err_y = std::abs(new_y) * (safe_err_y + std::abs(err_w / w)) +
            std::abs(new_y) * rounding_error_[1];
    err_z = std::abs(new_z) * (safe_err_z + std::abs(err_w / w)) +
            std::abs(new_z) * rounding_error_[1];

    x = new_x;
    y = new_y;
    z = new_z;
  }

  if (existing_error) {
    geometric_t relative_error =
        static_cast<geometric_t>(1.0) + rounding_error_[3];

    geometric_t existing_err_x = std::abs(m[0][0] * existing_error->x) +
                                 std::abs(m[0][1] * existing_error->y) +
                                 std::abs(m[0][2] * existing_error->z);
    existing_err_x *= relative_error;

    geometric_t existing_err_y = std::abs(m[1][0] * existing_error->x) +
                                 std::abs(m[1][1] * existing_error->y) +
                                 std::abs(m[1][2] * existing_error->z);
    existing_err_y *= relative_error;

    geometric_t existing_err_z = std::abs(m[2][0] * existing_error->x) +
                                 std::abs(m[2][1] * existing_error->y) +
                                 std::abs(m[2][2] * existing_error->z);
    existing_err_z *= relative_error;

    if (w != static_cast<geometric_t>(1.0)) {
      relative_error = (static_cast<geometric_t>(1.0) + err_w / w) *
                       (static_cast<geometric_t>(1.0) + rounding_error_[1]);

      existing_err_x = std::abs(existing_err_x / w);
      existing_err_x *= relative_error;

      existing_err_y = std::abs(existing_err_y / w);
      existing_err_y *= relative_error;

      existing_err_z = std::abs(existing_err_z / w);
      existing_err_z *= relative_error;
    }

    err_x += existing_err_x;
    err_y += existing_err_y;
    err_z += existing_err_z;
  }

  return std::make_pair(Point(x, y, z), PositionError(err_x, err_y, err_z));
}

inline std::pair<Point, PositionError> Matrix::InverseMultiplyWithError(
    const Point& point, const PositionError* existing_error) const {
  geometric_t x =
      (i[0][0] * point.x + i[0][1] * point.y) + (i[0][2] * point.z + i[0][3]);
  geometric_t y =
      (i[1][0] * point.x + i[1][1] * point.y) + (i[1][2] * point.z + i[1][3]);
  geometric_t z =
      (i[2][0] * point.x + i[2][1] * point.y) + (i[2][2] * point.z + i[2][3]);
  geometric_t w =
      (i[3][0] * point.x + i[3][1] * point.y) + (i[3][2] * point.z + i[3][3]);

  geometric_t err_x = std::abs(i[0][0] * point.x) +
                      std::abs(i[0][1] * point.y) +
                      std::abs(i[0][2] * point.z) + std::abs(i[0][3]);
  err_x *= rounding_error_[3];

  geometric_t err_y = std::abs(i[1][0] * point.x) +
                      std::abs(i[1][1] * point.y) +
                      std::abs(i[1][2] * point.z) + std::abs(i[1][3]);
  err_y *= rounding_error_[3];

  geometric_t err_z = std::abs(i[2][0] * point.x) +
                      std::abs(i[2][1] * point.y) +
                      std::abs(i[2][2] * point.z) + std::abs(i[2][3]);
  err_z *= rounding_error_[3];

  geometric_t err_w = 0.0;
  if (w != static_cast<geometric_t>(1.0)) {
    geometric_t new_x = x / w;
    geometric_t new_y = y / w;
    geometric_t new_z = z / w;

    err_w = std::abs(i[3][0] * point.x) + std::abs(i[3][1] * point.y) +
            std::abs(i[3][2] * point.z) + std::abs(i[3][3]);
    err_w *= rounding_error_[3];

    geometric_t safe_err_x = static_cast<iris::geometric>(0.0);
    if (x != static_cast<iris::geometric>(0.0)) {
      safe_err_x = std::abs(err_x / x);
    }

    geometric_t safe_err_y = static_cast<iris::geometric>(0.0);
    if (y != static_cast<iris::geometric>(0.0)) {
      safe_err_y = std::abs(err_y / y);
    }

    geometric_t safe_err_z = static_cast<iris::geometric>(0.0);
    if (z != static_cast<iris::geometric>(0.0)) {
      safe_err_z = std::abs(err_z / z);
    }

    err_x = std::abs(new_x) * (safe_err_x + std::abs(err_w / w)) +
            std::abs(new_x) * rounding_error_[1];
    err_y = std::abs(new_y) * (safe_err_y + std::abs(err_w / w)) +
            std::abs(new_y) * rounding_error_[1];
    err_z = std::abs(new_z) * (safe_err_z + std::abs(err_w / w)) +
            std::abs(new_z) * rounding_error_[1];

    x = new_x;
    y = new_y;
    z = new_z;
  }

  if (existing_error) {
    geometric_t relative_error =
        static_cast<geometric_t>(1.0) + rounding_error_[3];

    geometric_t existing_err_x = std::abs(i[0][0] * existing_error->x) +
                                 std::abs(i[0][1] * existing_error->y) +
                                 std::abs(i[0][2] * existing_error->z);
    existing_err_x *= relative_error;

    geometric_t existing_err_y = std::abs(i[1][0] * existing_error->x) +
                                 std::abs(i[1][1] * existing_error->y) +
                                 std::abs(i[1][2] * existing_error->z);
    existing_err_y *= relative_error;

    geometric_t existing_err_z = std::abs(i[2][0] * existing_error->x) +
                                 std::abs(i[2][1] * existing_error->y) +
                                 std::abs(i[2][2] * existing_error->z);
    existing_err_z *= relative_error;

    if (w != static_cast<geometric_t>(1.0)) {
      relative_error = (static_cast<geometric_t>(1.0) + err_w / w) *
                       (static_cast<geometric_t>(1.0) + rounding_error_[1]);

      existing_err_x = std::abs(existing_err_x / w);
      existing_err_x *= relative_error;

      existing_err_y = std::abs(existing_err_y / w);
      existing_err_y *= relative_error;

      existing_err_z = std::abs(existing_err_z / w);
      existing_err_z *= relative_error;
    }

    err_x += existing_err_x;
    err_y += existing_err_y;
    err_z += existing_err_z;
  }

  return std::make_pair(Point(x, y, z), PositionError(err_x, err_y, err_z));
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
