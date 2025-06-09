#include "iris/matrix.h"

#include <cassert>
#include <cmath>
#include <cstring>
#include <optional>

namespace iris {
namespace {

std::optional<std::array<std::array<geometric, 4>, 4>> Invert(
    const std::array<std::array<geometric, 4>, 4>& m) {
  std::array<std::array<intermediate_t, 4>, 4> inverse;
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      inverse[i][j] = static_cast<intermediate_t>(m[i][j]);
    }
  }

  size_t column_index[] = {0, 0, 0, 0};
  size_t row_index[] = {0, 0, 0, 0};
  size_t pivot[] = {0, 0, 0, 0};

  for (size_t i = 0; i < 4; i++) {
    intermediate_t best_candidate = 0.0;
    size_t best_column = 0;
    size_t best_row = 0;

    for (size_t j = 0; j < 4; j++) {
      if (pivot[j] == 1) {
        continue;
      }

      for (size_t k = 0; k < 4; k++) {
        if (pivot[k] == 1) {
          continue;
        }

        if (pivot[k] > 1) {
          return std::nullopt;
        }

        intermediate_t new_candidate = std::abs(inverse[j][k]);
        if (new_candidate >= best_candidate) {
          best_candidate = new_candidate;
          best_row = j;
          best_column = k;
        }
      }
    }

    pivot[best_column] += 1;

    if (best_row != best_column) {
      for (size_t k = 0; k < 4; k++) {
        intermediate_t tmp = inverse[best_row][k];
        inverse[best_row][k] = inverse[best_column][k];
        inverse[best_column][k] = tmp;
      }
    }

    if (inverse[best_column][best_column] == (float_t)0.0) {
      return std::nullopt;
    }

    column_index[i] = best_column;
    row_index[i] = best_row;

    intermediate_t divisor = inverse[best_column][best_column];
    inverse[best_column][best_column] = (float_t)1.0;
    for (size_t j = 0; j < 4; j++) {
      inverse[best_column][j] /= divisor;
    }

    for (size_t j = 0; j < 4; j++) {
      if (j == best_column) {
        continue;
      }

      intermediate_t scalar = -inverse[j][best_column];
      inverse[j][best_column] = 0.0;
      for (size_t k = 0; k < 4; k++) {
        inverse[j][k] += scalar * inverse[best_column][k];
      }
    }
  }

  for (size_t jj = 0; jj < 4; jj++) {
    size_t j = 3 - jj;
    if (row_index[j] == column_index[j]) {
      continue;
    }

    for (size_t k = 0; k < 4; k++) {
      intermediate_t tmp = inverse[k][row_index[j]];
      inverse[k][row_index[j]] = inverse[k][column_index[j]];
      inverse[k][column_index[j]] = tmp;
    }
  }

  std::array<std::array<geometric, 4>, 4> result;
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      result[i][j] = static_cast<geometric>(inverse[i][j]);
    }
  }

  return result;
}

std::array<std::array<geometric, 4>, 4> Multiply4x4(
    const std::array<std::array<geometric, 4>, 4>& left,
    const std::array<std::array<geometric, 4>, 4>& right) {
  std::array<std::array<geometric, 4>, 4> result;
  for (size_t i = 0; i < 4; i++) {
    for (size_t j = 0; j < 4; j++) {
      intermediate_t intermediate =
          static_cast<intermediate_t>(left[i][0]) *
              static_cast<intermediate_t>(right[0][j]) +
          static_cast<intermediate_t>(left[i][1]) *
              static_cast<intermediate_t>(right[1][j]) +
          static_cast<intermediate_t>(left[i][2]) *
              static_cast<intermediate_t>(right[2][j]) +
          static_cast<intermediate_t>(left[i][3]) *
              static_cast<intermediate_t>(right[3][j]);
      result[i][j] = static_cast<geometric>(intermediate);
    }
  }

  return result;
}

}  // namespace

