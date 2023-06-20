#include "iris/spectra/uniform_spectrum.h"

#include "googletest/include/gtest/gtest.h"

TEST(UniformSpectrum, Intensity) {
  iris::spectra::UniformSpectrum spectrum(0.25);
  EXPECT_EQ(0.25, spectrum.Intensity(0.5));
  EXPECT_EQ(0.25, spectrum.Intensity(1.0));
  EXPECT_EQ(0.25, spectrum.Intensity(1.5));
}