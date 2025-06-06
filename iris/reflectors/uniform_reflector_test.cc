#include "iris/reflectors/uniform_reflector.h"

#include "googletest/include/gtest/gtest.h"

namespace iris {
namespace reflectors {
namespace {

TEST(UniformReflector, WithZeroReflectance) {
  ReferenceCounted<Reflector> reflector = CreateUniformReflector(0.0);
  EXPECT_FALSE(reflector);
}

TEST(UniformReflector, WithValidReflectance) {
  ReferenceCounted<Reflector> reflector = CreateUniformReflector(0.25);
  EXPECT_EQ(0.25, reflector->Reflectance(0.5));
  EXPECT_EQ(0.25, reflector->Reflectance(1.0));
  EXPECT_EQ(0.25, reflector->Reflectance(1.5));
}

TEST(UniformReflector, WithLargeReflectance) {
  ReferenceCounted<Reflector> reflector = CreateUniformReflector(1.25);
  EXPECT_EQ(1.0, reflector->Reflectance(0.5));
  EXPECT_EQ(1.0, reflector->Reflectance(1.0));
  EXPECT_EQ(1.0, reflector->Reflectance(1.5));
}

}  // namespace
}  // namespace reflectors
}  // namespace iris
