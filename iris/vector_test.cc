#include "iris/vector.h"

#include <utility>

#include "googletest/include/gtest/gtest.h"

namespace iris {
namespace {

TEST(VectorTest, Create) {
  Vector vector(1.0, 2.0, 3.0);
  EXPECT_EQ(1.0, vector.x);
  EXPECT_EQ(2.0, vector.y);
  EXPECT_EQ(3.0, vector.z);
}

TEST(VectorTest, Subscript) {
  Vector vector(1.0, 2.0, 3.0);
  EXPECT_EQ(1.0, vector[0]);
  EXPECT_EQ(2.0, vector[1]);
  EXPECT_EQ(3.0, vector[2]);
}

TEST(VectorTest, Length) {
  Vector vector0(2.0, 0.0, 0.0);
  EXPECT_EQ(2.0, vector0.Length());

  Vector vector1(0.0, 2.0, 0.0);
  EXPECT_EQ(2.0, vector1.Length());

  Vector vector2(0.0, 0.0, 2.0);
  EXPECT_EQ(2.0, vector2.Length());
}

TEST(VectorTest, Negate) {
  Vector vector(1.0, 2.0, 3.0);
  EXPECT_EQ(Vector(-1.0, -2.0, -3.0), -vector);
}

TEST(VectorTest, Subtract) {
  Vector minuend(3.0, 4.0, 5.0);
  Vector subtrahend(2.0, 1.0, 3.0);
  EXPECT_EQ(Vector(1.0, 3.0, 2.0), minuend - subtrahend);
}

TEST(VectorTest, Add) {
  Vector addend0(3.0, 4.0, 5.0);
  Vector addend1(2.0, 2.0, 3.0);
  EXPECT_EQ(Vector(5.0, 6.0, 8.0), addend0 + addend1);
}

TEST(VectorTest, Scale) {
  Vector vector(1.0, 2.0, 3.0);
  geometric_t scalar = 2.0;
  EXPECT_EQ(Vector(2.0, 4.0, 6.0), vector * scalar);
  EXPECT_EQ(Vector(2.0, 4.0, 6.0), scalar * vector);
}

TEST(VectorTest, Divide) {
  Vector dividend(2.0, 4.0, 6.0);
  geometric_t divisor = 2.0;
  EXPECT_EQ(Vector(1.0, 2.0, 3.0), dividend / divisor);
}

TEST(VectorTest, DotProduct) {
  Vector operand0(1.0, 2.0, 3.0);
  Vector operand1(3.0, 4.0, 5.0);
  EXPECT_EQ(26.0, DotProduct(operand0, operand1));
  EXPECT_EQ(-26.0, DotProduct(operand0, -operand1));
}

TEST(VectorTest, ClampedDotProduct) {
  Vector operand0(1.0, 2.0, 3.0);
  Vector operand1(3.0, 4.0, 5.0);
  EXPECT_EQ(-1.0, ClampedDotProduct(operand0, -operand1));
  EXPECT_EQ(1.0, ClampedDotProduct(operand0, operand1));
}

TEST(VectorTest, ClampedAbsDotProduct) {
  Vector operand0(1.0, 2.0, 3.0);
  Vector operand1(3.0, 4.0, 5.0);
  EXPECT_EQ(1.0, ClampedAbsDotProduct(operand0, operand1));
  EXPECT_EQ(1.0, ClampedAbsDotProduct(operand0, -operand1));
}

TEST(VectorTest, CrossProduct) {
  Vector operand0(1.0, 2.0, 3.0);
  Vector operand1(3.0, 2.0, 1.0);
  EXPECT_EQ(Vector(-4.0, 8.0, -4.0), CrossProduct(operand0, operand1));
}

TEST(VectorTest, Normalize) {
  Vector vector(2.0, 0.0, 0.0);
  EXPECT_EQ(Vector(1.0, 0.0, 0.0), Normalize(vector));

  geometric_t length_squared, length;
  EXPECT_EQ(Vector(1.0, 0.0, 0.0), Normalize(vector, &length_squared, &length));
  EXPECT_EQ(4.0, length_squared);
  EXPECT_EQ(2.0, length);
}

TEST(VectorTest, CoordinateSystem) {
  std::pair<Vector, Vector> sys0 = CoordinateSystem(Vector(1.0, 0.0, 0.0));
  EXPECT_EQ(Vector(0.0, 0.0, 1.0), sys0.first);
  EXPECT_EQ(Vector(0.0, -1.0, 0.0), sys0.second);

  std::pair<Vector, Vector> sys1 = CoordinateSystem(Vector(0.0, 1.0, 0.0));
  EXPECT_EQ(Vector(0.0, 0.0, -1.0), sys1.first);
  EXPECT_EQ(Vector(-1.0, 0.0, 0.0), sys1.second);

  std::pair<Vector, Vector> sys2 = CoordinateSystem(Vector(0.0, 0.0, 1.0));
  EXPECT_EQ(Vector(0.0, 1.0, 0.0), sys2.first);
  EXPECT_EQ(Vector(-1.0, 0.0, 0.0), sys2.second);
}

TEST(VectorTest, DiminishedAxis) {
  EXPECT_EQ(Vector::X_AXIS, Vector(1.0, -2.0, 3.0).DiminishedAxis());
  EXPECT_EQ(Vector::Y_AXIS, Vector(-2.0, 1.0, 3.0).DiminishedAxis());
  EXPECT_EQ(Vector::Z_AXIS, Vector(3.0, -2.0, 1.0).DiminishedAxis());
}

TEST(VectorTest, DominantAxis) {
  EXPECT_EQ(Vector::X_AXIS, Vector(3.0, -2.0, 1.0).DominantAxis());
  EXPECT_EQ(Vector::Y_AXIS, Vector(-2.0, 3.0, 1.0).DominantAxis());
  EXPECT_EQ(Vector::Z_AXIS, Vector(1.0, -2.0, 3.0).DominantAxis());
}

TEST(VectorTest, AlignAgainst) {
  Vector v0(1.0, 1.0, 1.0);
  EXPECT_EQ(-v0, v0.AlignAgainst(v0));
  EXPECT_EQ(v0, v0.AlignAgainst(-v0));
}

TEST(VectorTest, AlignWith) {
  Vector v0(1.0, 1.0, 1.0);
  EXPECT_EQ(v0, v0.AlignWith(v0));
  EXPECT_EQ(-v0, v0.AlignWith(-v0));
}

}  // namespace
}  // namespace iris
