#ifndef _IRIS_VECTOR_
#define _IRIS_VECTOR_

#include <cmath>
#include <type_traits>
#include <utility>

#include "iris/float.h"

namespace iris {

struct Vector final {
  enum Axis {
    X_AXIS = 0,
    Y_AXIS = 1,
    Z_AXIS = 2,
  };

#ifdef NDEBUG
  Vector(geometric x, geometric y, geometric z) noexcept : x(x), y(y), z(z) {}
#else
  Vector(geometric x, geometric y, geometric z) noexcept;
#endif  // NDEBUG

  Vector(const Vector&) noexcept = default;
  bool operator==(const Vector&) const = default;

  bool IsZero() const {
    return x == static_cast<geometric>(0.0) &&
           y == static_cast<geometric>(0.0) && z == static_cast<geometric>(0.0);
  }

  geometric_t Length() const;

#ifdef NDEBUG
  const geometric& operator[](size_t index) const {
    const geometric* as_array = &x;
    return as_array[index];
  }
#else
  const geometric& operator[](size_t index) const;
#endif  // NDEBUG

  Axis DominantAxis() const;

  Vector AlignAgainst(const Vector& vector) const;
  Vector AlignWith(const Vector& vector) const;

  const geometric x;
  const geometric y;
  const geometric z;
};

static inline Vector operator-(const Vector& vector) {
  return Vector(-vector.x, -vector.y, -vector.z);
}

static inline Vector operator-(const Vector& minuend,
                               const Vector& subtrahend) {
  return Vector(minuend.x - subtrahend.x, minuend.y - subtrahend.y,
                minuend.z - subtrahend.z);
}

static inline Vector operator+(const Vector& addend0, const Vector& addend1) {
  return Vector(addend0.x + addend1.x, addend0.y + addend1.y,
                addend0.z + addend1.z);
}

template <typename T>
static inline Vector operator*(const Vector& vector, T scalar)
  requires std::is_floating_point<T>::value
{
  return Vector(vector.x * scalar, vector.y * scalar, vector.z * scalar);
}

template <typename T>
static inline Vector operator*(T scalar, const Vector& vector)
  requires std::is_floating_point<T>::value
{
  return vector * scalar;
}

template <typename T>
static inline Vector operator/(const Vector& dividend, T divisor)
  requires std::is_floating_point<T>::value
{
  return Vector(dividend.x / divisor, dividend.y / divisor,
                dividend.z / divisor);
}

static inline Vector CrossProduct(const Vector& operand0,
                                  const Vector& operand1) {
  return Vector(operand0.y * operand1.z - operand0.z * operand1.y,
                operand0.z * operand1.x - operand0.x * operand1.z,
                operand0.x * operand1.y - operand0.y * operand1.x);
}

static inline geometric_t DotProduct(const Vector& operand0,
                                     const Vector& operand1) {
  return operand0.x * operand1.x + operand0.y * operand1.y +
         operand0.z * operand1.z;
}

static inline geometric_t ClampedDotProduct(const Vector& operand0,
                                            const Vector& operand1) {
  geometric_t dp = DotProduct(operand0, operand1);
  if (dp < static_cast<visual_t>(-1.0)) {
    return static_cast<visual_t>(-1.0);
  }

  return (dp < static_cast<visual_t>(1.0)) ? dp : static_cast<visual_t>(1.0);
}

static inline geometric_t ClampedAbsDotProduct(const Vector& operand0,
                                               const Vector& operand1) {
  geometric_t dp = std::abs(DotProduct(operand0, operand1));
  return (dp < static_cast<visual_t>(1.0)) ? dp : static_cast<visual_t>(1.0);
}

static inline Vector Normalize(const Vector& vector,
                               geometric_t* length_squared = nullptr,
                               geometric_t* length = nullptr) {
  geometric_t old_length_squared = DotProduct(vector, vector);
  if (length_squared) {
    *length_squared = old_length_squared;
  }

  geometric_t old_length = std::sqrt(old_length_squared);
  if (length) {
    *length = old_length;
  }

  return vector / old_length;
}

inline Vector::Axis Vector::DominantAxis() const {
  geometric max = std::abs(x);
  Vector::Axis max_axis = X_AXIS;

  geometric abs_y = std::abs(y);
  if (max < abs_y) {
    max = abs_y;
    max_axis = Y_AXIS;
  }

  geometric abs_z = std::abs(z);
  if (max < abs_z) {
    max_axis = Z_AXIS;
  }

  return max_axis;
}

inline Vector Vector::AlignAgainst(const Vector& vector) const {
  if (DotProduct(*this, vector) <= static_cast<geometric_t>(0.0)) {
    return *this;
  }

  return -*this;
}

inline Vector Vector::AlignWith(const Vector& vector) const {
  if (DotProduct(*this, vector) >= static_cast<geometric_t>(0.0)) {
    return *this;
  }

  return -*this;
}

std::pair<Vector, Vector> CoordinateSystem(const Vector& vector);

}  // namespace iris

#endif  // _IRIS_VECTOR_
