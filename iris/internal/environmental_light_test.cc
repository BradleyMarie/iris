#include "iris/internal/environmental_light.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/environmental_lights/mock_environmental_light.h"
#include "iris/power_matchers/mock_power_matcher.h"
#include "iris/random/mock_random.h"
#include "iris/spectra/mock_spectrum.h"
#include "iris/testing/spectral_allocator.h"
#include "iris/testing/visibility_tester.h"

namespace iris {
namespace internal {
namespace {

using ::iris::environmental_lights::MockEnvironmentalLight;
using ::iris::internal::EnvironmentalLight;
using ::iris::power_matchers::MockPowerMatcher;
using ::iris::random::MockRandom;
using ::iris::spectra::MockSpectrum;
using ::iris::testing::GetAlwaysVisibleVisibilityTester;
using ::iris::testing::GetNeverVisibleVisibilityTester;
using ::iris::testing::GetSpectralAllocator;
using ::testing::_;
using ::testing::DoAll;
using ::testing::Ref;
using ::testing::Return;
using ::testing::SetArgPointee;

TEST(EnvironmentalLightTest, EmissionFails) {
  MockEnvironmentalLight environmental_light;
  EXPECT_CALL(environmental_light, Emission(_, _, _)).WillOnce(Return(nullptr));

  EnvironmentalLight light(environmental_light);

  EXPECT_EQ(nullptr,
            light.Emission(Ray(Point(0.0, 0.0, 0.0), Vector(0.0, 0.0, 1.0)),
                           GetNeverVisibleVisibilityTester(),
                           GetSpectralAllocator()));
}

TEST(EnvironmentalLightTest, EmissionNotVisible) {
  MockSpectrum spectrum;

  MockEnvironmentalLight environmental_light;
  EXPECT_CALL(environmental_light, Emission(_, _, _))
      .WillOnce(Return(&spectrum));

  EnvironmentalLight light(environmental_light);

  EXPECT_EQ(nullptr,
            light.Emission(Ray(Point(0.0, 0.0, 0.0), Vector(0.0, 0.0, 1.0)),
                           GetNeverVisibleVisibilityTester(),
                           GetSpectralAllocator()));
}

TEST(EnvironmentalLightTest, EmissionSucceeds) {
  MockSpectrum spectrum;
  visual_t pdf;

  MockEnvironmentalLight environmental_light;
  EXPECT_CALL(environmental_light, Emission(_, _, &pdf))
      .WillOnce(DoAll(SetArgPointee<2>(1.0), Return(&spectrum)));

  EnvironmentalLight light(environmental_light);

  EXPECT_EQ(&spectrum,
            light.Emission(Ray(Point(0.0, 0.0, 0.0), Vector(0.0, 0.0, 1.0)),
                           GetAlwaysVisibleVisibilityTester(),
                           GetSpectralAllocator(), &pdf));
  EXPECT_EQ(1.0, pdf);
}

TEST(EnvironmentalLightTest, SampleFails) {
  MockEnvironmentalLight environmental_light;
  EXPECT_CALL(environmental_light, Sample(_, _)).WillOnce(Return(std::nullopt));

  EnvironmentalLight light(environmental_light);

  MockRandom random;
  EXPECT_CALL(random, DiscardGeometric(2));

  EXPECT_FALSE(
      light.Sample(HitPoint(Point(0.0, 0.0, 0.0), PositionError(0.0, 0.0, 0.0),
                            Vector(1.0, 0.0, 0.0)),
                   Sampler(random), GetNeverVisibleVisibilityTester(),
                   GetSpectralAllocator()));
}

TEST(EnvironmentalLightTest, SampleNotVisible) {
  MockSpectrum spectrum;

  MockEnvironmentalLight environmental_light;
  EXPECT_CALL(environmental_light, Sample(_, _))
      .WillOnce(Return(MockEnvironmentalLight::SampleResult{
          spectrum, Vector(1.0, 0.0, 0.0), 1.0}));

  EnvironmentalLight light(environmental_light);

  MockRandom random;
  EXPECT_CALL(random, DiscardGeometric(2));

  EXPECT_FALSE(
      light.Sample(HitPoint(Point(0.0, 0.0, 0.0), PositionError(0.0, 0.0, 0.0),
                            Vector(1.0, 0.0, 0.0)),
                   Sampler(random), GetNeverVisibleVisibilityTester(),
                   GetSpectralAllocator()));
}

TEST(EnvironmentalLightTest, SampleSucceeds) {
  MockSpectrum spectrum;

  MockEnvironmentalLight environmental_light;
  EXPECT_CALL(environmental_light, Sample(_, _))
      .WillOnce(Return(MockEnvironmentalLight::SampleResult{
          spectrum, Vector(1.0, 0.0, 0.0), 1.0}));

  EnvironmentalLight light(environmental_light);

  MockRandom random;
  EXPECT_CALL(random, DiscardGeometric(2));

  std::optional<Light::SampleResult> result =
      light.Sample(HitPoint(Point(0.0, 0.0, 0.0), PositionError(0.0, 0.0, 0.0),
                            Vector(1.0, 0.0, 0.0)),
                   Sampler(random), GetAlwaysVisibleVisibilityTester(),
                   GetSpectralAllocator());
  ASSERT_TRUE(result);
  EXPECT_EQ(&spectrum, &result->emission);
  EXPECT_EQ(Vector(1.0, 0.0, 0.0), result->to_light);
  EXPECT_EQ(1.0, result->pdf);
}

TEST(EnvironmentalLightTest, Power) {
  MockPowerMatcher power_matcher;

  MockEnvironmentalLight environmental_light;
  EXPECT_CALL(environmental_light, Power(Ref(power_matcher), 1.0))
      .WillOnce(Return(0.75));

  EnvironmentalLight light(environmental_light);
  EXPECT_EQ(0.75, light.Power(power_matcher, 1.0));
}

}  // namespace
}  // namespace internal
}  // namespace iris