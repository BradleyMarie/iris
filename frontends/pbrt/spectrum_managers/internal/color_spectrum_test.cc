#include "frontends/pbrt/spectrum_managers/internal/color_spectrum.h"

#include "googletest/include/gtest/gtest.h"

namespace iris {
namespace pbrt_frontend {
namespace spectrum_managers {
namespace internal {
namespace {

TEST(ColorSpectrum, Intensity) {
  iris::Color color(0.25, 0.5, 0.75, iris::Color::LINEAR_SRGB);
  ColorSpectrum spectrum(color);
  EXPECT_EQ(0.25, spectrum.Intensity(0.5));
  EXPECT_EQ(0.50, spectrum.Intensity(1.5));
  EXPECT_EQ(0.75, spectrum.Intensity(2.5));
}

TEST(ColorSpectrum, SpectrumFromPbrtColor) {
  Color color({0.25, 0.5, 0.75}, Color::RGB);
  ColorSpectrum spectrum(color);
  EXPECT_EQ(0.25, spectrum.Intensity(0.5));
  EXPECT_EQ(0.50, spectrum.Intensity(1.5));
  EXPECT_EQ(0.75, spectrum.Intensity(2.5));
}

}  // namespace
}  // namespace internal
}  // namespace spectrum_managers
}  // namespace pbrt_frontend
}  // namespace iris