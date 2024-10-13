#include "iris/internal/area_light.h"

#include <memory>

#include "googletest/include/gtest/gtest.h"
#include "iris/emissive_materials/mock_emissive_material.h"
#include "iris/geometry/mock_geometry.h"
#include "iris/power_matchers/mock_power_matcher.h"
#include "iris/random/mock_random.h"
#include "iris/spectra/mock_spectrum.h"
#include "iris/testing/spectral_allocator.h"
#include "iris/testing/visibility_tester.h"

namespace iris {
namespace internal {
namespace {

using ::iris::emissive_materials::MockEmissiveMaterial;
using ::iris::geometry::MockGeometry;
using ::iris::random::MockRandom;
using ::iris::spectra::MockSpectrum;
using ::iris::testing::GetNeverVisibleVisibilityTester;
using ::iris::testing::GetSpectralAllocator;
using ::iris::testing::ScopedSingleGeometryVisibilityTester;
using ::testing::_;
using ::testing::Invoke;
using ::testing::Ref;
using ::testing::Return;

std::unique_ptr<MockGeometry> MakeGeometry(
    const EmissiveMaterial* emissive_material) {
  std::unique_ptr<MockGeometry> geometry = std::make_unique<MockGeometry>();
  EXPECT_CALL(*geometry, GetFaces())
      .WillRepeatedly(Return(std::vector<face_t>({1u, 2u})));
  EXPECT_CALL(*geometry, Trace(_, _))
      .WillRepeatedly(Invoke([](const Ray& ray, HitAllocator& hit_allocator) {
        return &hit_allocator.Allocate(nullptr, 1.0, 0.0, 1u, 2u);
      }));
  EXPECT_CALL(*geometry, ComputeTextureCoordinates(_, _, _, _))
      .WillRepeatedly(Return(std::nullopt));
  EXPECT_CALL(*geometry, GetEmissiveMaterial(1u))
      .WillRepeatedly(Return(emissive_material));
  EXPECT_CALL(*geometry, GetEmissiveMaterial(2u))
      .WillRepeatedly(Return(nullptr));
  EXPECT_CALL(*geometry, ComputeSurfaceArea(1u, _)).WillRepeatedly(Return(2.0));
  return geometry;
}

TEST(AreaLightTest, AreaLightEmission) {
  MockSpectrum spectrum;
  MockEmissiveMaterial emissive_material;
  EXPECT_CALL(emissive_material, Evaluate(_, _)).WillOnce(Return(&spectrum));

  std::unique_ptr<MockGeometry> geometry = MakeGeometry(&emissive_material);
  EXPECT_CALL(*geometry, ComputePdfBySolidAngle(_, _, _, _))
      .WillRepeatedly(Return(1.0));
  AreaLight light(*geometry, nullptr, 1);

  ScopedSingleGeometryVisibilityTester(
      *geometry, nullptr, [&](iris::VisibilityTester& visibility_tester) {
        EXPECT_EQ(
            &spectrum,
            light.Emission(Ray(Point(0.0, 0.0, 0.0), Vector(0.0, 0.0, 1.0)),
                           visibility_tester, GetSpectralAllocator()));
      });
}

TEST(AreaLightTest, AreaLightEmissionMisses) {
  MockEmissiveMaterial emissive_material;

  std::unique_ptr<MockGeometry> geometry = MakeGeometry(&emissive_material);
  AreaLight light(*geometry, nullptr, 1);

  EXPECT_EQ(nullptr,
            light.Emission(Ray(Point(0.0, 0.0, 0.0), Vector(0.0, 0.0, 1.0)),
                           GetNeverVisibleVisibilityTester(),
                           GetSpectralAllocator()));
}

TEST(AreaLightTest, AreaLightSampleRngFails) {
  MockSpectrum spectrum;
  MockEmissiveMaterial emissive_material;

  std::unique_ptr<MockGeometry> geometry = MakeGeometry(&emissive_material);
  EXPECT_CALL(*geometry, SampleBySolidAngle(Point(0.0, 0.0, 0.0), _, _))
      .WillRepeatedly(Return(std::variant<std::monostate, Point, Vector>()));
  AreaLight light(*geometry, nullptr, 1);

  MockRandom random;
  EXPECT_CALL(random, DiscardGeometric(2));

  ScopedSingleGeometryVisibilityTester(
      *geometry, nullptr, [&](iris::VisibilityTester& visibility_tester) {
        EXPECT_FALSE(light.Sample(
            HitPoint(Point(0.0, 0.0, 0.0), PositionError(0.0, 0.0, 0.0),
                     Vector(1.0, 0.0, 0.0)),
            Sampler(random), visibility_tester, GetSpectralAllocator()));
      });
}

TEST(AreaLightTest, AreaLightSampleNotVisible) {
  MockSpectrum spectrum;
  MockEmissiveMaterial emissive_material;

  std::unique_ptr<MockGeometry> geometry = MakeGeometry(&emissive_material);
  EXPECT_CALL(*geometry, SampleBySolidAngle(Point(0.0, 0.0, 0.0), _, _))
      .WillRepeatedly(Return(Point(0.0, 0.0, 1.0)));
  AreaLight light(*geometry, nullptr, 1);

  MockRandom random;
  EXPECT_CALL(random, DiscardGeometric(2));

  EXPECT_FALSE(
      light.Sample(HitPoint(Point(0.0, 0.0, 0.0), PositionError(0.0, 0.0, 0.0),
                            Vector(1.0, 0.0, 0.0)),
                   Sampler(random), GetNeverVisibleVisibilityTester(),
                   GetSpectralAllocator()));
}

TEST(AreaLightTest, AreaLightSampleWorld) {
  MockSpectrum spectrum;
  MockEmissiveMaterial emissive_material;

  EXPECT_CALL(emissive_material, Evaluate(_, _)).WillOnce(Return(&spectrum));

  std::unique_ptr<MockGeometry> geometry = MakeGeometry(&emissive_material);
  EXPECT_CALL(*geometry, SampleBySolidAngle(Point(0.0, 0.0, 2.0), _, _))
      .WillRepeatedly(Return(Point(0.0, 0.0, 1.0)));
  EXPECT_CALL(*geometry, ComputePdfBySolidAngle(_, _, _, _))
      .WillRepeatedly(Return(1.0));
  AreaLight light(*geometry, nullptr, 1);

  MockRandom random;
  EXPECT_CALL(random, DiscardGeometric(2));

  ScopedSingleGeometryVisibilityTester(
      *geometry, nullptr, [&](iris::VisibilityTester& visibility_tester) {
        std::optional<Light::SampleResult> result = light.Sample(
            HitPoint(Point(0.0, 0.0, 2.0), PositionError(0.0, 0.0, 0.0),
                     Vector(1.0, 0.0, 0.0)),
            Sampler(random), visibility_tester, GetSpectralAllocator());
        EXPECT_TRUE(result);
        EXPECT_EQ(&spectrum, &result->emission);
        EXPECT_EQ(1.0, result->pdf);
        EXPECT_EQ(Vector(0.0, 0.0, -1.0), result->to_light);
      });
}

TEST(AreaLightTest, AreaLightSampleWithTransform) {
  MockSpectrum spectrum;
  MockEmissiveMaterial emissive_material;

  EXPECT_CALL(emissive_material, Evaluate(_, _)).WillOnce(Return(&spectrum));

  Matrix transform = Matrix::Scalar(1.0, 1.0, -1.0).value();
  std::unique_ptr<MockGeometry> geometry = MakeGeometry(&emissive_material);
  EXPECT_CALL(*geometry, SampleBySolidAngle(Point(0.0, 0.0, 2.0), _, _))
      .WillRepeatedly(Return(Point(0.0, 0.0, -1.0)));
  EXPECT_CALL(*geometry, ComputePdfBySolidAngle(_, _, _, _))
      .WillRepeatedly(Return(1.0));
  AreaLight light(*geometry, &transform, 1);

  MockRandom random;
  EXPECT_CALL(random, DiscardGeometric(2));

  ScopedSingleGeometryVisibilityTester(
      *geometry, &transform, [&](iris::VisibilityTester& visibility_tester) {
        std::optional<Light::SampleResult> result = light.Sample(
            HitPoint(Point(0.0, 0.0, -2.0), PositionError(0.0, 0.0, 0.0),
                     Vector(1.0, 0.0, 0.0)),
            Sampler(random), visibility_tester, GetSpectralAllocator());
        EXPECT_TRUE(result);
        EXPECT_EQ(&spectrum, &result->emission);
        EXPECT_EQ(1.0, result->pdf);
        EXPECT_EQ(Vector(0.0, 0.0, 1.0), result->to_light);
      });
}

TEST(AreaLightTest, AreaLightSampleVector) {
  MockSpectrum spectrum;
  MockEmissiveMaterial emissive_material;

  EXPECT_CALL(emissive_material, Evaluate(_, _)).WillOnce(Return(&spectrum));

  std::unique_ptr<MockGeometry> geometry = MakeGeometry(&emissive_material);
  EXPECT_CALL(*geometry, SampleBySolidAngle(Point(0.0, 0.0, 2.0), _, _))
      .WillRepeatedly(Return(Vector(0.0, 0.0, -1.0)));
  EXPECT_CALL(*geometry, ComputePdfBySolidAngle(_, _, _, _))
      .WillRepeatedly(Return(1.0));
  AreaLight light(*geometry, nullptr, 1);

  MockRandom random;
  EXPECT_CALL(random, DiscardGeometric(2));

  ScopedSingleGeometryVisibilityTester(
      *geometry, nullptr, [&](iris::VisibilityTester& visibility_tester) {
        std::optional<Light::SampleResult> result = light.Sample(
            HitPoint(Point(0.0, 0.0, 2.0), PositionError(0.0, 0.0, 0.0),
                     Vector(1.0, 0.0, 0.0)),
            Sampler(random), visibility_tester, GetSpectralAllocator());
        EXPECT_TRUE(result);
        EXPECT_EQ(&spectrum, &result->emission);
        EXPECT_EQ(1.0, result->pdf);
        EXPECT_EQ(Vector(0.0, 0.0, -1.0), result->to_light);
      });
}

TEST(AreaLightTest, Power) {
  power_matchers::MockPowerMatcher power_matcher;

  MockEmissiveMaterial emissive_material;
  EXPECT_CALL(emissive_material, UnitPower(Ref(power_matcher)))
      .WillOnce(Return(3.0));

  std::unique_ptr<MockGeometry> geometry = MakeGeometry(&emissive_material);
  AreaLight light(*geometry, nullptr, 1);

  EXPECT_EQ(6.0, light.Power(power_matcher, 1.0));
}

}  // namespace
}  // namespace internal
}  // namespace iris