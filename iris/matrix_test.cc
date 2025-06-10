#include "iris/matrix.h"

#include <limits>

#include "googletest/include/gtest/gtest.h"
#include "iris/bounding_box.h"
#include "iris/float.h"
#include "iris/point.h"
#include "iris/position_error.h"
#include "iris/ray.h"
#include "iris/vector.h"

namespace iris {
namespace {

TEST(MatrixTest, Identity) {
  const Matrix& matrix = Matrix::Identity();
  EXPECT_EQ(1.0, matrix.m[0][0]);
  EXPECT_EQ(0.0, matrix.m[0][1]);
  EXPECT_EQ(0.0, matrix.m[0][2]);
  EXPECT_EQ(0.0, matrix.m[0][3]);
  EXPECT_EQ(0.0, matrix.m[1][0]);
  EXPECT_EQ(1.0, matrix.m[1][1]);
  EXPECT_EQ(0.0, matrix.m[1][2]);
  EXPECT_EQ(0.0, matrix.m[1][3]);
  EXPECT_EQ(0.0, matrix.m[2][0]);
  EXPECT_EQ(0.0, matrix.m[2][1]);
  EXPECT_EQ(1.0, matrix.m[2][2]);
  EXPECT_EQ(0.0, matrix.m[2][3]);
  EXPECT_EQ(0.0, matrix.m[3][0]);
  EXPECT_EQ(0.0, matrix.m[3][1]);
  EXPECT_EQ(0.0, matrix.m[3][2]);
  EXPECT_EQ(1.0, matrix.m[3][3]);

  EXPECT_EQ(1.0, matrix.i[0][0]);
  EXPECT_EQ(0.0, matrix.i[0][1]);
  EXPECT_EQ(0.0, matrix.i[0][2]);
  EXPECT_EQ(0.0, matrix.i[0][3]);
  EXPECT_EQ(0.0, matrix.i[1][0]);
  EXPECT_EQ(1.0, matrix.i[1][1]);
  EXPECT_EQ(0.0, matrix.i[1][2]);
  EXPECT_EQ(0.0, matrix.i[1][3]);
  EXPECT_EQ(0.0, matrix.i[2][0]);
  EXPECT_EQ(0.0, matrix.i[2][1]);
  EXPECT_EQ(1.0, matrix.i[2][2]);
  EXPECT_EQ(0.0, matrix.i[2][3]);
  EXPECT_EQ(0.0, matrix.i[3][0]);
  EXPECT_EQ(0.0, matrix.i[3][1]);
  EXPECT_EQ(0.0, matrix.i[3][2]);
  EXPECT_EQ(1.0, matrix.i[3][3]);
}

TEST(MatrixTest, TranslationErrors) {
  EXPECT_STREQ(
      Matrix::Translation(std::numeric_limits<geometric>::infinity(), 1.0, 1.0)
          .error(),
      "x must be finite");
  EXPECT_STREQ(
      Matrix::Translation(1.0, std::numeric_limits<geometric>::infinity(), 1.0)
          .error(),
      "y must be finite");
  EXPECT_STREQ(
      Matrix::Translation(1.0, 1.0, std::numeric_limits<geometric>::infinity())
          .error(),
      "z must be finite");
}

TEST(MatrixTest, Translation) {
  Matrix matrix = Matrix::Translation(1.0, 2.0, 3.0).value();
  EXPECT_EQ(1.0, matrix.m[0][0]);
  EXPECT_EQ(0.0, matrix.m[0][1]);
  EXPECT_EQ(0.0, matrix.m[0][2]);
  EXPECT_EQ(1.0, matrix.m[0][3]);
  EXPECT_EQ(0.0, matrix.m[1][0]);
  EXPECT_EQ(1.0, matrix.m[1][1]);
  EXPECT_EQ(0.0, matrix.m[1][2]);
  EXPECT_EQ(2.0, matrix.m[1][3]);
  EXPECT_EQ(0.0, matrix.m[2][0]);
  EXPECT_EQ(0.0, matrix.m[2][1]);
  EXPECT_EQ(1.0, matrix.m[2][2]);
  EXPECT_EQ(3.0, matrix.m[2][3]);
  EXPECT_EQ(0.0, matrix.m[3][0]);
  EXPECT_EQ(0.0, matrix.m[3][1]);
  EXPECT_EQ(0.0, matrix.m[3][2]);
  EXPECT_EQ(1.0, matrix.m[3][3]);

  EXPECT_EQ(1.0, matrix.i[0][0]);
  EXPECT_EQ(0.0, matrix.i[0][1]);
  EXPECT_EQ(0.0, matrix.i[0][2]);
  EXPECT_EQ(-1.0, matrix.i[0][3]);
  EXPECT_EQ(0.0, matrix.i[1][0]);
  EXPECT_EQ(1.0, matrix.i[1][1]);
  EXPECT_EQ(0.0, matrix.i[1][2]);
  EXPECT_EQ(-2.0, matrix.i[1][3]);
  EXPECT_EQ(0.0, matrix.i[2][0]);
  EXPECT_EQ(0.0, matrix.i[2][1]);
  EXPECT_EQ(1.0, matrix.i[2][2]);
  EXPECT_EQ(-3.0, matrix.i[2][3]);
  EXPECT_EQ(0.0, matrix.i[3][0]);
  EXPECT_EQ(0.0, matrix.i[3][1]);
  EXPECT_EQ(0.0, matrix.i[3][2]);
  EXPECT_EQ(1.0, matrix.i[3][3]);
}

TEST(MatrixTest, ScalarErrors) {
  EXPECT_STREQ(
      Matrix::Scalar(std::numeric_limits<geometric>::infinity(), 1.0, 1.0)
          .error(),
      "x must be finite");
  EXPECT_STREQ(
      Matrix::Scalar(1.0, std::numeric_limits<geometric>::infinity(), 1.0)
          .error(),
      "y must be finite");
  EXPECT_STREQ(
      Matrix::Scalar(1.0, 1.0, std::numeric_limits<geometric>::infinity())
          .error(),
      "z must be finite");
  EXPECT_STREQ(Matrix::Scalar(0.0, 1.0, 1.0).error(), "x must be non-zero");
  EXPECT_STREQ(Matrix::Scalar(1.0, 0.0, 1.0).error(), "y must be non-zero");
  EXPECT_STREQ(Matrix::Scalar(1.0, 1.0, 0.0).error(), "z must be non-zero");
}

TEST(MatrixTest, Scalar) {
  Matrix matrix = Matrix::Scalar(2.0, 4.0, 8.0).value();
  EXPECT_EQ(2.0, matrix.m[0][0]);
  EXPECT_EQ(0.0, matrix.m[0][1]);
  EXPECT_EQ(0.0, matrix.m[0][2]);
  EXPECT_EQ(0.0, matrix.m[0][3]);
  EXPECT_EQ(0.0, matrix.m[1][0]);
  EXPECT_EQ(4.0, matrix.m[1][1]);
  EXPECT_EQ(0.0, matrix.m[1][2]);
  EXPECT_EQ(0.0, matrix.m[1][3]);
  EXPECT_EQ(0.0, matrix.m[2][0]);
  EXPECT_EQ(0.0, matrix.m[2][1]);
  EXPECT_EQ(8.0, matrix.m[2][2]);
  EXPECT_EQ(0.0, matrix.m[2][3]);
  EXPECT_EQ(0.0, matrix.m[3][0]);
  EXPECT_EQ(0.0, matrix.m[3][1]);
  EXPECT_EQ(0.0, matrix.m[3][2]);
  EXPECT_EQ(1.0, matrix.m[3][3]);

  EXPECT_EQ(0.5, matrix.i[0][0]);
  EXPECT_EQ(0.0, matrix.i[0][1]);
  EXPECT_EQ(0.0, matrix.i[0][2]);
  EXPECT_EQ(0.0, matrix.i[0][3]);
  EXPECT_EQ(0.0, matrix.i[1][0]);
  EXPECT_EQ(0.25, matrix.i[1][1]);
  EXPECT_EQ(0.0, matrix.i[1][2]);
  EXPECT_EQ(0.0, matrix.i[1][3]);
  EXPECT_EQ(0.0, matrix.i[2][0]);
  EXPECT_EQ(0.0, matrix.i[2][1]);
  EXPECT_EQ(0.125, matrix.i[2][2]);
  EXPECT_EQ(0.0, matrix.i[2][3]);
  EXPECT_EQ(0.0, matrix.i[3][0]);
  EXPECT_EQ(0.0, matrix.i[3][1]);
  EXPECT_EQ(0.0, matrix.i[3][2]);
  EXPECT_EQ(1.0, matrix.i[3][3]);
}

TEST(MatrixTest, RotationErrors) {
  EXPECT_STREQ(Matrix::Rotation(std::numeric_limits<geometric>::infinity(), 1.0,
                                1.0, 1.0)
                   .error(),
               "theta must be finite");
  EXPECT_STREQ(Matrix::Rotation(1.0, std::numeric_limits<geometric>::infinity(),
                                1.0, 1.0)
                   .error(),
               "x must be finite");
  EXPECT_STREQ(Matrix::Rotation(1.0, 1.0,
                                std::numeric_limits<geometric>::infinity(), 1.0)
                   .error(),
               "y must be finite");
  EXPECT_STREQ(Matrix::Rotation(1.0, 1.0, 1.0,
                                std::numeric_limits<geometric>::infinity())
                   .error(),
               "z must be finite");
  EXPECT_STREQ(Matrix::Rotation(1.0, 0.0, 0.0, 0.0).error(),
               "One of x, y, or z must be non-zero");
}

TEST(MatrixTest, Rotation) {
  Matrix matrix = Matrix::Rotation(4.0, 1.0, 2.0, 3.0).value();
  EXPECT_NEAR(static_cast<geometric>(-0.5355262), matrix.m[0][0],
              static_cast<geometric>(0.0001));
  EXPECT_NEAR(static_cast<geometric>(0.8430267), matrix.m[0][1],
              static_cast<geometric>(0.0001));
  EXPECT_NEAR(static_cast<geometric>(-0.0501757), matrix.m[0][2],
              static_cast<geometric>(0.0001));
  EXPECT_EQ(0.0, matrix.m[0][3]);
  EXPECT_NEAR(static_cast<geometric>(-0.3705571), matrix.m[1][0],
              static_cast<geometric>(0.0001));
  EXPECT_NEAR(static_cast<geometric>(-0.181174), matrix.m[1][1],
              static_cast<geometric>(0.0001));
  EXPECT_NEAR(static_cast<geometric>(0.9109684), matrix.m[1][2],
              static_cast<geometric>(0.0001));
  EXPECT_EQ(0.0, matrix.m[1][3]);
  EXPECT_NEAR(static_cast<geometric>(0.7588801), matrix.m[2][0],
              static_cast<geometric>(0.0001));
  EXPECT_NEAR(static_cast<geometric>(0.5064405), matrix.m[2][1],
              static_cast<geometric>(0.0001));
  EXPECT_NEAR(static_cast<geometric>(0.4094130), matrix.m[2][2],
              static_cast<geometric>(0.0001));
  EXPECT_EQ(0.0, matrix.m[2][3]);
  EXPECT_EQ(0.0, matrix.m[3][0]);
  EXPECT_EQ(0.0, matrix.m[3][1]);
  EXPECT_EQ(0.0, matrix.m[3][2]);
  EXPECT_EQ(1.0, matrix.m[3][3]);

  EXPECT_NEAR(static_cast<geometric>(-0.5355262), matrix.i[0][0],
              static_cast<geometric>(0.0001));
  EXPECT_NEAR(static_cast<geometric>(0.8430267), matrix.i[1][0],
              static_cast<geometric>(0.0001));
  EXPECT_NEAR(static_cast<geometric>(-0.0501757), matrix.i[2][0],
              static_cast<geometric>(0.0001));
  EXPECT_EQ(0.0, matrix.m[3][0]);
  EXPECT_NEAR(static_cast<geometric>(-0.3705571), matrix.i[0][1],
              static_cast<geometric>(0.0001));
  EXPECT_NEAR(static_cast<geometric>(-0.181174), matrix.i[1][1],
              static_cast<geometric>(0.0001));
  EXPECT_NEAR(static_cast<geometric>(0.9109684), matrix.i[2][1],
              static_cast<geometric>(0.0001));
  EXPECT_EQ(0.0, matrix.m[3][1]);
  EXPECT_NEAR(static_cast<geometric>(0.7588801), matrix.i[0][2],
              static_cast<geometric>(0.0001));
  EXPECT_NEAR(static_cast<geometric>(0.5064405), matrix.i[1][2],
              static_cast<geometric>(0.0001));
  EXPECT_NEAR(static_cast<geometric>(0.4094130), matrix.i[2][2],
              static_cast<geometric>(0.0001));
  EXPECT_EQ(0.0, matrix.m[3][2]);
  EXPECT_EQ(0.0, matrix.m[0][3]);
  EXPECT_EQ(0.0, matrix.m[1][3]);
  EXPECT_EQ(0.0, matrix.m[2][3]);
  EXPECT_EQ(1.0, matrix.m[3][3]);
}

TEST(MatrixTest, OrthographicErrors) {
  EXPECT_STREQ(Matrix::Orthographic(std::numeric_limits<geometric>::infinity(),
                                    2.0, 1.0, 2.0, 1.0, 2.0)
                   .error(),
               "left must be finite");
  EXPECT_STREQ(
      Matrix::Orthographic(1.0, std::numeric_limits<geometric>::infinity(), 1.0,
                           2.0, 1.0, 2.0)
          .error(),
      "right must be finite");
  EXPECT_STREQ(
      Matrix::Orthographic(1.0, 2.0, std::numeric_limits<geometric>::infinity(),
                           2.0, 1.0, 2.0)
          .error(),
      "bottom must be finite");
  EXPECT_STREQ(
      Matrix::Orthographic(1.0, 2.0, 1.0,
                           std::numeric_limits<geometric>::infinity(), 1.0, 2.0)
          .error(),
      "top must be finite");
  EXPECT_STREQ(
      Matrix::Orthographic(1.0, 2.0, 1.0, 2.0,
                           std::numeric_limits<geometric>::infinity(), 2.0)
          .error(),
      "near must be finite");
  EXPECT_STREQ(Matrix::Orthographic(1.0, 2.0, 1.0, 2.0, 1.0,
                                    std::numeric_limits<geometric>::infinity())
                   .error(),
               "far must be finite");
  EXPECT_STREQ(Matrix::Orthographic(1.0, 1.0, 1.0, 2.0, 1.0, 2.0).error(),
               "left must not equal right");
  EXPECT_STREQ(Matrix::Orthographic(1.0, 2.0, 1.0, 1.0, 1.0, 2.0).error(),
               "bottom must not equal top");
  EXPECT_STREQ(Matrix::Orthographic(1.0, 2.0, 1.0, 2.0, 1.0, 1.0).error(),
               "near must not equal far");
}

TEST(MatrixTest, Orthographic) {
  Matrix matrix = Matrix::Orthographic(1.0, 2.0, 2.0, 4.0, 4.0, 8.0).value();
  EXPECT_EQ(2.0, matrix.m[0][0]);
  EXPECT_EQ(0.0, matrix.m[0][1]);
  EXPECT_EQ(0.0, matrix.m[0][2]);
  EXPECT_EQ(-3.0, matrix.m[0][3]);
  EXPECT_EQ(0.0, matrix.m[1][0]);
  EXPECT_EQ(1.0, matrix.m[1][1]);
  EXPECT_EQ(0.0, matrix.m[1][2]);
  EXPECT_EQ(-3.0, matrix.m[1][3]);
  EXPECT_EQ(0.0, matrix.m[2][0]);
  EXPECT_EQ(0.0, matrix.m[2][1]);
  EXPECT_EQ(-0.5, matrix.m[2][2]);
  EXPECT_EQ(-3.0, matrix.m[2][3]);
  EXPECT_EQ(0.0, matrix.m[3][0]);
  EXPECT_EQ(0.0, matrix.m[3][1]);
  EXPECT_EQ(0.0, matrix.m[3][2]);
  EXPECT_EQ(1.0, matrix.m[3][3]);

  EXPECT_EQ(0.5, matrix.i[0][0]);
  EXPECT_EQ(0.0, matrix.i[0][1]);
  EXPECT_EQ(0.0, matrix.i[0][2]);
  EXPECT_EQ(1.5, matrix.i[0][3]);
  EXPECT_EQ(0.0, matrix.i[1][0]);
  EXPECT_EQ(1.0, matrix.i[1][1]);
  EXPECT_EQ(0.0, matrix.i[1][2]);
  EXPECT_EQ(3.0, matrix.i[1][3]);
  EXPECT_EQ(0.0, matrix.i[2][0]);
  EXPECT_EQ(0.0, matrix.i[2][1]);
  EXPECT_EQ(-2.0, matrix.i[2][2]);
  EXPECT_EQ(-6.0, matrix.i[2][3]);
  EXPECT_EQ(0.0, matrix.i[3][0]);
  EXPECT_EQ(0.0, matrix.i[3][1]);
  EXPECT_EQ(0.0, matrix.i[3][2]);
  EXPECT_EQ(1.0, matrix.i[3][3]);
}

TEST(MatrixTest, LookAt) {
  Matrix matrix =
      Matrix::LookAt(1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 1.0, 2.0, 3.0).value();
  EXPECT_NEAR(-0.408248276, matrix.m[0][0], 0.001);
  EXPECT_NEAR(-0.707106829, matrix.m[0][1], 0.001);
  EXPECT_NEAR(0.577350318, matrix.m[0][2], 0.001);
  EXPECT_NEAR(1.0, matrix.m[0][3], 0.001);
  EXPECT_NEAR(0.816496551, matrix.m[1][0], 0.001);
  EXPECT_NEAR(2.98023e-08, matrix.m[1][1], 0.001);
  EXPECT_NEAR(0.577350318, matrix.m[1][2], 0.001);
  EXPECT_NEAR(2.0, matrix.m[1][3], 0.001);
  EXPECT_NEAR(-0.408248335, matrix.m[2][0], 0.001);
  EXPECT_NEAR(0.707106829, matrix.m[2][1], 0.001);
  EXPECT_NEAR(0.577350318, matrix.m[2][2], 0.001);
  EXPECT_NEAR(3.0, matrix.m[2][3], 0.001);
  EXPECT_NEAR(0.0, matrix.m[3][0], 0.001);
  EXPECT_NEAR(0.0, matrix.m[3][1], 0.001);
  EXPECT_NEAR(0.0, matrix.m[3][2], 0.001);
  EXPECT_NEAR(1.0, matrix.m[3][3], 0.001);

  EXPECT_NEAR(-0.408248276, matrix.i[0][0], 0.001);
  EXPECT_NEAR(0.816496611, matrix.i[0][1], 0.001);
  EXPECT_NEAR(-0.408248305, matrix.i[0][2], 0.001);
  EXPECT_NEAR(3.441275e-08, matrix.i[0][3], 0.001);
  EXPECT_NEAR(-0.707106769, matrix.i[1][0], 0.001);
  EXPECT_NEAR(3.441275e-08, matrix.i[1][1], 0.001);
  EXPECT_NEAR(0.707106709, matrix.i[1][2], 0.001);
  EXPECT_NEAR(-1.41421342, matrix.i[1][3], 0.001);
  EXPECT_NEAR(0.577350199, matrix.i[2][0], 0.001);
  EXPECT_NEAR(0.577350259, matrix.i[2][1], 0.001);
  EXPECT_NEAR(0.577350199, matrix.i[2][2], 0.001);
  EXPECT_NEAR(-3.46410131, matrix.i[2][3], 0.001);
  EXPECT_NEAR(0.0, matrix.i[3][0], 0.001);
  EXPECT_NEAR(0.0, matrix.i[3][1], 0.001);
  EXPECT_NEAR(0.0, matrix.i[3][2], 0.001);
  EXPECT_NEAR(1.0, matrix.i[3][3], 0.001);
}

TEST(MatrixTest, CreateErrors) {
  for (size_t i = 0; i < 4; i++) {
    for (size_t j = 0; j < 4; j++) {
      std::array<std::array<geometric, 4>, 4> matrix = {{{1.0, 0.0, 0.0, 0.0},
                                                         {0.0, 1.0, 0.0, 0.0},
                                                         {0.0, 0.0, 1.0, 0.0},
                                                         {0.0, 0.0, 0.0, 1.0}}};
      matrix[i][j] = std::numeric_limits<geometric>::infinity();
      EXPECT_EQ(Matrix::Create(matrix).error(), "m[" + std::to_string(i) +
                                                    "][" + std::to_string(j) +
                                                    "] must be finite");
    }
  }

  std::array<std::array<geometric, 4>, 4> matrix = {{{0.0, 0.0, 0.0, 0.0},
                                                     {0.0, 0.0, 0.0, 0.0},
                                                     {0.0, 0.0, 0.0, 0.0},
                                                     {0.0, 0.0, 0.0, 0.0}}};
  EXPECT_STREQ(Matrix::Create(matrix).error(), "Matrix must be invertible");
}

TEST(MatrixTest, Create) {
  std::array<std::array<geometric, 4>, 4> ortho = {{{2.0, 0.0, 0.0, -3.0},
                                                    {0.0, 1.0, 0.0, -3.0},
                                                    {0.0, 0.0, -0.5, -3.0},
                                                    {0.0, 0.0, 0.0, 1.0}}};
  Matrix matrix = Matrix::Create(ortho).value();

  EXPECT_EQ(2.0, matrix.m[0][0]);
  EXPECT_EQ(0.0, matrix.m[0][1]);
  EXPECT_EQ(0.0, matrix.m[0][2]);
  EXPECT_EQ(-3.0, matrix.m[0][3]);
  EXPECT_EQ(0.0, matrix.m[1][0]);
  EXPECT_EQ(1.0, matrix.m[1][1]);
  EXPECT_EQ(0.0, matrix.m[1][2]);
  EXPECT_EQ(-3.0, matrix.m[1][3]);
  EXPECT_EQ(0.0, matrix.m[2][0]);
  EXPECT_EQ(0.0, matrix.m[2][1]);
  EXPECT_EQ(-0.5, matrix.m[2][2]);
  EXPECT_EQ(-3.0, matrix.m[2][3]);
  EXPECT_EQ(0.0, matrix.m[3][0]);
  EXPECT_EQ(0.0, matrix.m[3][1]);
  EXPECT_EQ(0.0, matrix.m[3][2]);
  EXPECT_EQ(1.0, matrix.m[3][3]);

  EXPECT_EQ(0.5, matrix.i[0][0]);
  EXPECT_EQ(0.0, matrix.i[0][1]);
  EXPECT_EQ(0.0, matrix.i[0][2]);
  EXPECT_EQ(1.5, matrix.i[0][3]);
  EXPECT_EQ(0.0, matrix.i[1][0]);
  EXPECT_EQ(1.0, matrix.i[1][1]);
  EXPECT_EQ(0.0, matrix.i[1][2]);
  EXPECT_EQ(3.0, matrix.i[1][3]);
  EXPECT_EQ(0.0, matrix.i[2][0]);
  EXPECT_EQ(0.0, matrix.i[2][1]);
  EXPECT_EQ(-2.0, matrix.i[2][2]);
  EXPECT_EQ(-6.0, matrix.i[2][3]);
  EXPECT_EQ(0.0, matrix.i[3][0]);
  EXPECT_EQ(0.0, matrix.i[3][1]);
  EXPECT_EQ(0.0, matrix.i[3][2]);
  EXPECT_EQ(1.0, matrix.i[3][3]);
}

TEST(MatrixTest, MultiplyMatrix) {
  Matrix matrix =
      Matrix::Multiply(Matrix::Translation(-3.0, -3.0, -3.0).value(),
                       Matrix::Scalar(2.0, 1.0, -0.5).value());

  EXPECT_EQ(2.0, matrix.m[0][0]);
  EXPECT_EQ(0.0, matrix.m[0][1]);
  EXPECT_EQ(0.0, matrix.m[0][2]);
  EXPECT_EQ(-3.0, matrix.m[0][3]);
  EXPECT_EQ(0.0, matrix.m[1][0]);
  EXPECT_EQ(1.0, matrix.m[1][1]);
  EXPECT_EQ(0.0, matrix.m[1][2]);
  EXPECT_EQ(-3.0, matrix.m[1][3]);
  EXPECT_EQ(0.0, matrix.m[2][0]);
  EXPECT_EQ(0.0, matrix.m[2][1]);
  EXPECT_EQ(-0.5, matrix.m[2][2]);
  EXPECT_EQ(-3.0, matrix.m[2][3]);
  EXPECT_EQ(0.0, matrix.m[3][0]);
  EXPECT_EQ(0.0, matrix.m[3][1]);
  EXPECT_EQ(0.0, matrix.m[3][2]);
  EXPECT_EQ(1.0, matrix.m[3][3]);

  EXPECT_EQ(0.5, matrix.i[0][0]);
  EXPECT_EQ(0.0, matrix.i[0][1]);
  EXPECT_EQ(0.0, matrix.i[0][2]);
  EXPECT_EQ(1.5, matrix.i[0][3]);
  EXPECT_EQ(0.0, matrix.i[1][0]);
  EXPECT_EQ(1.0, matrix.i[1][1]);
  EXPECT_EQ(0.0, matrix.i[1][2]);
  EXPECT_EQ(3.0, matrix.i[1][3]);
  EXPECT_EQ(0.0, matrix.i[2][0]);
  EXPECT_EQ(0.0, matrix.i[2][1]);
  EXPECT_EQ(-2.0, matrix.i[2][2]);
  EXPECT_EQ(-6.0, matrix.i[2][3]);
  EXPECT_EQ(0.0, matrix.i[3][0]);
  EXPECT_EQ(0.0, matrix.i[3][1]);
  EXPECT_EQ(0.0, matrix.i[3][2]);
  EXPECT_EQ(1.0, matrix.i[3][3]);
}

TEST(MatrixTest, Inverse) {
  Matrix matrix = Matrix::Translation(1.0, 2.0, 3.0).value().Inverse();
  EXPECT_EQ(1.0, matrix.m[0][0]);
  EXPECT_EQ(0.0, matrix.m[0][1]);
  EXPECT_EQ(0.0, matrix.m[0][2]);
  EXPECT_EQ(-1.0, matrix.m[0][3]);
  EXPECT_EQ(0.0, matrix.m[1][0]);
  EXPECT_EQ(1.0, matrix.m[1][1]);
  EXPECT_EQ(0.0, matrix.m[1][2]);
  EXPECT_EQ(-2.0, matrix.m[1][3]);
  EXPECT_EQ(0.0, matrix.m[2][0]);
  EXPECT_EQ(0.0, matrix.m[2][1]);
  EXPECT_EQ(1.0, matrix.m[2][2]);
  EXPECT_EQ(-3.0, matrix.m[2][3]);
  EXPECT_EQ(0.0, matrix.m[3][0]);
  EXPECT_EQ(0.0, matrix.m[3][1]);
  EXPECT_EQ(0.0, matrix.m[3][2]);
  EXPECT_EQ(1.0, matrix.m[3][3]);

  EXPECT_EQ(1.0, matrix.i[0][0]);
  EXPECT_EQ(0.0, matrix.i[0][1]);
  EXPECT_EQ(0.0, matrix.i[0][2]);
  EXPECT_EQ(1.0, matrix.i[0][3]);
  EXPECT_EQ(0.0, matrix.i[1][0]);
  EXPECT_EQ(1.0, matrix.i[1][1]);
  EXPECT_EQ(0.0, matrix.i[1][2]);
  EXPECT_EQ(2.0, matrix.i[1][3]);
  EXPECT_EQ(0.0, matrix.i[2][0]);
  EXPECT_EQ(0.0, matrix.i[2][1]);
  EXPECT_EQ(1.0, matrix.i[2][2]);
  EXPECT_EQ(3.0, matrix.i[2][3]);
  EXPECT_EQ(0.0, matrix.i[3][0]);
  EXPECT_EQ(0.0, matrix.i[3][1]);
  EXPECT_EQ(0.0, matrix.i[3][2]);
  EXPECT_EQ(1.0, matrix.i[3][3]);
}

TEST(MatrixTest, SwapsHandedness) {
  EXPECT_FALSE(Matrix::Scalar(1.0, 1.0, 1.0)->SwapsHandedness());
  EXPECT_TRUE(Matrix::Scalar(1.0, 1.0, -1.0)->SwapsHandedness());
  EXPECT_TRUE(Matrix::Scalar(1.0, -1.0, 1.0)->SwapsHandedness());
  EXPECT_FALSE(Matrix::Scalar(1.0, -1.0, -1.0)->SwapsHandedness());
  EXPECT_TRUE(Matrix::Scalar(-1.0, 1.0, 1.0)->SwapsHandedness());
  EXPECT_FALSE(Matrix::Scalar(-1.0, 1.0, -1.0)->SwapsHandedness());
  EXPECT_FALSE(Matrix::Scalar(-1.0, -1.0, 1.0)->SwapsHandedness());
  EXPECT_TRUE(Matrix::Scalar(-1.0, -1.0, -1.0)->SwapsHandedness());
}

TEST(MatrixTest, Subscript) {
  std::array<std::array<geometric, 4>, 4> ortho = {{{2.0, 0.0, 0.0, -3.0},
                                                    {0.0, 1.0, 0.0, -3.0},
                                                    {0.0, 0.0, -0.5, -3.0},
                                                    {0.0, 0.0, 0.0, 1.0}}};
  Matrix matrix = Matrix::Create(ortho).value();

  EXPECT_EQ(matrix.m[0][0], matrix[0][0]);
  EXPECT_EQ(matrix.m[0][1], matrix[0][1]);
  EXPECT_EQ(matrix.m[0][2], matrix[0][2]);
  EXPECT_EQ(matrix.m[0][3], matrix[0][3]);
  EXPECT_EQ(matrix.m[1][0], matrix[1][0]);
  EXPECT_EQ(matrix.m[1][1], matrix[1][1]);
  EXPECT_EQ(matrix.m[1][2], matrix[1][2]);
  EXPECT_EQ(matrix.m[1][3], matrix[1][3]);
  EXPECT_EQ(matrix.m[2][0], matrix[2][0]);
  EXPECT_EQ(matrix.m[2][1], matrix[2][1]);
  EXPECT_EQ(matrix.m[2][2], matrix[2][2]);
  EXPECT_EQ(matrix.m[2][3], matrix[2][3]);
  EXPECT_EQ(matrix.m[3][0], matrix[3][0]);
  EXPECT_EQ(matrix.m[3][1], matrix[3][1]);
  EXPECT_EQ(matrix.m[3][2], matrix[3][2]);
  EXPECT_EQ(matrix.m[3][3], matrix[3][3]);
}

TEST(MatrixTest, MultiplyBoundingBox) {
  Matrix matrix = Matrix::Translation(1.0, 2.0, 3.0).value();
  BoundingBox bounds(Point(0.0, 0.0, 0.0), Point(1.0, 1.0, 1.0));
  BoundingBox transformed = matrix.Multiply(bounds);
  EXPECT_EQ(Point(1.0, 2.0, 3.0), transformed.lower);
  EXPECT_EQ(Point(2.0, 3.0, 4.0), transformed.upper);
}

TEST(MatrixTest, MultiplyPoint) {
  Matrix matrix = Matrix::Translation(1.0, 2.0, 3.0).value();
  EXPECT_EQ(Point(1.0, 2.0, 3.0), matrix.Multiply(Point(0.0, 0.0, 0.0)));
}

TEST(MatrixTest, MultiplyPointWithW) {
  std::array<std::array<geometric, 4>, 4> elements = {{{1.0, 0.0, 0.0, 0.0},
                                                       {0.0, 1.0, 0.0, 0.0},
                                                       {0.0, 0.0, 1.0, 0.0},
                                                       {0.0, 0.0, 0.0, 0.5}}};
  Matrix matrix = Matrix::Create(elements).value();
  EXPECT_EQ(Point(2.0, 2.0, 2.0), matrix.Multiply(Point(1.0, 1.0, 1.0)));
}

TEST(MatrixTest, InverseMultiplyPoint) {
  Matrix matrix = Matrix::Translation(1.0, 2.0, 3.0).value();
  EXPECT_EQ(Point(-1.0, -2.0, -3.0),
            matrix.InverseMultiply(Point(0.0, 0.0, 0.0)));
}

TEST(MatrixTest, InverseMultiplyPointWithW) {
  std::array<std::array<geometric, 4>, 4> elements = {{{1.0, 0.0, 0.0, 0.0},
                                                       {0.0, 1.0, 0.0, 0.0},
                                                       {0.0, 0.0, 1.0, 0.0},
                                                       {0.0, 0.0, 0.0, 2.0}}};
  Matrix matrix = Matrix::Create(elements).value();
  EXPECT_EQ(Point(2.0, 2.0, 2.0), matrix.InverseMultiply(Point(1.0, 1.0, 1.0)));
}

TEST(MatrixTest, MultiplyPointWithError) {
  Matrix matrix = Matrix::Translation(1.0, 2.0, 3.0).value();
  Point as_point = matrix.Multiply(Point(1.0, 2.0, 3.0));
  auto [point, error] = matrix.MultiplyWithError(Point(1.0, 2.0, 3.0));
  EXPECT_EQ(as_point.x, point.x);
  EXPECT_EQ(as_point.x * RoundingError(3), error.x);
  EXPECT_EQ(as_point.y, point.y);
  EXPECT_EQ(as_point.y * RoundingError(3), error.y);
  EXPECT_EQ(as_point.z, point.z);
  EXPECT_EQ(as_point.z * RoundingError(3), error.z);
}

TEST(MatrixTest, MultiplyPointWithErrorWithW) {
  std::array<std::array<geometric, 4>, 4> elements = {{{1.0, 0.0, 0.0, 0.0},
                                                       {0.0, 1.0, 0.0, 0.0},
                                                       {0.0, 0.0, 1.0, 0.0},
                                                       {0.0, 0.0, 0.0, 0.5}}};
  Matrix matrix = Matrix::Create(elements).value();

  auto [point0, error0] = matrix.MultiplyWithError(Point(1.0, 0.0, 0.0));
  EXPECT_EQ(2.0, point0.x);
  EXPECT_EQ(point0.x * (static_cast<geometric_t>(1.0) * RoundingError(3) /
                            static_cast<geometric_t>(1.0) +
                        static_cast<geometric_t>(0.5) * RoundingError(3) /
                            static_cast<geometric_t>(0.5)) +
                (point0.x * RoundingError(1)),
            error0.x);
  EXPECT_EQ(0.0, point0.y);
  EXPECT_EQ(0.0, error0.z);
  EXPECT_EQ(0.0, point0.z);
  EXPECT_EQ(0.0, error0.z);

  auto [point1, error1] = matrix.MultiplyWithError(Point(0.0, 1.0, 0.0));
  EXPECT_EQ(0.0, point1.x);
  EXPECT_EQ(0.0, error1.x);
  EXPECT_EQ(2.0, point1.y);
  EXPECT_EQ(point1.y * (static_cast<geometric_t>(1.0) * RoundingError(3) /
                            static_cast<geometric_t>(1.0) +
                        static_cast<geometric_t>(0.5) * RoundingError(3) /
                            static_cast<geometric_t>(0.5)) +
                (point1.y * RoundingError(1)),
            error1.y);
  EXPECT_EQ(0.0, point1.z);
  EXPECT_EQ(0.0, error1.z);

  auto [point2, error2] = matrix.MultiplyWithError(Point(0.0, 0.0, 1.0));
  EXPECT_EQ(0.0, point2.x);
  EXPECT_EQ(0.0, error2.x);
  EXPECT_EQ(0.0, point2.y);
  EXPECT_EQ(0.0, error2.y);
  EXPECT_EQ(2.0, point2.z);
  EXPECT_EQ(point2.z * (static_cast<geometric_t>(1.0) * RoundingError(3) /
                            static_cast<geometric_t>(1.0) +
                        static_cast<geometric_t>(0.5) * RoundingError(3) /
                            static_cast<geometric_t>(0.5)) +
                (point2.z * RoundingError(1)),
            error2.z);
}

TEST(MatrixTest, InverseMultiplyPointWithError) {
  Matrix matrix = Matrix::Translation(-1.0, -2.0, -3.0).value();
  Point as_point = matrix.InverseMultiply(Point(1.0, 2.0, 3.0));
  auto [point, error] = matrix.InverseMultiplyWithError(Point(1.0, 2.0, 3.0));
  EXPECT_EQ(as_point.x, point.x);
  EXPECT_EQ(as_point.x * RoundingError(3), error.x);
  EXPECT_EQ(as_point.y, point.y);
  EXPECT_EQ(as_point.y * RoundingError(3), error.y);
  EXPECT_EQ(as_point.z, point.z);
  EXPECT_EQ(as_point.z * RoundingError(3), error.z);
}

TEST(MatrixTest, InverseMultiplyPointWithErrorWithW) {
  std::array<std::array<geometric, 4>, 4> elements = {{{1.0, 0.0, 0.0, 0.0},
                                                       {0.0, 1.0, 0.0, 0.0},
                                                       {0.0, 0.0, 1.0, 0.0},
                                                       {0.0, 0.0, 0.0, 2.0}}};
  Matrix matrix = Matrix::Create(elements).value();

  auto [point0, error0] = matrix.InverseMultiplyWithError(Point(1.0, 0.0, 0.0));
  EXPECT_EQ(2.0, point0.x);
  EXPECT_EQ(point0.x * (static_cast<geometric_t>(1.0) * RoundingError(3) /
                            static_cast<geometric_t>(1.0) +
                        static_cast<geometric_t>(0.5) * RoundingError(3) /
                            static_cast<geometric_t>(0.5)) +
                (point0.x * RoundingError(1)),
            error0.x);
  EXPECT_EQ(0.0, point0.y);
  EXPECT_EQ(0.0, error0.z);
  EXPECT_EQ(0.0, point0.z);
  EXPECT_EQ(0.0, error0.z);

  auto [point1, error1] = matrix.InverseMultiplyWithError(Point(0.0, 1.0, 0.0));
  EXPECT_EQ(0.0, point1.x);
  EXPECT_EQ(0.0, error1.x);
  EXPECT_EQ(2.0, point1.y);
  EXPECT_EQ(point1.y * (static_cast<geometric_t>(1.0) * RoundingError(3) /
                            static_cast<geometric_t>(1.0) +
                        static_cast<geometric_t>(0.5) * RoundingError(3) /
                            static_cast<geometric_t>(0.5)) +
                (point1.y * RoundingError(1)),
            error1.y);
  EXPECT_EQ(0.0, point1.z);
  EXPECT_EQ(0.0, error1.z);

  auto [point2, error2] = matrix.InverseMultiplyWithError(Point(0.0, 0.0, 1.0));
  EXPECT_EQ(0.0, point2.x);
  EXPECT_EQ(0.0, error2.x);
  EXPECT_EQ(0.0, point2.y);
  EXPECT_EQ(0.0, error2.y);
  EXPECT_EQ(2.0, point2.z);
  EXPECT_EQ(point2.z * (static_cast<geometric_t>(1.0) * RoundingError(3) /
                            static_cast<geometric_t>(1.0) +
                        static_cast<geometric_t>(0.5) * RoundingError(3) /
                            static_cast<geometric_t>(0.5)) +
                (point2.z * RoundingError(1)),
            error2.z);
}

TEST(MatrixTest, MultiplyPointWithExistingError) {
  Point point(0.0, 0.0, 0.0);

  PositionError existing_error0(-1.0, 0.0, 0.0);
  auto [point0, error0] = Matrix::Translation(1.0, 0.0, 0.0)
                              .value()
                              .MultiplyWithError(point, &existing_error0);
  EXPECT_EQ(1.0, point0.x);
  EXPECT_EQ((static_cast<geometric_t>(1.0) * RoundingError(3)) +
                (static_cast<geometric_t>(1.0) + RoundingError(3)),
            error0.x);
  EXPECT_EQ(0.0, point0.y);
  EXPECT_EQ(0.0, error0.y);
  EXPECT_EQ(0.0, point0.z);
  EXPECT_EQ(0.0, error0.z);

  PositionError existing_error1(0.0, -1.0, 0.0);
  auto [point1, error1] = Matrix::Translation(0.0, 1.0, 0.0)
                              .value()
                              .MultiplyWithError(point, &existing_error1);
  EXPECT_EQ(0.0, point1.x);
  EXPECT_EQ(0.0, error1.x);
  EXPECT_EQ(1.0, point1.y);
  EXPECT_EQ((static_cast<geometric_t>(1.0) * RoundingError(3)) +
                (static_cast<geometric_t>(1.0) + RoundingError(3)),
            error1.y);
  EXPECT_EQ(0.0, point1.z);
  EXPECT_EQ(0.0, error1.z);

  PositionError existing_error2(0.0, 0.0, -1.0);
  auto [point2, error2] = Matrix::Translation(0.0, 0.0, 1.0)
                              .value()
                              .MultiplyWithError(point, &existing_error2);
  EXPECT_EQ(0.0, point2.x);
  EXPECT_EQ(0.0, error2.x);
  EXPECT_EQ(0.0, point2.y);
  EXPECT_EQ(0.0, error2.y);
  EXPECT_EQ(1.0, point2.z);
  EXPECT_EQ((static_cast<geometric_t>(1.0) * RoundingError(3)) +
                (static_cast<geometric_t>(1.0) + RoundingError(3)),
            error2.z);
}

TEST(MatrixTest, MultiplyPointWithExistingErrorWithW) {
  Point point(0.0, 0.0, 0.0);

  std::array<std::array<geometric, 4>, 4> elements = {{{1.0, 0.0, 0.0, 0.0},
                                                       {0.0, 1.0, 0.0, 0.0},
                                                       {0.0, 0.0, 1.0, 0.0},
                                                       {0.0, 0.0, 0.0, 0.5}}};
  Matrix matrix = Matrix::Create(elements).value();

  geometric_t existing_error =
      (static_cast<geometric_t>(1.0) *
       (static_cast<geometric_t>(1.0) + RoundingError(3))) /
      static_cast<geometric_t>(0.5);
  geometric_t relative_error =
      ((static_cast<geometric_t>(1.0) +
        ((static_cast<geometric_t>(0.5) * RoundingError(3)) /
         static_cast<geometric_t>(0.5))) *
       (static_cast<geometric_t>(1.0) + RoundingError(1)));

  PositionError existing_error0(-1.0, 0.0, 0.0);
  auto [point0, error0] = matrix.MultiplyWithError(point, &existing_error0);
  EXPECT_EQ(0.0, point0.x);
  EXPECT_EQ(existing_error * relative_error, error0.x);
  EXPECT_EQ(0.0, point0.y);
  EXPECT_EQ(0.0, error0.y);
  EXPECT_EQ(0.0, point0.z);
  EXPECT_EQ(0.0, error0.z);

  PositionError existing_error1(0.0, -1.0, 0.0);
  auto [point1, error1] = matrix.MultiplyWithError(point, &existing_error1);
  EXPECT_EQ(0.0, point1.x);
  EXPECT_EQ(0.0, error1.x);
  EXPECT_EQ(0.0, point1.y);
  EXPECT_EQ(existing_error * relative_error, error1.y);
  EXPECT_EQ(0.0, point1.z);
  EXPECT_EQ(0.0, error1.z);

  PositionError existing_error2(0.0, 0.0, -1.0);
  auto [point2, error2] = matrix.MultiplyWithError(point, &existing_error2);
  EXPECT_EQ(0.0, point2.x);
  EXPECT_EQ(0.0, error2.x);
  EXPECT_EQ(0.0, point2.y);
  EXPECT_EQ(0.0, error2.y);
  EXPECT_EQ(0.0, point2.z);
  EXPECT_EQ(existing_error * relative_error, error2.z);
}

TEST(MatrixTest, InverseMultiplyPointWithExistingError) {
  Point point(0.0, 0.0, 0.0);

  PositionError existing_error0(1.0, 0.0, 0.0);
  auto [point0, error0] =
      Matrix::Translation(1.0, 0.0, 0.0)
          .value()
          .InverseMultiplyWithError(point, &existing_error0);
  EXPECT_EQ(-1.0, point0.x);
  EXPECT_EQ((static_cast<geometric_t>(1.0) * RoundingError(3)) +
                (static_cast<geometric_t>(1.0) + RoundingError(3)),
            error0.x);
  EXPECT_EQ(0.0, point0.y);
  EXPECT_EQ(0.0, error0.y);
  EXPECT_EQ(0.0, point0.z);
  EXPECT_EQ(0.0, error0.z);

  PositionError existing_error1(0.0, 1.0, 0.0);
  auto [point1, error1] =
      Matrix::Translation(0.0, 1.0, 0.0)
          .value()
          .InverseMultiplyWithError(point, &existing_error1);
  EXPECT_EQ(0.0, point1.x);
  EXPECT_EQ(0.0, error1.x);
  EXPECT_EQ(-1.0, point1.y);
  EXPECT_EQ((static_cast<geometric_t>(1.0) * RoundingError(3)) +
                (static_cast<geometric_t>(1.0) + RoundingError(3)),
            error1.y);
  EXPECT_EQ(0.0, point1.z);
  EXPECT_EQ(0.0, error1.z);

  PositionError existing_error2(0.0, 0.0, 1.0);
  auto [point2, error2] =
      Matrix::Translation(0.0, 0.0, 1.0)
          .value()
          .InverseMultiplyWithError(point, &existing_error2);
  EXPECT_EQ(0.0, point2.x);
  EXPECT_EQ(0.0, error2.x);
  EXPECT_EQ(0.0, point2.y);
  EXPECT_EQ(0.0, error2.y);
  EXPECT_EQ(-1.0, point2.z);
  EXPECT_EQ((static_cast<geometric_t>(1.0) * RoundingError(3)) +
                (static_cast<geometric_t>(1.0) + RoundingError(3)),
            error2.z);
}

TEST(MatrixTest, InverseMultiplyPointWithExistingErrorWithW) {
  Point point(0.0, 0.0, 0.0);

  std::array<std::array<geometric, 4>, 4> elements = {{{1.0, 0.0, 0.0, 0.0},
                                                       {0.0, 1.0, 0.0, 0.0},
                                                       {0.0, 0.0, 1.0, 0.0},
                                                       {0.0, 0.0, 0.0, 2.0}}};
  Matrix matrix = Matrix::Create(elements).value();

  geometric_t existing_error =
      (static_cast<geometric_t>(1.0) *
       (static_cast<geometric_t>(1.0) + RoundingError(3))) /
      static_cast<geometric_t>(0.5);
  geometric_t relative_error =
      ((static_cast<geometric_t>(1.0) +
        ((static_cast<geometric_t>(0.5) * RoundingError(3)) /
         static_cast<geometric_t>(0.5))) *
       (static_cast<geometric_t>(1.0) + RoundingError(1)));

  PositionError existing_error0(-1.0, 0.0, 0.0);
  auto [point0, error0] =
      matrix.InverseMultiplyWithError(point, &existing_error0);
  EXPECT_EQ(0.0, point0.x);
  EXPECT_EQ(existing_error * relative_error, error0.x);
  EXPECT_EQ(0.0, point0.y);
  EXPECT_EQ(0.0, error0.y);
  EXPECT_EQ(0.0, point0.z);
  EXPECT_EQ(0.0, error0.z);

  PositionError existing_error1(0.0, -1.0, 0.0);
  auto [point1, error1] =
      matrix.InverseMultiplyWithError(point, &existing_error1);
  EXPECT_EQ(0.0, point1.x);
  EXPECT_EQ(0.0, error1.x);
  EXPECT_EQ(0.0, point1.y);
  EXPECT_EQ(existing_error * relative_error, error1.y);
  EXPECT_EQ(0.0, point1.z);
  EXPECT_EQ(0.0, error1.z);

  PositionError existing_error2(0.0, 0.0, -1.0);
  auto [point2, error2] =
      matrix.InverseMultiplyWithError(point, &existing_error2);
  EXPECT_EQ(0.0, point2.x);
  EXPECT_EQ(0.0, error2.x);
  EXPECT_EQ(0.0, point2.y);
  EXPECT_EQ(0.0, error2.y);
  EXPECT_EQ(0.0, point2.z);
  EXPECT_EQ(existing_error * relative_error, error2.z);
}

TEST(MatrixTest, MultiplyRay) {
  Matrix matrix = Matrix::Scalar(2.0, 2.0, 2.0).value();
  Ray transformed =
      matrix.Multiply(Ray(Point(1.0, 1.0, 1.0), Vector(1.0, 1.0, 1.0)));
  EXPECT_EQ(Point(2.0, 2.0, 2.0), transformed.origin);
  EXPECT_EQ(Vector(2.0, 2.0, 2.0), transformed.direction);
}

TEST(MatrixTest, InverseMultiplyRay) {
  Matrix matrix = Matrix::Scalar(0.5, 0.5, 0.5).value();
  Ray transformed =
      matrix.InverseMultiply(Ray(Point(1.0, 1.0, 1.0), Vector(1.0, 1.0, 1.0)));
  EXPECT_EQ(Point(2.0, 2.0, 2.0), transformed.origin);
  EXPECT_EQ(Vector(2.0, 2.0, 2.0), transformed.direction);
}

TEST(MatrixTest, MultiplyRayWithError) {
  Matrix matrix = Matrix::Scalar(2.0, 2.0, 2.0).value();
  Ray transformed = matrix.MultiplyWithError(
      Ray(Point(1.0, 1.0, 1.0), Vector(1.0, 1.0, 1.0)));
  EXPECT_EQ(Point(2.0, 2.0, 2.0) +
                transformed.direction *
                    ((static_cast<geometric_t>(2.0) *
                          (static_cast<geometric_t>(2.0) * RoundingError(3)) +
                      static_cast<geometric_t>(2.0) *
                          (static_cast<geometric_t>(2.0) * RoundingError(3)) +
                      static_cast<geometric_t>(2.0) *
                          (static_cast<geometric_t>(2.0) * RoundingError(3))) /
                     static_cast<geometric_t>(12.0)),
            transformed.origin);
  EXPECT_EQ(Vector(2.0, 2.0, 2.0), transformed.direction);
}

TEST(MatrixTest, InverseMultiplyRayWithError) {
  Matrix matrix = Matrix::Scalar(0.5, 0.5, 0.5).value();
  Ray transformed = matrix.InverseMultiplyWithError(
      Ray(Point(1.0, 1.0, 1.0), Vector(1.0, 1.0, 1.0)));
  EXPECT_EQ(Point(2.0, 2.0, 2.0) +
                transformed.direction *
                    ((static_cast<geometric_t>(2.0) *
                          (static_cast<geometric_t>(2.0) * RoundingError(3)) +
                      static_cast<geometric_t>(2.0) *
                          (static_cast<geometric_t>(2.0) * RoundingError(3)) +
                      static_cast<geometric_t>(2.0) *
                          (static_cast<geometric_t>(2.0) * RoundingError(3))) /
                     static_cast<geometric_t>(12.0)),
            transformed.origin);
  EXPECT_EQ(Vector(2.0, 2.0, 2.0), transformed.direction);
}

TEST(MatrixTest, MultiplyVector) {
  geometric half_pi = std::acos(0.0);
  Vector rotate_x = Matrix::Rotation(half_pi, 1.0, 0.0, 0.0)
                        .value()
                        .Multiply(Vector(1.0, 1.0, 1.0));
  EXPECT_NEAR(1.0, rotate_x.x, static_cast<geometric>(0.0001));
  EXPECT_NEAR(-1.0, rotate_x.y, static_cast<geometric>(0.0001));
  EXPECT_NEAR(1.0, rotate_x.z, static_cast<geometric>(0.0001));

  Vector rotate_y = Matrix::Rotation(half_pi, 0.0, 1.0, 0.0)
                        .value()
                        .Multiply(Vector(1.0, 1.0, 1.0));
  EXPECT_NEAR(1.0, rotate_y.x, static_cast<geometric>(0.0001));
  EXPECT_NEAR(1.0, rotate_y.y, static_cast<geometric>(0.0001));
  EXPECT_NEAR(-1.0, rotate_y.z, static_cast<geometric>(0.0001));

  Vector rotate_z = Matrix::Rotation(half_pi, 0.0, 0.0, 1.0)
                        .value()
                        .Multiply(Vector(1.0, 1.0, 1.0));
  EXPECT_NEAR(-1.0, rotate_z.x, static_cast<geometric>(0.0001));
  EXPECT_NEAR(1.0, rotate_z.y, static_cast<geometric>(0.0001));
  EXPECT_NEAR(1.0, rotate_z.z, static_cast<geometric>(0.0001));
}

TEST(MatrixTest, InverseMultiplyVector) {
  geometric half_pi = std::acos(0.0);
  Vector rotate_x = Matrix::Rotation(half_pi, 1.0, 0.0, 0.0)
                        .value()
                        .InverseMultiply(Vector(1.0, 1.0, 1.0));
  EXPECT_NEAR(1.0, rotate_x.x, static_cast<geometric>(0.0001));
  EXPECT_NEAR(1.0, rotate_x.y, static_cast<geometric>(0.0001));
  EXPECT_NEAR(-1.0, rotate_x.z, static_cast<geometric>(0.0001));

  Vector rotate_y = Matrix::Rotation(half_pi, 0.0, 1.0, 0.0)
                        .value()
                        .InverseMultiply(Vector(1.0, 1.0, 1.0));
  EXPECT_NEAR(-1.0, rotate_y.x, static_cast<geometric>(0.0001));
  EXPECT_NEAR(1.0, rotate_y.y, static_cast<geometric>(0.0001));
  EXPECT_NEAR(1.0, rotate_y.z, static_cast<geometric>(0.0001));

  Vector rotate_z = Matrix::Rotation(half_pi, 0.0, 0.0, 1.0)
                        .value()
                        .InverseMultiply(Vector(1.0, 1.0, 1.0));
  EXPECT_NEAR(1.0, rotate_z.x, static_cast<geometric>(0.0001));
  EXPECT_NEAR(-1.0, rotate_z.y, static_cast<geometric>(0.0001));
  EXPECT_NEAR(1.0, rotate_z.z, static_cast<geometric>(0.0001));
}

TEST(MatrixTest, TransposeMultiplyVector) {
  geometric half_pi = std::acos(0.0);
  Vector rotate_x = Matrix::Rotation(half_pi, 1.0, 0.0, 0.0)
                        .value()
                        .TransposeMultiply(Vector(1.0, 1.0, 1.0));
  EXPECT_NEAR(1.0, rotate_x.x, static_cast<geometric>(0.0001));
  EXPECT_NEAR(1.0, rotate_x.y, static_cast<geometric>(0.0001));
  EXPECT_NEAR(-1.0, rotate_x.z, static_cast<geometric>(0.0001));

  Vector rotate_y = Matrix::Rotation(half_pi, 0.0, 1.0, 0.0)
                        .value()
                        .TransposeMultiply(Vector(1.0, 1.0, 1.0));
  EXPECT_NEAR(-1.0, rotate_y.x, static_cast<geometric>(0.0001));
  EXPECT_NEAR(1.0, rotate_y.y, static_cast<geometric>(0.0001));
  EXPECT_NEAR(1.0, rotate_y.z, static_cast<geometric>(0.0001));

  Vector rotate_z = Matrix::Rotation(half_pi, 0.0, 0.0, 1.0)
                        .value()
                        .TransposeMultiply(Vector(1.0, 1.0, 1.0));
  EXPECT_NEAR(1.0, rotate_z.x, static_cast<geometric>(0.0001));
  EXPECT_NEAR(-1.0, rotate_z.y, static_cast<geometric>(0.0001));
  EXPECT_NEAR(1.0, rotate_z.z, static_cast<geometric>(0.0001));
}

TEST(MatrixTest, InverseTransposeMultiplyVector) {
  geometric half_pi = std::acos(0.0);
  Vector rotate_x = Matrix::Rotation(half_pi, 1.0, 0.0, 0.0)
                        .value()
                        .InverseTransposeMultiply(Vector(1.0, 1.0, 1.0));
  EXPECT_NEAR(1.0, rotate_x.x, static_cast<geometric>(0.0001));
  EXPECT_NEAR(-1.0, rotate_x.y, static_cast<geometric>(0.0001));
  EXPECT_NEAR(1.0, rotate_x.z, static_cast<geometric>(0.0001));

  Vector rotate_y = Matrix::Rotation(half_pi, 0.0, 1.0, 0.0)
                        .value()
                        .InverseTransposeMultiply(Vector(1.0, 1.0, 1.0));
  EXPECT_NEAR(1.0, rotate_y.x, static_cast<geometric>(0.0001));
  EXPECT_NEAR(1.0, rotate_y.y, static_cast<geometric>(0.0001));
  EXPECT_NEAR(-1.0, rotate_y.z, static_cast<geometric>(0.0001));

  Vector rotate_z = Matrix::Rotation(half_pi, 0.0, 0.0, 1.0)
                        .value()
                        .InverseTransposeMultiply(Vector(1.0, 1.0, 1.0));
  EXPECT_NEAR(-1.0, rotate_z.x, static_cast<geometric>(0.0001));
  EXPECT_NEAR(1.0, rotate_z.y, static_cast<geometric>(0.0001));
  EXPECT_NEAR(1.0, rotate_z.z, static_cast<geometric>(0.0001));
}

TEST(MatrixTest, Equals) {
  Matrix left = Matrix::Scalar(2.0, 1.0, 1.0).value();
  EXPECT_TRUE(left == left);

  Matrix right = Matrix::Scalar(1.0, 1.0, 1.0).value();
  EXPECT_FALSE(left == right);
}

TEST(MatrixTest, LessThan) {
  Matrix left = Matrix::Scalar(2.0, 1.0, 1.0).value();
  EXPECT_FALSE(left < left);

  Matrix right = Matrix::Scalar(1.0, 1.0, 1.0).value();
  EXPECT_FALSE(right < left);
  EXPECT_TRUE(left < right);
}

}  // namespace
}  // namespace iris
