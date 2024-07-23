#include "frontends/pbrt/spectrum_managers/internal/color_reflector.h"

#include "googletest/include/gtest/gtest.h"

TEST(ColorReflector, Reflectance) {
  iris::Color color(0.25, 0.5, 0.75, iris::Color::LINEAR_SRGB);
  iris::pbrt_frontend::spectrum_managers::internal::ColorReflector reflector(
      color);
  EXPECT_EQ(0.25, reflector.Reflectance(0.5));
  EXPECT_EQ(0.50, reflector.Reflectance(1.5));
  EXPECT_EQ(0.75, reflector.Reflectance(2.5));
}

TEST(ColorReflector, Albedo) {
  iris::pbrt_frontend::Color color({0.25, 0.5, 0.75},
                                   iris::pbrt_frontend::Color::RGB);
  iris::pbrt_frontend::spectrum_managers::internal::ColorReflector reflector(
      color);
  EXPECT_NEAR(0.464875579, reflector.Albedo(), 0.001);
}

TEST(ColorReflector, AlbedoWhite) {
  iris::pbrt_frontend::Color color({1.0, 1.0, 1.0},
                                   iris::pbrt_frontend::Color::RGB);
  iris::pbrt_frontend::spectrum_managers::internal::ColorReflector reflector(
      color);
  EXPECT_LE(1.0, reflector.Albedo());
  EXPECT_NEAR(1.0, reflector.Albedo(), 0.001);
}