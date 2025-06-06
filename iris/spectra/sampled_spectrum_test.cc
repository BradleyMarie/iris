#include "iris/spectra/sampled_spectrum.h"

#include <map>

#include "googletest/include/gtest/gtest.h"
#include "iris/float.h"
#include "iris/reference_counted.h"
#include "iris/spectrum.h"

namespace iris {
namespace spectra {
namespace {

TEST(SampledSpectrum, Null) {
  EXPECT_FALSE(MakeSampledSpectrum({}));
  EXPECT_FALSE(MakeSampledSpectrum({{1.0, 0.0}}));
}

TEST(SampledSpectrum, SingleSample) {
  std::map<visual, visual> samples = {
      {static_cast<visual>(1.0), static_cast<visual>(2.0)}};
  ReferenceCounted<Spectrum> spectrum = MakeSampledSpectrum(samples);
  EXPECT_EQ(2.0, spectrum->Intensity(0.5));
  EXPECT_EQ(2.0, spectrum->Intensity(1.0));
  EXPECT_EQ(2.0, spectrum->Intensity(1.5));
  EXPECT_EQ(2.0, spectrum->Intensity(2.0));
  EXPECT_EQ(2.0, spectrum->Intensity(2.5));
  EXPECT_EQ(2.0, spectrum->Intensity(3.0));
  EXPECT_EQ(2.0, spectrum->Intensity(3.5));
}

TEST(SampledSpectrum, Intensity) {
  std::map<visual, visual> samples = {
      {static_cast<visual>(1.0), static_cast<visual>(2.0)},
      {static_cast<visual>(2.0), static_cast<visual>(3.0)},
      {static_cast<visual>(3.0), static_cast<visual>(2.0)}};
  ReferenceCounted<Spectrum> spectrum = MakeSampledSpectrum(samples);
  EXPECT_EQ(2.0, spectrum->Intensity(0.5));
  EXPECT_EQ(2.0, spectrum->Intensity(1.0));
  EXPECT_EQ(2.5, spectrum->Intensity(1.5));
  EXPECT_EQ(3.0, spectrum->Intensity(2.0));
  EXPECT_EQ(2.5, spectrum->Intensity(2.5));
  EXPECT_EQ(2.0, spectrum->Intensity(3.0));
  EXPECT_EQ(2.0, spectrum->Intensity(3.5));
}

}  // namespace
}  // namespace spectra
}  // namespace iris
