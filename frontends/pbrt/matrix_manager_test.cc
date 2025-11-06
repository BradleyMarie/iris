#include "frontends/pbrt/matrix_manager.h"

#include <cstdlib>
#include <memory>

#include "googletest/include/gtest/gtest.h"

namespace iris {
namespace pbrt_frontend {
namespace {

using ::testing::ExitedWithCode;

TEST(MatrixManager, InitialState) {
  MatrixManager matrix_manager;

  EXPECT_EQ(Matrix::Identity(), matrix_manager.Get().start);
  EXPECT_EQ(Matrix::Identity(), matrix_manager.Get().end);
}

TEST(MatrixManager, MissingAttributeEnd) {
  std::function<void()> body = []() {
    MatrixManager matrix_manager;

    matrix_manager.AttributeBegin();
  };

  EXPECT_EXIT(body(), ExitedWithCode(EXIT_FAILURE),
              "ERROR: Missing AttributeEnd directive");
}

TEST(MatrixManager, MissingTransformEnd) {
  std::function<void()> body = []() {
    MatrixManager matrix_manager;

    matrix_manager.TransformBegin();
  };

  EXPECT_EXIT(body(), ExitedWithCode(EXIT_FAILURE),
              "ERROR: Missing TransformEnd directive");
}

TEST(MatrixManager, BadAttributeEnd) {
  MatrixManager matrix_manager;

  EXPECT_EXIT(matrix_manager.AttributeEnd(), ExitedWithCode(EXIT_FAILURE),
              "ERROR: Missing AttributeBegin directive");
}

TEST(MatrixManager, BadTransformEnd) {
  MatrixManager matrix_manager;

  EXPECT_EXIT(matrix_manager.TransformEnd(), ExitedWithCode(EXIT_FAILURE),
              "ERROR: Missing TransformBegin directive");
}

TEST(MatrixManager, ActiveTransformStartTime) {
  MatrixManager matrix_manager;

  matrix_manager.ActiveTransform(MatrixManager::ActiveTransformation::START);
  matrix_manager.Translate(1.0, 1.0, 1.0);

  EXPECT_EQ(Matrix::Translation(1.0, 1.0, 1.0).value(),
            matrix_manager.Get().start);
  EXPECT_EQ(Matrix::Identity(), matrix_manager.Get().end);
}

TEST(MatrixManager, ActiveTransformStartEnd) {
  MatrixManager matrix_manager;

  matrix_manager.ActiveTransform(MatrixManager::ActiveTransformation::END);
  matrix_manager.Translate(1.0, 1.0, 1.0);

  EXPECT_EQ(Matrix::Identity(), matrix_manager.Get().start);
  EXPECT_EQ(Matrix::Translation(1.0, 1.0, 1.0).value(),
            matrix_manager.Get().end);
}

TEST(MatrixManager, ActiveTransformStartAll) {
  MatrixManager matrix_manager;

  matrix_manager.ActiveTransform(MatrixManager::ActiveTransformation::ALL);
  matrix_manager.Translate(1.0, 1.0, 1.0);

  EXPECT_EQ(Matrix::Translation(1.0, 1.0, 1.0).value(),
            matrix_manager.Get().start);
  EXPECT_EQ(Matrix::Translation(1.0, 1.0, 1.0).value(),
            matrix_manager.Get().end);
}

TEST(MatrixManager, Identity) {
  MatrixManager matrix_manager;

  matrix_manager.Translate(1.0, 1.0, 1.0);
  matrix_manager.Identity();

  EXPECT_EQ(Matrix::Identity(), matrix_manager.Get().start);
}

TEST(MatrixManager, Translate) {
  MatrixManager matrix_manager;

  matrix_manager.Translate(1.0, 2.0, 3.0);

  EXPECT_EQ(Matrix::Translation(1.0, 2.0, 3.0).value(),
            matrix_manager.Get().start);
}

TEST(MatrixManager, ScaleX) {
  MatrixManager matrix_manager;

  EXPECT_EXIT(matrix_manager.Scale(0.0, 1.0, 1.0), ExitedWithCode(EXIT_FAILURE),
              "ERROR: Parameters to Scale must be non-zero");
}

TEST(MatrixManager, ScaleY) {
  MatrixManager matrix_manager;

  EXPECT_EXIT(matrix_manager.Scale(1.0, 0.0, 1.0), ExitedWithCode(EXIT_FAILURE),
              "ERROR: Parameters to Scale must be non-zero");
}

TEST(MatrixManager, ScaleZ) {
  MatrixManager matrix_manager;

  EXPECT_EXIT(matrix_manager.Scale(1.0, 1.0, 0.0), ExitedWithCode(EXIT_FAILURE),
              "ERROR: Parameters to Scale must be non-zero");
}

TEST(MatrixManager, Scale) {
  MatrixManager matrix_manager;

  matrix_manager.Scale(2.0, 3.0, 4.0);

  EXPECT_EQ(Matrix::Scalar(2.0, 3.0, 4.0).value(), matrix_manager.Get().start);
}

TEST(MatrixManager, RotateZeroLength) {
  MatrixManager matrix_manager;

  EXPECT_EXIT(
      matrix_manager.Rotate(1.0, 0.0, 0.0, 0.0), ExitedWithCode(EXIT_FAILURE),
      "ERROR: One of the x, y, or z parameters to Rotate must be non-zero");
}

TEST(MatrixManager, Rotate) {
  MatrixManager matrix_manager;

  matrix_manager.Rotate(0.0, 2.0, 3.0, 4.0);

  EXPECT_EQ(Matrix::Rotation(0.0, 2.0, 3.0, 4.0).value(),
            matrix_manager.Get().start);
}

TEST(MatrixManager, LookAtSamePoint) {
  MatrixManager matrix_manager;

  EXPECT_EXIT(
      matrix_manager.LookAt(1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0),
      ExitedWithCode(EXIT_FAILURE),
      "ERROR: eye and look_at parameters to LookAt must not be equal");
}

TEST(MatrixManager, LookAtNoUp) {
  MatrixManager matrix_manager;

  EXPECT_EXIT(
      matrix_manager.LookAt(1.0, 2.0, 3.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0),
      ExitedWithCode(EXIT_FAILURE),
      "ERROR: One of up_x, up_y, or up_z parameters to LookAt must be "
      "non-zero");
}

TEST(MatrixManager, LookAtUpParallel) {
  MatrixManager matrix_manager;

  EXPECT_EXIT(
      matrix_manager.LookAt(0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0),
      ExitedWithCode(EXIT_FAILURE),
      "ERROR: Up parameter to LookAt must not be parallel with look direction");
}

TEST(MatrixManager, LookAt) {
  MatrixManager matrix_manager;
  matrix_manager.LookAt(1.0, 1.0, 1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);

  EXPECT_EQ(Matrix::LookAt(1.0, 1.0, 1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0)
                .value()
                .Inverse(),
            matrix_manager.Get().start);
}

TEST(MatrixManager, TransformNotInvertible) {
  MatrixManager matrix_manager;

  EXPECT_EXIT(matrix_manager.Transform(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                                       0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Uninvertible matrix specified for Transform directive");
}

TEST(MatrixManager, Transform) {
  MatrixManager matrix_manager;
  matrix_manager.Transform(1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0,
                           1.0, 0.0, 2.0, 3.0, 4.0, 1.0);

  EXPECT_EQ(Matrix::Translation(2.0, 3.0, 4.0).value(),
            matrix_manager.Get().start);
}

TEST(MatrixManager, ConcatTransformNotInvertible) {
  MatrixManager matrix_manager;

  EXPECT_EXIT(
      matrix_manager.ConcatTransform(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                                     0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0),
      ExitedWithCode(EXIT_FAILURE),
      "ERROR: Uninvertible matrix specified for ConcatTransform directive");
}

TEST(MatrixManager, ConcatTransform) {
  MatrixManager matrix_manager;
  matrix_manager.ConcatTransform(1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0,
                                 0.0, 1.0, 0.0, 2.0, 3.0, 4.0, 1.0);

  EXPECT_EQ(Matrix::Translation(2.0, 3.0, 4.0).value(),
            matrix_manager.Get().start);
}

TEST(MatrixManager, CoordSysTransformNotFound) {
  MatrixManager matrix_manager;

  EXPECT_EXIT(matrix_manager.CoordSysTransform("scalar"),
              ExitedWithCode(EXIT_FAILURE),
              "Unknown coordinate system name specified for directive "
              "CoordSysTransform: \"scalar\"");
}

TEST(MatrixManager, CoordSysTransform) {
  MatrixManager matrix_manager;

  matrix_manager.Translate(1.0, 2.0, 3.0);
  EXPECT_EQ(Matrix::Translation(1.0, 2.0, 3.0).value(),
            matrix_manager.Get().start);

  matrix_manager.CoordinateSystem("a");
  EXPECT_EQ(Matrix::Translation(1.0, 2.0, 3.0).value(),
            matrix_manager.Get().start);

  matrix_manager.Identity();
  EXPECT_EQ(Matrix::Identity(), matrix_manager.Get().start);

  matrix_manager.CoordSysTransform("a");
  EXPECT_EQ(Matrix::Translation(1.0, 2.0, 3.0).value(),
            matrix_manager.Get().start);
}

TEST(MatrixManager, MismatchedPushAndPop) {
  std::function<void()> body0 = []() {
    MatrixManager matrix_manager;

    matrix_manager.TransformBegin();
    matrix_manager.AttributeEnd();
  };

  EXPECT_EXIT(body0(), ExitedWithCode(EXIT_FAILURE),
              "ERROR: Mismatched TransformBegin and TransformEnd directives");

  std::function<void()> body1 = []() {
    MatrixManager matrix_manager;

    matrix_manager.AttributeBegin();
    matrix_manager.TransformEnd();
  };

  EXPECT_EXIT(body1(), ExitedWithCode(EXIT_FAILURE),
              "ERROR: Mismatched AttributeBegin and AttributeEnd directives");
}

TEST(MatrixManager, TransformBeginTransformEnd) {
  MatrixManager matrix_manager;

  matrix_manager.TransformBegin();
  EXPECT_EQ(Matrix::Identity(), matrix_manager.Get().start);

  matrix_manager.Translate(1.0, 2.0, 3.0);
  EXPECT_EQ(Matrix::Translation(1.0, 2.0, 3.0).value(),
            matrix_manager.Get().start);

  matrix_manager.TransformEnd();
  EXPECT_EQ(Matrix::Identity(), matrix_manager.Get().start);
}

TEST(MatrixManager, PushPop) {
  MatrixManager matrix_manager;

  matrix_manager.AttributeBegin();
  EXPECT_EQ(Matrix::Identity(), matrix_manager.Get().start);

  matrix_manager.Translate(1.0, 2.0, 3.0);
  EXPECT_EQ(Matrix::Translation(1.0, 2.0, 3.0).value(),
            matrix_manager.Get().start);

  matrix_manager.AttributeEnd();
  EXPECT_EQ(Matrix::Identity(), matrix_manager.Get().start);
}

}  // namespace
}  // namespace pbrt_frontend
}  // namespace iris
