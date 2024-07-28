#include "iris/internal/area_light.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/emissive_materials/mock_emissive_material.h"
#include "iris/geometry/mock_geometry.h"
#include "iris/power_matchers/mock_power_matcher.h"
#include "iris/random/mock_random.h"
#include "iris/spectra/mock_spectrum.h"
#include "iris/testing/spectral_allocator.h"
#include "iris/testing/visibility_tester.h"

std::unique_ptr<iris::geometry::MockGeometry> MakeGeometry(
    const iris::EmissiveMaterial* emissive_material) {
  auto geometry = std::make_unique<iris::geometry::MockGeometry>();
  EXPECT_CALL(*geometry, GetFaces())
      .WillRepeatedly(testing::Return(std::vector<iris::face_t>({1u, 2u})));
  EXPECT_CALL(*geometry, Trace(testing::_, testing::_))
      .WillRepeatedly(testing::Invoke(
          [](const iris::Ray& ray, iris::HitAllocator& hit_allocator) {
            return &hit_allocator.Allocate(
                nullptr, 1.0, static_cast<iris::geometric_t>(0.0), 1u, 2u);
          }));
  EXPECT_CALL(*geometry, ComputeTextureCoordinates(testing::_, testing::_,
                                                   testing::_, testing::_))
      .WillRepeatedly(testing::Return(std::nullopt));
  EXPECT_CALL(*geometry, GetEmissiveMaterial(1u))
      .WillRepeatedly(testing::Return(emissive_material));
  EXPECT_CALL(*geometry, GetEmissiveMaterial(2u))
      .WillRepeatedly(testing::Return(nullptr));
  EXPECT_CALL(*geometry, ComputeSurfaceArea(1u, testing::_))
      .WillRepeatedly(testing::Return(2.0));
  return geometry;
}

TEST(AreaLightTest, AreaLightEmission) {
  iris::spectra::MockSpectrum spectrum;
  iris::emissive_materials::MockEmissiveMaterial emissive_material;
  EXPECT_CALL(emissive_material, Evaluate(testing::_, testing::_))
      .WillOnce(testing::Return(&spectrum));

  auto geometry = MakeGeometry(&emissive_material);
  EXPECT_CALL(*geometry, ComputePdfBySolidAngle(testing::_, testing::_,
                                                testing::_, testing::_))
      .WillRepeatedly(testing::Return(static_cast<iris::visual_t>(1.0)));
  iris::internal::AreaLight light(*geometry, nullptr, 1);

  iris::testing::ScopedSingleGeometryVisibilityTester(
      *geometry, nullptr, [&](auto& visibility_tester) {
        EXPECT_EQ(&spectrum,
                  light.Emission(iris::Ray(iris::Point(0.0, 0.0, 0.0),
                                           iris::Vector(0.0, 0.0, 1.0)),
                                 visibility_tester,
                                 iris::testing::GetSpectralAllocator()));
      });
}

TEST(AreaLightTest, AreaLightEmissionMisses) {
  iris::emissive_materials::MockEmissiveMaterial emissive_material;

  auto geometry = MakeGeometry(&emissive_material);
  iris::internal::AreaLight light(*geometry, nullptr, 1);

  EXPECT_EQ(nullptr,
            light.Emission(iris::Ray(iris::Point(0.0, 0.0, 0.0),
                                     iris::Vector(0.0, 0.0, 1.0)),
                           iris::testing::GetNeverVisibleVisibilityTester(),
                           iris::testing::GetSpectralAllocator()));
}