Matrix::Matrix(const std::array<std::array<geometric, 4>, 4>& m,
               const std::array<std::array<geometric, 4>, 4>& i)
    : m{m}, i{i} {
  assert(std::isfinite(m[0][0]));
  assert(std::isfinite(m[0][1]));
  assert(std::isfinite(m[0][2]));
  assert(std::isfinite(m[0][3]));
  assert(std::isfinite(m[1][0]));
  assert(std::isfinite(m[1][1]));
  assert(std::isfinite(m[1][2]));
  assert(std::isfinite(m[1][3]));
  assert(std::isfinite(m[2][0]));
  assert(std::isfinite(m[2][1]));
  assert(std::isfinite(m[2][2]));
  assert(std::isfinite(m[2][3]));
  assert(std::isfinite(m[3][0]));
  assert(std::isfinite(m[3][1]));
  assert(std::isfinite(m[3][2]));
  assert(std::isfinite(m[3][3]));
  assert(std::isfinite(i[0][0]));
  assert(std::isfinite(i[0][1]));
  assert(std::isfinite(i[0][2]));
  assert(std::isfinite(i[0][3]));
  assert(std::isfinite(i[1][0]));
  assert(std::isfinite(i[1][1]));
  assert(std::isfinite(i[1][2]));
  assert(std::isfinite(i[1][3]));
  assert(std::isfinite(i[2][0]));
  assert(std::isfinite(i[2][1]));
  assert(std::isfinite(i[2][2]));
  assert(std::isfinite(i[2][3]));
  assert(std::isfinite(i[3][0]));
  assert(std::isfinite(i[3][1]));
  assert(std::isfinite(i[3][2]));
  assert(std::isfinite(i[3][3]));

  assert(m[0][0] != 0.0 || m[0][1] != 0.0 || m[0][2] != 0.0 || m[0][3] != 0.0 ||
         m[1][0] != 0.0 || m[1][1] != 0.0 || m[1][2] != 0.0 || m[1][3] != 0.0 ||
         m[2][0] != 0.0 || m[2][1] != 0.0 || m[2][2] != 0.0 || m[2][3] != 0.0 ||
         m[3][0] != 0.0 || m[3][1] != 0.0 || m[3][2] != 0.0 || m[3][3] != 0.0 ||
         i[0][0] != 0.0 || i[0][1] != 0.0 || i[0][2] != 0.0 || i[0][3] != 0.0 ||
         i[1][0] != 0.0 || i[1][1] != 0.0 || i[1][2] != 0.0 || i[1][3] != 0.0 ||
         i[2][0] != 0.0 || i[2][1] != 0.0 || i[2][2] != 0.0 || i[2][3] != 0.0 ||
         i[3][0] != 0.0 || i[3][1] != 0.0 || i[3][2] != 0.0 || i[3][3] != 0.0);
}

std::expected<Matrix, const char*> Matrix::Create(
    const std::array<std::array<geometric, 4>, 4>& m) {
  static const char* errors[4][4] = {
      {"m[0][0] must be finite", "m[0][1] must be finite",
       "m[0][2] must be finite", "m[0][3] must be finite"},
      {"m[1][0] must be finite", "m[1][1] must be finite",
       "m[1][2] must be finite", "m[1][3] must be finite"},
      {"m[2][0] must be finite", "m[2][1] must be finite",
       "m[2][2] must be finite", "m[2][3] must be finite"},
      {"m[3][0] must be finite", "m[3][1] must be finite",
       "m[3][2] must be finite", "m[3][3] must be finite"}};

  for (size_t i = 0; i < m.size(); i++) {
    for (size_t j = 0; j < m[i].size(); j++) {
      if (!std::isfinite(m[i][j])) {
        return std::unexpected(errors[i][j]);
      }
    }
  }

  auto inverse = Invert(m);
  if (!inverse) {
    return std::unexpected("Matrix must be invertible");
  }

  return Matrix(m, inverse.value());
}

std::expected<Matrix, const char*> Matrix::Translation(geometric x, geometric y,
                                                       geometric z) {
  if (!std::isfinite(x)) {
    return std::unexpected("x must be finite");
  }

  if (!std::isfinite(y)) {
    return std::unexpected("y must be finite");
  }

  if (!std::isfinite(z)) {
    return std::unexpected("z must be finite");
  }

  std::array<std::array<geometric, 4>, 4> matrix = {{{1.0, 0.0, 0.0, x},
                                                     {0.0, 1.0, 0.0, y},
                                                     {0.0, 0.0, 1.0, z},
                                                     {0.0, 0.0, 0.0, 1.0}}};
  std::array<std::array<geometric, 4>, 4> inverse = {{{1.0, 0.0, 0.0, -x},
                                                      {0.0, 1.0, 0.0, -y},
                                                      {0.0, 0.0, 1.0, -z},
                                                      {0.0, 0.0, 0.0, 1.0}}};

  return Matrix(matrix, inverse);
}

