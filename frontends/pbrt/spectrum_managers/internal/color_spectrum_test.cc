#include "frontends/pbrt/spectrum_managers/internal/color_spectrum.h"

#include "googletest/include/gtest/gtest.h"

TEST(ColorSpectrum, Intensity) {
  iris::Color color(0.25, 0.5, 0.75, iris::Color::LINEAR_SRGB);
  iris::pbrt_frontend::spectrum_managers::internal::ColorSpectrum spectrum(
      color);
  EXPECT_EQ(0.25, spectrum.Intensity(0.5));
  EXPECT_EQ(0.50, spectrum.Intensity(1.5));
  EXPECT_EQ(0.75, spectrum.Intensity(2.5));
}

TEST(ColorSpectrum, ReflectorFromPbrtColor) {
  iris::pbrt_frontend::Color color({0.25, 0.5, 0.75},
                                   iris::pbrt_frontend::Color::RGB);
  iris::pbrt_frontend::spectrum_managers::internal::ColorSpectrum spectrum(
      color);
  EXPECT_EQ(0.25, spectrum.Intensity(0.5));
  EXPECT_EQ(0.50, spectrum.Intensity(1.5));
  EXPECT_EQ(0.75, spectrum.Intensity(2.5));
}