TEST(AreaLightTest, AreaLightSampleRngFails) {
  iris::spectra::MockSpectrum spectrum;
  iris::emissive_materials::MockEmissiveMaterial emissive_material;

  auto geometry = MakeGeometry(&emissive_material);
  EXPECT_CALL(*geometry, SampleBySolidAngle(iris::Point(0.0, 0.0, 0.0),
                                            testing::_, testing::_))
      .WillRepeatedly(testing::Return(
          std::variant<std::monostate, iris::Point, iris::Vector>()));
  iris::internal::AreaLight light(*geometry, nullptr, 1);

  iris::random::MockRandom random;
  EXPECT_CALL(random, DiscardGeometric(2));

  iris::testing::ScopedSingleGeometryVisibilityTester(
      *geometry, nullptr, [&](auto& visibility_tester) {
        EXPECT_FALSE(
            light.Sample(iris::HitPoint(iris::Point(0.0, 0.0, 0.0),
                                        iris::PositionError(0.0, 0.0, 0.0),
                                        iris::Vector(1.0, 0.0, 0.0)),
                         iris::Sampler(random), visibility_tester,
                         iris::testing::GetSpectralAllocator()));
      });
}

TEST(AreaLightTest, AreaLightSampleNotVisible) {
  iris::spectra::MockSpectrum spectrum;
  iris::emissive_materials::MockEmissiveMaterial emissive_material;

  auto geometry = MakeGeometry(&emissive_material);
  EXPECT_CALL(*geometry, SampleBySolidAngle(iris::Point(0.0, 0.0, 0.0),
                                            testing::_, testing::_))
      .WillRepeatedly(testing::Return(iris::Point(0.0, 0.0, 1.0)));
  iris::internal::AreaLight light(*geometry, nullptr, 1);

  iris::random::MockRandom random;
  EXPECT_CALL(random, DiscardGeometric(2));

  EXPECT_FALSE(light.Sample(iris::HitPoint(iris::Point(0.0, 0.0, 0.0),
                                           iris::PositionError(0.0, 0.0, 0.0),
                                           iris::Vector(1.0, 0.0, 0.0)),
                            iris::Sampler(random),
                            iris::testing::GetNeverVisibleVisibilityTester(),
                            iris::testing::GetSpectralAllocator()));
}

TEST(AreaLightTest, AreaLightSampleWorld) {
  iris::spectra::MockSpectrum spectrum;
  iris::emissive_materials::MockEmissiveMaterial emissive_material;

  EXPECT_CALL(emissive_material, Evaluate(testing::_, testing::_))
      .WillOnce(testing::Return(&spectrum));

  auto geometry = MakeGeometry(&emissive_material);
  EXPECT_CALL(*geometry, SampleBySolidAngle(iris::Point(0.0, 0.0, 2.0),
                                            testing::_, testing::_))
      .WillRepeatedly(testing::Return(iris::Point(0.0, 0.0, 1.0)));
  EXPECT_CALL(*geometry, ComputePdfBySolidAngle(testing::_, testing::_,
                                                testing::_, testing::_))
      .WillRepeatedly(testing::Return(static_cast<iris::visual_t>(1.0)));
  iris::internal::AreaLight light(*geometry, nullptr, 1);

  iris::random::MockRandom random;
  EXPECT_CALL(random, DiscardGeometric(2));

  iris::testing::ScopedSingleGeometryVisibilityTester(
      *geometry, nullptr, [&](auto& visibility_tester) {
        auto result =
            light.Sample(iris::HitPoint(iris::Point(0.0, 0.0, 2.0),
                                        iris::PositionError(0.0, 0.0, 0.0),
                                        iris::Vector(1.0, 0.0, 0.0)),
                         iris::Sampler(random), visibility_tester,
                         iris::testing::GetSpectralAllocator());
        EXPECT_TRUE(result);
        EXPECT_EQ(&spectrum, &result->emission);
        EXPECT_EQ(1.0, result->pdf);
        EXPECT_EQ(iris::Vector(0.0, 0.0, -1.0), result->to_light);
      });
}