std::expected<Matrix, const char*> Matrix::Scalar(geometric x, geometric y,
                                                  geometric z) {
  if (!std::isfinite(x)) {
    return std::unexpected("x must be finite");
  }

  if (x == 0.0) {
    return std::unexpected("x must be non-zero");
  }

  if (!std::isfinite(y)) {
    return std::unexpected("y must be finite");
  }

  if (y == 0.0) {
    return std::unexpected("y must be non-zero");
  }

  if (!std::isfinite(z)) {
    return std::unexpected("z must be finite");
  }

  if (z == 0.0) {
    return std::unexpected("z must be non-zero");
  }

  std::array<std::array<geometric, 4>, 4> matrix = {{{x, 0.0, 0.0, 0.0},
                                                     {0.0, y, 0.0, 0.0},
                                                     {0.0, 0.0, z, 0.0},
                                                     {0.0, 0.0, 0.0, 1.0}}};
  std::array<std::array<geometric, 4>, 4> inverse = {
      {{static_cast<geometric>(1.0) / x, 0.0, 0.0, 0.0},
       {0.0, static_cast<geometric>(1.0) / y, 0.0, 0.0},
       {0.0, 0.0, static_cast<geometric>(1.0) / z, 0.0},
       {0.0, 0.0, 0.0, 1.0}}};

  return Matrix(matrix, inverse);
}

std::expected<Matrix, const char*> Matrix::Rotation(geometric theta,
                                                    geometric x, geometric y,
                                                    geometric z) {
  if (!std::isfinite(theta)) {
    return std::unexpected("theta must be finite");
  }

  if (!std::isfinite(x)) {
    return std::unexpected("x must be finite");
  }

  if (!std::isfinite(y)) {
    return std::unexpected("y must be finite");
  }

  if (!std::isfinite(z)) {
    return std::unexpected("z must be finite");
  }

  if (x == 0.0 && y == 0.0 && z == 0.0) {
    return std::unexpected("One of x, y, or z must be non-zero");
  }

  intermediate_t theta_intermediate = static_cast<intermediate_t>(theta);
  intermediate_t x_intermediate = static_cast<intermediate_t>(x);
  intermediate_t y_intermediate = static_cast<intermediate_t>(y);
  intermediate_t z_intermediate = static_cast<intermediate_t>(z);

  intermediate_t magnitude = std::sqrt(x * x + y * y + z * z);
  x_intermediate /= magnitude;
  y_intermediate /= magnitude;
  z_intermediate /= magnitude;

  intermediate_t sin_theta = std::sin(theta_intermediate);
  intermediate_t cos_theta = std::cos(theta_intermediate);
  intermediate_t ic = static_cast<intermediate_t>(1.0) - cos_theta;

  geometric m00 =
      static_cast<geometric>(x_intermediate * x_intermediate * ic + cos_theta);
  geometric m01 = static_cast<geometric>(x_intermediate * y_intermediate * ic -
                                         z_intermediate * sin_theta);
  geometric m02 = static_cast<geometric>(x_intermediate * z_intermediate * ic +
                                         y_intermediate * sin_theta);

  geometric a0 = static_cast<geometric>(y_intermediate * x_intermediate * ic +
                                        z_intermediate * sin_theta);
  geometric a1 =
      static_cast<geometric>(y_intermediate * y_intermediate * ic + cos_theta);
  geometric a2 = static_cast<geometric>(y_intermediate * z_intermediate * ic -
                                        x_intermediate * sin_theta);

  geometric b0 = static_cast<geometric>(z_intermediate * x_intermediate * ic -
                                        y_intermediate * sin_theta);
  geometric b1 = static_cast<geometric>(z_intermediate * y_intermediate * ic +
                                        x_intermediate * sin_theta);
  geometric b2 =
      static_cast<geometric>(z_intermediate * z_intermediate * ic + cos_theta);

  std::array<std::array<geometric, 4>, 4> matrix = {{{m00, m01, m02, 0.0},
                                                     {a0, a1, a2, 0.0},
                                                     {b0, b1, b2, 0.0},
                                                     {0.0, 0.0, 0.0, 1.0}}};
  std::array<std::array<geometric, 4>, 4> inverse = {{{m00, a0, b0, 0.0},
                                                      {m01, a1, b1, 0.0},
                                                      {m02, a2, b2, 0.0},
                                                      {0.0, 0.0, 0.0, 1.0}}};

  return Matrix(matrix, inverse);
}

