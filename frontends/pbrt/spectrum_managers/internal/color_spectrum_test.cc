#include "frontends/pbrt/spectrum_managers/internal/color_spectrum.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/color.h"
#include "iris/float.h"
#include "iris/reference_counted.h"
#include "iris/spectrum.h"

namespace iris {
namespace pbrt_frontend {
namespace spectrum_managers {
namespace internal {
namespace {

TEST(ColorSpectrum, Null) {
  EXPECT_FALSE(MakeColorSpectrum(0.0, 0.0, 0.0, Color::LINEAR_SRGB));
}

TEST(ColorSpectrum, FromRgb) {
  ReferenceCounted<Spectrum> spectrum =
      MakeColorSpectrum(0.25, 0.5, 0.75, Color::LINEAR_SRGB);
  ASSERT_TRUE(spectrum);
  EXPECT_EQ(0.25, spectrum->Intensity(0.5));
  EXPECT_EQ(0.50, spectrum->Intensity(1.5));
  EXPECT_EQ(0.75, spectrum->Intensity(2.5));
}

}  // namespace
}  // namespace internal
}  // namespace spectrum_managers
}  // namespace pbrt_frontend
}  // namespace iris
