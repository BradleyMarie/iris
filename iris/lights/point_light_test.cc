#include "iris/lights/point_light.h"

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
using ::testing::Ref;
using ::testing::Return;

TEST(PointLightTest, Null) {
  EXPECT_FALSE(
      MakePointLight(Point(0.0, 0.0, 0.0), ReferenceCounted<Spectrum>()));
}

TEST(PointLightTest, SampleHits) {
  MockRandom random;
  EXPECT_CALL(random, DiscardGeometric(2));

  ReferenceCounted<MockSpectrum> spectrum =
      MakeReferenceCounted<MockSpectrum>();

  ReferenceCounted<Light> light =
      MakePointLight(Point(0.0, 0.0, 0.0), spectrum);
  std::optional<Light::SampleResult> result = light->Sample(
      HitPoint(Point(0.0, 0.0, -1.0), PositionError(0.0, 0.0, 0.0),
               Vector(1.0, 0.0, 0.0)),
      Sampler(random), GetAlwaysVisibleVisibilityTester(),
      GetSpectralAllocator());
  EXPECT_TRUE(result);
  EXPECT_EQ(spectrum.Get(), &result->emission);
  EXPECT_FALSE(result->pdf);
  EXPECT_EQ(Vector(0.0, 0.0, 1.0), result->to_light);
}

TEST(PointLightTest, SampleMisses) {
  MockRandom random;
  EXPECT_CALL(random, DiscardGeometric(2));

  ReferenceCounted<MockSpectrum> spectrum =
      MakeReferenceCounted<MockSpectrum>();

  ReferenceCounted<Light> light =
      MakePointLight(Point(0.0, 0.0, 0.0), spectrum);
  EXPECT_FALSE(light->Sample(
      HitPoint(Point(0.0, 0.0, -1.0), PositionError(0.0, 0.0, 0.0),
               Vector(1.0, 0.0, 0.0)),
      Sampler(random), GetNeverVisibleVisibilityTester(),
      GetSpectralAllocator()));
}

TEST(PointLightTest, Emission) {
  ReferenceCounted<MockSpectrum> spectrum =
      MakeReferenceCounted<MockSpectrum>();

  ReferenceCounted<Light> light =
      MakePointLight(Point(0.0, 0.0, 0.0), spectrum);

  visual_t pdf;
  EXPECT_EQ(nullptr,
            light->Emission(Ray(Point(0.0, 0.0, 0.0), Vector(0.0, 0.0, 1.0)),
                            GetAlwaysVisibleVisibilityTester(),
                            GetSpectralAllocator(), &pdf));
}

TEST(PointLightTest, Power) {
  ReferenceCounted<MockSpectrum> spectrum =
      MakeReferenceCounted<MockSpectrum>();

  ReferenceCounted<Light> light =
      MakePointLight(Point(0.0, 0.0, 0.0), spectrum);

  MockPowerMatcher power_matcher;
  EXPECT_CALL(power_matcher, Match(Ref(*spectrum))).WillOnce(Return(1.0));
  EXPECT_NEAR(12.566371, light->Power(power_matcher, 2.0), 0.01);
}

}  // namespace
}  // namespace lights
}  // namespace iris