std::expected<Matrix, const char*> Matrix::LookAt(
    geometric eye_x, geometric eye_y, geometric eye_z, geometric look_at_x,
    geometric look_at_y, geometric look_at_z, geometric up_x, geometric up_y,
    geometric up_z) {
  if (!std::isfinite(eye_x)) {
    return std::unexpected("eye_x must be finite");
  }

  if (!std::isfinite(eye_y)) {
    return std::unexpected("eye_y must be finite");
  }

  if (!std::isfinite(eye_z)) {
    return std::unexpected("eye_z must be finite");
  }

  if (!std::isfinite(look_at_x)) {
    return std::unexpected("look_at_x must be finite");
  }

  if (!std::isfinite(look_at_y)) {
    return std::unexpected("look_at_y must be finite");
  }

  if (!std::isfinite(look_at_z)) {
    return std::unexpected("look_at_z must be finite");
  }

  if (!std::isfinite(up_x)) {
    return std::unexpected("up_x must be finite");
  }

  if (!std::isfinite(up_y)) {
    return std::unexpected("up_y must be finite");
  }

  if (!std::isfinite(up_z)) {
    return std::unexpected("up_z must be finite");
  }

  if (eye_x == look_at_x && eye_y == look_at_y && eye_z == look_at_z) {
    return std::unexpected("eye and look_at are the same point");
  }

  if (up_x == 0.0 && up_y == 0.0 && up_z == 0.0) {
    return std::unexpected("One of up_x, up_y, or up_z must be non-zero");
  }

  intermediate_t intermediate_eye_x = eye_x;
  intermediate_t intermediate_eye_y = eye_y;
  intermediate_t intermediate_eye_z = eye_z;

  intermediate_t intermediate_look_at_x = look_at_x;
  intermediate_t intermediate_look_at_y = look_at_y;
  intermediate_t intermediate_look_at_z = look_at_z;

  intermediate_t intermediate_up_x = up_x;
  intermediate_t intermediate_up_y = up_y;
  intermediate_t intermediate_up_z = up_z;

  intermediate_t look_direction_x = intermediate_look_at_x - intermediate_eye_x;
  intermediate_t look_direction_y = intermediate_look_at_y - intermediate_eye_y;
  intermediate_t look_direction_z = intermediate_look_at_z - intermediate_eye_z;

  intermediate_t look_direction_magnitude =
      std::sqrt(look_direction_x * look_direction_x +
                look_direction_y * look_direction_y +
                look_direction_z * look_direction_z);
  look_direction_x /= look_direction_magnitude;
  look_direction_y /= look_direction_magnitude;
  look_direction_z /= look_direction_magnitude;

  intermediate_t right_x = intermediate_up_y * look_direction_z -
                           intermediate_up_z * look_direction_y;
  intermediate_t right_y = intermediate_up_z * look_direction_x -
                           intermediate_up_x * look_direction_z;
  intermediate_t right_z = intermediate_up_x * look_direction_y -
                           intermediate_up_y * look_direction_x;
  if (right_x == 0.0 && right_y == 0.0 && right_z == 0.0) {
    return std::unexpected("up and look_at must be perpendicular");
  }

  intermediate_t right_magnitude =
      std::sqrt(right_x * right_x + right_y * right_y + right_z * right_z);
  right_x /= right_magnitude;
  right_y /= right_magnitude;
  right_z /= right_magnitude;

  intermediate_up_x = look_direction_y * right_z - look_direction_z * right_y;
  intermediate_up_y = look_direction_z * right_x - look_direction_x * right_z;
  intermediate_up_z = look_direction_x * right_y - look_direction_y * right_x;

  std::array<std::array<geometric, 4>, 4> matrix = {
      {{static_cast<geometric>(right_x),
        static_cast<geometric>(intermediate_up_x),
        static_cast<geometric>(look_direction_x), eye_x},
       {static_cast<geometric>(right_y),
        static_cast<geometric>(intermediate_up_y),
        static_cast<geometric>(look_direction_y), eye_y},
       {static_cast<geometric>(right_z),
        static_cast<geometric>(intermediate_up_z),
        static_cast<geometric>(look_direction_z), eye_z},
       {0.0, 0.0, 0.0, 1.0}}};

  return Matrix::Create(matrix).value();
}

