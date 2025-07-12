#include "iris/spectra/blackbody_spectrum.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/float.h"
#include "iris/reference_counted.h"
#include "iris/spectrum.h"

namespace iris {
namespace spectra {
namespace {

TEST(BlackbodySpectrum, Null) {
  EXPECT_FALSE(MakeBlackbodySpectrum(0.0));
  EXPECT_FALSE(MakeBlackbodySpectrum(-1.0));
}

TEST(BlackbodySpectrum, Intensity) {
  EXPECT_NEAR(MakeBlackbodySpectrum(6000.0)->Intensity(483.0), 3.1849e13, 1e11);
  EXPECT_NEAR(MakeBlackbodySpectrum(6000.0)->Intensity(600.0), 2.86772e13,
              1e11);
  EXPECT_NEAR(MakeBlackbodySpectrum(3700.0)->Intensity(500.0), 1.59845e12,
              1e10);
  EXPECT_NEAR(MakeBlackbodySpectrum(4500.0)->Intensity(600.0), 7.46497e12,
              1e10);
}

TEST(ScaledBlackbodySpectrum, Null) {
  EXPECT_FALSE(MakeScaledBlackbodySpectrum(0.0, 1.0));
  EXPECT_FALSE(MakeScaledBlackbodySpectrum(-1.0, 1.0));
  EXPECT_FALSE(MakeScaledBlackbodySpectrum(65000.0, 0.0));
  EXPECT_FALSE(MakeScaledBlackbodySpectrum(65000.0, -1.0));
}

TEST(ScaledBlackbodySpectrum, Intensity) {
  EXPECT_NEAR(MakeScaledBlackbodySpectrum(6000.0, 2.0)->Intensity(482.9619925),
              2.0, 0.01);
}

}  // namespace
}  // namespace spectra
}  // namespace iris
