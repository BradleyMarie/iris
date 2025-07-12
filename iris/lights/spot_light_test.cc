#include "iris/lights/spot_light.h"

#include <limits>
#include <memory>
#include <optional>

#include "googlemock/include/gmock/gmock.h"
#include "googletest/include/gtest/gtest.h"
#include "iris/float.h"
#include "iris/hit_point.h"
#include "iris/light.h"
#include "iris/point.h"
#include "iris/power_matchers/mock_power_matcher.h"
#include "iris/random/mock_random.h"
#include "iris/ray.h"
#include "iris/reference_counted.h"
#include "iris/sampler.h"
#include "iris/spectra/mock_spectrum.h"
#include "iris/spectrum.h"
#include "iris/testing/spectral_allocator.h"
#include "iris/testing/visibility_tester.h"

namespace iris {
namespace lights {
namespace {

using ::iris::power_matchers::MockPowerMatcher;
using ::iris::random::MockRandom;
using ::iris::spectra::MockSpectrum;
using ::iris::testing::GetAlwaysVisibleVisibilityTester;
using ::iris::testing::GetNeverVisibleVisibilityTester;
using ::iris::testing::GetSpectralAllocator;
using ::testing::_;
using ::testing::Ref;
using ::testing::Return;

TEST(SpotLightTest, Null) {
  EXPECT_FALSE(MakeSpotLight(Point(0.0, 0.0, 0.0), Vector(0.0, 0.0, 1.0), 30.0,
                             25.0, ReferenceCounted<Spectrum>()));
  EXPECT_FALSE(MakeSpotLight(Point(0.0, 0.0, 0.0), Vector(0.0, 0.0, 1.0), 0.0,
                             25.0, MakeReferenceCounted<MockSpectrum>()));
}

TEST(SpotLightTest, SampleHitsNoFalloff) {
  MockRandom random;
  EXPECT_CALL(random, DiscardGeometric(2));

  ReferenceCounted<MockSpectrum> spectrum =
      MakeReferenceCounted<MockSpectrum>();

  ReferenceCounted<Light> light = MakeSpotLight(
      Point(0.0, 0.0, 0.0), Vector(0.0, 0.0, 1.0), 360.0, 360.0, spectrum);

  std::optional<Light::SampleResult> result = light->Sample(
      HitPoint(Point(0.0, 0.0, -1.0), PositionError(0.0, 0.0, 0.0),
               Vector(1.0, 0.0, 0.0)),
      Sampler(random), GetAlwaysVisibleVisibilityTester(),
      GetSpectralAllocator());
  ASSERT_TRUE(result);
  EXPECT_EQ(spectrum.Get(), &result->emission);
  EXPECT_FALSE(result->pdf);
  EXPECT_EQ(Vector(0.0, 0.0, 1.0), result->to_light);
}

TEST(SpotLightTest, SampleHitsWithFalloff) {
  MockRandom random;
  EXPECT_CALL(random, DiscardGeometric(2));

  ReferenceCounted<MockSpectrum> spectrum =
      MakeReferenceCounted<MockSpectrum>();
  EXPECT_CALL(*spectrum, Intensity(_)).WillOnce(Return(1.0));

  ReferenceCounted<Light> light = MakeSpotLight(
      Point(0.0, 0.0, 0.0), Vector(0.0, 0.0, 1.0), 360.0, 0.0, spectrum);

  std::optional<Light::SampleResult> result =
      light->Sample(HitPoint(Point(0.0, 1.0, 0.0), PositionError(0.0, 0.0, 0.0),
                             Vector(1.0, 0.0, 0.0)),
                    Sampler(random), GetAlwaysVisibleVisibilityTester(),
                    GetSpectralAllocator());
  ASSERT_TRUE(result);
  EXPECT_EQ(0.0625, result->emission.Intensity(1.0));
  EXPECT_FALSE(result->pdf);
  EXPECT_EQ(Vector(0.0, -1.0, 0.0), result->to_light);
}

TEST(SpotLightTest, SampleHitsPastCutoff) {
  MockRandom random;
  EXPECT_CALL(random, DiscardGeometric(2));

  ReferenceCounted<MockSpectrum> spectrum =
      MakeReferenceCounted<MockSpectrum>();

  ReferenceCounted<Light> light = MakeSpotLight(
      Point(0.0, 0.0, 0.0), Vector(0.0, 0.0, 1.0), 30.0, 5.0, spectrum);

  std::optional<Light::SampleResult> result =
      light->Sample(HitPoint(Point(0.0, 1.0, 0.0), PositionError(0.0, 0.0, 0.0),
                             Vector(1.0, 0.0, 0.0)),
                    Sampler(random), GetAlwaysVisibleVisibilityTester(),
                    GetSpectralAllocator());
  EXPECT_FALSE(result);
}

TEST(SpotLightTest, SampleMisses) {
  MockRandom random;
  EXPECT_CALL(random, DiscardGeometric(2));

  ReferenceCounted<MockSpectrum> spectrum =
      MakeReferenceCounted<MockSpectrum>();

  ReferenceCounted<Light> light = MakeSpotLight(
      Point(0.0, 0.0, 0.0), Vector(0.0, 0.0, 1.0), 360.0, 360.0, spectrum);
  EXPECT_FALSE(
      light->Sample(HitPoint(Point(0.0, 0.0, 2.0), PositionError(0.0, 0.0, 0.0),
                             Vector(1.0, 0.0, 0.0)),
                    Sampler(random), GetNeverVisibleVisibilityTester(),
                    GetSpectralAllocator()));
}

TEST(SpotLightTest, Emission) {
  ReferenceCounted<MockSpectrum> spectrum =
      MakeReferenceCounted<MockSpectrum>();

  ReferenceCounted<Light> light = MakeSpotLight(
      Point(0.0, 0.0, 0.0), Vector(0.0, 0.0, 1.0), 360.0, 360.0, spectrum);

  visual_t pdf;
  EXPECT_EQ(nullptr,
            light->Emission(Ray(Point(0.0, 0.0, 0.0), Vector(0.0, 0.0, 1.0)),
                            GetAlwaysVisibleVisibilityTester(),
                            GetSpectralAllocator(), &pdf));
}

TEST(SpotLightTest, Power) {
  ReferenceCounted<MockSpectrum> spectrum =
      MakeReferenceCounted<MockSpectrum>();

  MockPowerMatcher power_matcher;
  EXPECT_CALL(power_matcher, Match(Ref(*spectrum))).WillRepeatedly(Return(1.0));
  EXPECT_NEAR(12.566371,
              MakeSpotLight(Point(0.0, 0.0, 0.0), Vector(0.0, 0.0, 1.0), 360.0,
                            360.0, spectrum)
                  ->Power(power_matcher, 2.0),
              0.01);
  EXPECT_NEAR(6.2831855,
              MakeSpotLight(Point(0.0, 0.0, 0.0), Vector(0.0, 0.0, 1.0), 360.0,
                            0.0, spectrum)
                  ->Power(power_matcher, 2.0),
              0.01);
}

}  // namespace
}  // namespace lights
}  // namespace iris