std::expected<Matrix, const char*> Matrix::Orthographic(
    geometric left, geometric right, geometric bottom, geometric top,
    geometric near, geometric far) {
  if (!std::isfinite(left)) {
    return std::unexpected("left must be finite");
  }

  if (!std::isfinite(right)) {
    return std::unexpected("right must be finite");
  }

  if (!std::isfinite(bottom)) {
    return std::unexpected("bottom must be finite");
  }

  if (!std::isfinite(top)) {
    return std::unexpected("top must be finite");
  }

  if (!std::isfinite(near)) {
    return std::unexpected("near must be finite");
  }

  if (!std::isfinite(far)) {
    return std::unexpected("far must be finite");
  }

  if (left == right) {
    return std::unexpected("left must not equal right");
  }

  if (bottom == top) {
    return std::unexpected("bottom must not equal top");
  }

  if (near == far) {
    return std::unexpected("near must not equal far");
  }

  intermediate_t left_intermediate = left;
  intermediate_t right_intermediate = right;
  intermediate_t bottom_intermediate = bottom;
  intermediate_t top_intermediate = top;
  intermediate_t near_intermediate = near;
  intermediate_t far_intermediate = far;

  geometric tx =
      -static_cast<geometric>((right_intermediate + left_intermediate) /
                              (right_intermediate - left_intermediate));
  geometric ty =
      -static_cast<geometric>((top_intermediate + bottom_intermediate) /
                              (top_intermediate - bottom_intermediate));
  geometric tz =
      -static_cast<geometric>((far_intermediate + near_intermediate) /
                              (far_intermediate - near_intermediate));

  geometric sx =
      static_cast<geometric>(static_cast<intermediate_t>(2.0) /
                             (right_intermediate - left_intermediate));
  geometric sy =
      static_cast<geometric>(static_cast<intermediate_t>(2.0) /
                             (top_intermediate - bottom_intermediate));
  geometric sz = static_cast<geometric>(static_cast<intermediate_t>(-2.0) /
                                        (far_intermediate - near_intermediate));

  std::array<std::array<geometric, 4>, 4> matrix = {{{sx, 0.0, 0.0, tx},
                                                     {0.0, sy, 0.0, ty},
                                                     {0.0, 0.0, sz, tz},
                                                     {0.0, 0.0, 0.0, 1.0}}};
  std::array<std::array<geometric, 4>, 4> inverse = {
      {{static_cast<geometric>(1.0) / sx, 0.0, 0.0, -tx / sx},
       {0.0, static_cast<geometric>(1.0) / sy, 0.0, -ty / sy},
       {0.0, 0.0, static_cast<geometric>(1.0) / sz, -tz / sz},
       {0.0, 0.0, 0.0, 1.0}}};

  return Matrix(matrix, inverse);
}

const Matrix& Matrix::Identity() {
  static const std::array<std::array<geometric, 4>, 4> identity_values = {
      {{1.0, 0.0, 0.0, 0.0},
       {0.0, 1.0, 0.0, 0.0},
       {0.0, 0.0, 1.0, 0.0},
       {0.0, 0.0, 0.0, 1.0}}};
  static const Matrix identity_matrix(identity_values, identity_values);
  return identity_matrix;
}

Matrix Matrix::Multiply(const Matrix& left, const Matrix& right) {
  return left.Multiply(right);
}

Matrix Matrix::Multiply(const Matrix& matrix) const {
  return Matrix(Multiply4x4(m, matrix.m), Multiply4x4(matrix.i, i));
}

Matrix Matrix::Inverse() const { return Matrix(i, m); }

bool Matrix::SwapsHandedness() const {
  geometric_t determinant = m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]) -
                            m[0][1] * (m[1][0] * m[2][2] - m[1][2] * m[2][0]) +
                            m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);
  return determinant < static_cast<geometric_t>(0.0);
}

bool operator==(const Matrix& left, const Matrix& right) {
  return std::memcmp(&left.m, &right.m, sizeof(left.m)) == 0;
}

bool operator<(const Matrix& left, const Matrix& right) {
  return std::memcmp(&left.m, &right.m, sizeof(left.m)) < 0;
}

BoundingBox Matrix::Multiply(const BoundingBox& bounding_box) const {
  Point bottom0(bounding_box.lower.x, bounding_box.lower.y,
                bounding_box.lower.z);
  Point bottom1(bounding_box.upper.x, bounding_box.lower.y,
                bounding_box.lower.z);
  Point bottom2(bounding_box.lower.x, bounding_box.upper.y,
                bounding_box.lower.z);
  Point bottom3(bounding_box.upper.x, bounding_box.upper.y,
                bounding_box.lower.z);
  Point upper0(bounding_box.lower.x, bounding_box.lower.y,
               bounding_box.upper.z);
  Point upper1(bounding_box.upper.x, bounding_box.lower.y,
               bounding_box.upper.z);
  Point upper2(bounding_box.lower.x, bounding_box.upper.y,
               bounding_box.upper.z);
  Point upper3(bounding_box.upper.x, bounding_box.upper.y,
               bounding_box.upper.z);
  return BoundingBox(Multiply(bottom0), Multiply(bottom1), Multiply(bottom2),
                     Multiply(bottom3), Multiply(upper0), Multiply(upper1),
                     Multiply(upper2), Multiply(upper3));
}

}  // namespace iris