TEST(AreaLightTest, AreaLightSampleWithTransform) {
  iris::spectra::MockSpectrum spectrum;
  iris::emissive_materials::MockEmissiveMaterial emissive_material;

  EXPECT_CALL(emissive_material, Evaluate(testing::_, testing::_))
      .WillOnce(testing::Return(&spectrum));

  auto transform = iris::Matrix::Scalar(1.0, 1.0, -1.0).value();
  auto geometry = MakeGeometry(&emissive_material);
  EXPECT_CALL(*geometry, SampleBySolidAngle(iris::Point(0.0, 0.0, 2.0),
                                            testing::_, testing::_))
      .WillRepeatedly(testing::Return(iris::Point(0.0, 0.0, -1.0)));
  EXPECT_CALL(*geometry, ComputePdfBySolidAngle(testing::_, testing::_,
                                                testing::_, testing::_))
      .WillRepeatedly(testing::Return(static_cast<iris::visual_t>(1.0)));
  iris::internal::AreaLight light(*geometry, &transform, 1);

  iris::random::MockRandom random;
  EXPECT_CALL(random, DiscardGeometric(2));

  iris::testing::ScopedSingleGeometryVisibilityTester(
      *geometry, &transform, [&](auto& visibility_tester) {
        auto result =
            light.Sample(iris::HitPoint(iris::Point(0.0, 0.0, -2.0),
                                        iris::PositionError(0.0, 0.0, 0.0),
                                        iris::Vector(1.0, 0.0, 0.0)),
                         iris::Sampler(random), visibility_tester,
                         iris::testing::GetSpectralAllocator());
        EXPECT_TRUE(result);
        EXPECT_EQ(&spectrum, &result->emission);
        EXPECT_EQ(1.0, result->pdf);
        EXPECT_EQ(iris::Vector(0.0, 0.0, 1.0), result->to_light);
      });
}

TEST(AreaLightTest, AreaLightSampleVector) {
  iris::spectra::MockSpectrum spectrum;
  iris::emissive_materials::MockEmissiveMaterial emissive_material;

  EXPECT_CALL(emissive_material, Evaluate(testing::_, testing::_))
      .WillOnce(testing::Return(&spectrum));

  auto geometry = MakeGeometry(&emissive_material);
  EXPECT_CALL(*geometry, SampleBySolidAngle(iris::Point(0.0, 0.0, 2.0),
                                            testing::_, testing::_))
      .WillRepeatedly(testing::Return(iris::Vector(0.0, 0.0, -1.0)));
  EXPECT_CALL(*geometry, ComputePdfBySolidAngle(testing::_, testing::_,
                                                testing::_, testing::_))
      .WillRepeatedly(testing::Return(static_cast<iris::visual_t>(1.0)));
  iris::internal::AreaLight light(*geometry, nullptr, 1);

  iris::random::MockRandom random;
  EXPECT_CALL(random, DiscardGeometric(2));

  iris::testing::ScopedSingleGeometryVisibilityTester(
      *geometry, nullptr, [&](auto& visibility_tester) {
        auto result =
            light.Sample(iris::HitPoint(iris::Point(0.0, 0.0, 2.0),
                                        iris::PositionError(0.0, 0.0, 0.0),
                                        iris::Vector(1.0, 0.0, 0.0)),
                         iris::Sampler(random), visibility_tester,
                         iris::testing::GetSpectralAllocator());
        EXPECT_TRUE(result);
        EXPECT_EQ(&spectrum, &result->emission);
        EXPECT_EQ(1.0, result->pdf);
        EXPECT_EQ(iris::Vector(0.0, 0.0, -1.0), result->to_light);
      });
}

TEST(AreaLightTest, Power) {
  iris::power_matchers::MockPowerMatcher power_matcher;

  iris::emissive_materials::MockEmissiveMaterial emissive_material;
  EXPECT_CALL(emissive_material, UnitPower(testing::Ref(power_matcher)))
      .WillOnce(testing::Return(3.0));

  auto geometry = MakeGeometry(&emissive_material);
  iris::internal::AreaLight light(*geometry, nullptr, 1);

  EXPECT_EQ(6.0, light.Power(power_matcher));
}