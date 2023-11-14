#include "iris/spectra/sampled_spectrum.h"

#include "googletest/include/gtest/gtest.h"

TEST(SampledSpectrum, SingleSample) {
  std::map<iris::visual, iris::visual> samples = {
      {static_cast<iris::visual>(1.0), static_cast<iris::visual>(2.0)}};
  iris::spectra::SampledSpectrum spectrum(samples);
  EXPECT_EQ(2.0, spectrum.Intensity(0.5));
  EXPECT_EQ(2.0, spectrum.Intensity(1.0));
  EXPECT_EQ(2.0, spectrum.Intensity(1.5));
  EXPECT_EQ(2.0, spectrum.Intensity(2.0));
  EXPECT_EQ(2.0, spectrum.Intensity(2.5));
  EXPECT_EQ(2.0, spectrum.Intensity(3.0));
  EXPECT_EQ(2.0, spectrum.Intensity(3.5));
}

TEST(SampledSpectrum, Intensity) {
  std::map<iris::visual, iris::visual> samples = {
      {static_cast<iris::visual>(1.0), static_cast<iris::visual>(2.0)},
      {static_cast<iris::visual>(2.0), static_cast<iris::visual>(3.0)},
      {static_cast<iris::visual>(3.0), static_cast<iris::visual>(2.0)}};
  iris::spectra::SampledSpectrum spectrum(samples);
  EXPECT_EQ(2.0, spectrum.Intensity(0.5));
  EXPECT_EQ(2.0, spectrum.Intensity(1.0));
  EXPECT_EQ(2.5, spectrum.Intensity(1.5));
  EXPECT_EQ(3.0, spectrum.Intensity(2.0));
  EXPECT_EQ(2.5, spectrum.Intensity(2.5));
  EXPECT_EQ(2.0, spectrum.Intensity(3.0));
  EXPECT_EQ(2.0, spectrum.Intensity(3.5));
}