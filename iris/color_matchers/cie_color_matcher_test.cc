#include "iris/color_matchers/cie_color_matcher.h"

#include <memory>

#include "googletest/include/gtest/gtest.h"
#include "iris/color_matcher.h"
#include "iris/spectra/mock_spectrum.h"

namespace iris {
namespace color_matchers {
namespace {

using ::iris::spectra::MockSpectrum;
using ::testing::_;
using ::testing::Return;

TEST(CieColorMatcherTest, ColorSpace) {
  std::unique_ptr<ColorMatcher> matcher = MakeCieColorMatcher();
  EXPECT_EQ(Color::CIE_XYZ, matcher->ColorSpace());
}

TEST(CieColorMatcherTest, MatchSpectrum) {
  MockSpectrum spectrum;
  EXPECT_CALL(spectrum, Intensity(_)).WillRepeatedly(Return(1.0));

  std::unique_ptr<ColorMatcher> matcher = MakeCieColorMatcher();
  std::array<visual_t, 3> actual = matcher->Match(spectrum);
  EXPECT_NEAR(106.865, actual[0], 0.001);
  EXPECT_NEAR(106.857, actual[1], 0.001);
  EXPECT_NEAR(106.892, actual[2], 0.001);
}

}  // namespace
}  // namespace color_matchers
}  // namespace iris
