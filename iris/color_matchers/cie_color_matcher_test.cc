#include "iris/color_matchers/cie_color_matcher.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/reflectors/mock_reflector.h"
#include "iris/spectra/mock_spectrum.h"

TEST(CieColorMatcherTest, ColorSpace) {
  iris::color_matchers::CieColorMatcher matcher;
  EXPECT_EQ(iris::Color::CIE_XYZ, matcher.ColorSpace());
}

TEST(CieColorMatcherTest, MatchSpectrum) {
  iris::spectra::MockSpectrum spectrum;
  EXPECT_CALL(spectrum, Intensity(testing::_))
      .WillRepeatedly(testing::Return(1.0));

  iris::color_matchers::CieColorMatcher matcher;
  auto actual = matcher.Match(spectrum);
  EXPECT_NEAR(106.865, actual[0], 0.001);
  EXPECT_NEAR(106.857, actual[1], 0.001);
  EXPECT_NEAR(106.892, actual[2], 0.001);
}

TEST(CieColorMatcherTest, MatchReflector) {
  iris::reflectors::MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(testing::_))
      .WillRepeatedly(testing::Return(1.0));

  iris::color_matchers::CieColorMatcher matcher;
  auto actual = matcher.Match(reflector);
  EXPECT_NEAR(1.0, actual[0], 0.001);
  EXPECT_NEAR(1.0, actual[1], 0.001);
  EXPECT_NEAR(1.0, actual[2], 0.001);
}