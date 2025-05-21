#include "frontends/pbrt/spectrum_managers/internal/color_reflector.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/color.h"
#include "iris/float.h"
#include "iris/reference_counted.h"
#include "iris/reflector.h"

namespace iris {
namespace pbrt_frontend {
namespace spectrum_managers {
namespace internal {
namespace {

TEST(ColorReflector, Null) {
  EXPECT_FALSE(MakeColorReflector(0.0, 0.0, 0.0, Color::LINEAR_SRGB));
  EXPECT_TRUE(MakeColorReflector(1.0, 0.0, 0.0, Color::LINEAR_SRGB));
  EXPECT_TRUE(MakeColorReflector(0.0, 1.0, 0.0, Color::LINEAR_SRGB));
  EXPECT_TRUE(MakeColorReflector(0.0, 0.0, 1.0, Color::LINEAR_SRGB));
}

TEST(ColorReflector, FromRgb) {
  ReferenceCounted<Reflector> reflector =
      MakeColorReflector(0.25, 0.5, 0.75, Color::LINEAR_SRGB);
  ASSERT_TRUE(reflector);
  EXPECT_EQ(0.25, reflector->Reflectance(0.5));
  EXPECT_EQ(0.50, reflector->Reflectance(1.5));
  EXPECT_EQ(0.75, reflector->Reflectance(2.5));
}

}  // namespace
}  // namespace internal
}  // namespace spectrum_managers
}  // namespace pbrt_frontend
}  // namespace iris
