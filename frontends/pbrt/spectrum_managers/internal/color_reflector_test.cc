#include "frontends/pbrt/spectrum_managers/internal/color_reflector.h"

#include "googletest/include/gtest/gtest.h"

namespace iris {
namespace pbrt_frontend {
namespace spectrum_managers {
namespace internal {
namespace {

TEST(ColorReflector, Reflectance) {
  iris::Color color(0.25, 0.5, 0.75, iris::Color::LINEAR_SRGB);
  ColorReflector reflector(color);
  EXPECT_EQ(0.25, reflector.Reflectance(0.5));
  EXPECT_EQ(0.50, reflector.Reflectance(1.5));
  EXPECT_EQ(0.75, reflector.Reflectance(2.5));
}

TEST(ColorReflector, ReflectorFromPbrtColor) {
  Color color({0.25, 0.5, 0.75}, Color::RGB);
  ColorReflector reflector(color);
  EXPECT_EQ(0.25, reflector.Reflectance(0.5));
  EXPECT_EQ(0.50, reflector.Reflectance(1.5));
  EXPECT_EQ(0.75, reflector.Reflectance(2.5));
}

}  // namespace
}  // namespace internal
}  // namespace spectrum_managers
}  // namespace pbrt_frontend
}  // namespace iris