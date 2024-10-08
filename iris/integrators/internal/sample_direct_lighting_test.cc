#include "iris/integrators/internal/sample_direct_lighting.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/bxdfs/mock_bxdf.h"
#include "iris/lights/mock_light.h"
#include "iris/random/mock_random.h"
#include "iris/reflectors/mock_reflector.h"
#include "iris/spectra/mock_spectrum.h"
#include "iris/testing/light_sampler.h"
#include "iris/testing/spectral_allocator.h"
#include "iris/testing/visibility_tester.h"

using iris::integrators::internal::EstimateDirectLighting;
using iris::integrators::internal::SampleDirectLighting;
using iris::integrators::internal::internal::DeltaLight;
using iris::integrators::internal::internal::FromBsdfSample;
using iris::integrators::internal::internal::FromLightSample;
using iris::integrators::internal::internal::PowerHeuristic;

TEST(PowerHeuristict, Test) {
  EXPECT_NEAR(0.5, PowerHeuristic(1.0, 1.0), 0.001);
  EXPECT_NEAR(0.2, PowerHeuristic(1.0, 2.0), 0.001);
}

TEST(DeltaLight, NoReflectance) {
  iris::spectra::MockSpectrum spectrum;
  iris::Vector to_light(1.0, 0.0, 0.0);

  iris::Light::SampleResult light_sample{spectrum, to_light, std::nullopt};

  iris::bxdfs::MockBxdf bxdf;
  EXPECT_CALL(bxdf, IsDiffuse(testing::IsNull()))
      .WillRepeatedly(testing::Return(true));
  EXPECT_CALL(bxdf, Pdf(testing::_, testing::_, testing::_, testing::_))
      .WillOnce(testing::Return(1.0));

  iris::RayTracer::RayTracer::SurfaceIntersection intersection{
      iris::Bsdf(bxdf, to_light, to_light),
      iris::HitPoint(iris::Point(0.0, 0.0, 0.0),
                     iris::PositionError(0.0, 0.0, 0.0), to_light),
      std::nullopt, to_light, to_light};

  EXPECT_EQ(
      nullptr,
      DeltaLight(light_sample, iris::Ray(iris::Point(0.0, 0.0, 0.0), to_light),
                 intersection, iris::testing::GetSpectralAllocator()));
}

TEST(DeltaLight, WithReflectance) {
  iris::Ray trace_ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(0.0, 0.0, 1.0));

  iris::spectra::MockSpectrum spectrum;
  EXPECT_CALL(spectrum, Intensity(testing::_))
      .WillOnce(testing::Return(static_cast<iris::visual_t>(1.0)));

  iris::Vector to_light(0.0, 0.866025, 0.5);

  iris::Light::SampleResult light_sample{spectrum, to_light, std::nullopt};

  iris::reflectors::MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(1.0))
      .WillOnce(testing::Return(static_cast<iris::visual_t>(0.25)));

  iris::bxdfs::MockBxdf bxdf;
  EXPECT_CALL(bxdf, IsDiffuse(testing::IsNull()))
      .WillRepeatedly(testing::Return(true));
  EXPECT_CALL(bxdf, Pdf(testing::_, testing::_, testing::_, testing::_))
      .WillOnce(testing::Return(static_cast<iris::visual_t>(100.0)));
  EXPECT_CALL(bxdf, Reflectance(testing::_, testing::_, testing::_, testing::_))
      .WillOnce(testing::Return(&reflector));

  iris::Vector surface_normal(0.0, 0.0, 1.0);
  iris::RayTracer::RayTracer::SurfaceIntersection intersection{
      iris::Bsdf(bxdf, surface_normal, surface_normal),
      iris::HitPoint(trace_ray.Endpoint(1.0),
                     iris::PositionError(0.0, 0.0, 0.0), surface_normal),
      std::nullopt, surface_normal, surface_normal};

  auto* result = DeltaLight(light_sample, trace_ray, intersection,
                            iris::testing::GetSpectralAllocator());
  ASSERT_NE(nullptr, result);
  EXPECT_NEAR(0.125, result->Intensity(1.0), 0.001);
}

TEST(FromLightSample, NoReflectance) {
  iris::Ray trace_ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(0.0, 0.0, 1.0));

  iris::spectra::MockSpectrum spectrum;

  iris::Vector to_light(0.0, 0.866025, 0.5);

  iris::Light::SampleResult light_sample{spectrum, to_light, std::nullopt};

  iris::bxdfs::MockBxdf bxdf;
  EXPECT_CALL(bxdf, IsDiffuse(testing::IsNull()))
      .WillRepeatedly(testing::Return(true));
  EXPECT_CALL(bxdf, Pdf(testing::_, testing::_, testing::_, testing::_))
      .WillOnce(testing::Return(static_cast<iris::visual_t>(100.0)));
  EXPECT_CALL(bxdf, Reflectance(testing::_, testing::_, testing::_, testing::_))
      .WillOnce(testing::Return(nullptr));

  iris::Vector surface_normal(0.0, 0.0, 1.0);
  iris::RayTracer::RayTracer::SurfaceIntersection intersection{
      iris::Bsdf(bxdf, surface_normal, surface_normal),
      iris::HitPoint(trace_ray.Endpoint(1.0),
                     iris::PositionError(0.0, 0.0, 0.0), surface_normal),
      std::nullopt, surface_normal, surface_normal};

  auto* result =
      FromLightSample(light_sample, trace_ray, intersection,
                      iris::testing::GetAlwaysVisibleVisibilityTester(),
                      iris::testing::GetSpectralAllocator());
  EXPECT_EQ(nullptr, result);
}

TEST(FromLightSample, WithReflectance) {
  iris::Ray trace_ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(0.0, 0.0, 1.0));

  iris::spectra::MockSpectrum spectrum;
  EXPECT_CALL(spectrum, Intensity(testing::_))
      .WillOnce(testing::Return(static_cast<iris::visual_t>(1.0)));

  iris::Vector to_light(0.0, 0.866025, 0.5);

  iris::Light::SampleResult light_sample{spectrum, to_light,
                                         static_cast<iris::visual_t>(1.0)};

  iris::reflectors::MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(1.0))
      .WillOnce(testing::Return(static_cast<iris::visual_t>(0.5)));

  iris::bxdfs::MockBxdf bxdf;
  EXPECT_CALL(bxdf, IsDiffuse(testing::IsNull()))
      .WillRepeatedly(testing::Return(true));
  EXPECT_CALL(bxdf, Pdf(testing::_, testing::_, testing::_, testing::_))
      .WillOnce(testing::Return(static_cast<iris::visual_t>(2.0)));
  EXPECT_CALL(bxdf, Reflectance(testing::_, testing::_, testing::_, testing::_))
      .WillOnce(testing::Return(&reflector));

  iris::Vector surface_normal(0.0, 0.0, 1.0);
  iris::RayTracer::RayTracer::SurfaceIntersection intersection{
      iris::Bsdf(bxdf, surface_normal, surface_normal),
      iris::HitPoint(trace_ray.Endpoint(1.0),
                     iris::PositionError(0.0, 0.0, 0.0), surface_normal),
      std::nullopt, surface_normal, surface_normal};

  auto* result =
      FromLightSample(light_sample, trace_ray, intersection,
                      iris::testing::GetAlwaysVisibleVisibilityTester(),
                      iris::testing::GetSpectralAllocator());
  ASSERT_NE(nullptr, result);
  EXPECT_NEAR(0.05, result->Intensity(1.0), 0.001);
}

TEST(FromBsdfSample, NoEmission) {
  iris::Ray trace_ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(0.0, 0.0, 1.0));

  iris::Vector to_light(0.0, 0.866025, 0.5);

  iris::reflectors::MockReflector reflector;

  iris::Bsdf::SampleResult bsdf_sample{reflector, to_light, std::nullopt,
                                       static_cast<iris::visual_t>(1.0)};

  iris::bxdfs::MockBxdf bxdf;
  EXPECT_CALL(bxdf, IsDiffuse(testing::IsNull()))
      .WillRepeatedly(testing::Return(true));

  iris::Vector surface_normal(0.0, 0.0, 1.0);
  iris::RayTracer::RayTracer::SurfaceIntersection intersection{
      iris::Bsdf(bxdf, surface_normal, surface_normal),
      iris::HitPoint(trace_ray.Endpoint(1.0),
                     iris::PositionError(0.0, 0.0, 0.0), surface_normal),
      std::nullopt, surface_normal, surface_normal};

  iris::lights::MockLight light;
  EXPECT_CALL(light, Emission(testing::_, testing::_, testing::_, testing::_))
      .WillOnce(testing::Return(nullptr));

  auto* result =
      FromBsdfSample(bsdf_sample, light, trace_ray, intersection,
                     iris::testing::GetAlwaysVisibleVisibilityTester(),
                     iris::testing::GetSpectralAllocator());
  EXPECT_EQ(nullptr, result);
}

TEST(FromBsdfSample, ZeroPdf) {
  iris::Ray trace_ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(0.0, 0.0, 1.0));

  iris::Vector to_light(0.0, 0.866025, 0.5);

  iris::reflectors::MockReflector reflector;

  iris::Bsdf::SampleResult bsdf_sample{reflector, to_light, std::nullopt,
                                       static_cast<iris::visual_t>(1.0)};

  iris::bxdfs::MockBxdf bxdf;
  EXPECT_CALL(bxdf, IsDiffuse(testing::IsNull()))
      .WillRepeatedly(testing::Return(true));

  iris::Vector surface_normal(0.0, 0.0, 1.0);
  iris::RayTracer::RayTracer::SurfaceIntersection intersection{
      iris::Bsdf(bxdf, surface_normal, surface_normal),
      iris::HitPoint(trace_ray.Endpoint(1.0),
                     iris::PositionError(0.0, 0.0, 0.0), surface_normal),
      std::nullopt, surface_normal, surface_normal};

  iris::lights::MockLight light;
  EXPECT_CALL(light, Emission(testing::_, testing::_, testing::_, testing::_))
      .WillOnce(testing::DoAll(testing::SetArgPointee<3, iris::visual_t>(0.0),
                               testing::Return(nullptr)));

  auto* result =
      FromBsdfSample(bsdf_sample, light, trace_ray, intersection,
                     iris::testing::GetAlwaysVisibleVisibilityTester(),
                     iris::testing::GetSpectralAllocator());
  EXPECT_EQ(nullptr, result);
}

TEST(FromBsdfSample, NegativePdf) {
  iris::Ray trace_ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(0.0, 0.0, 1.0));

  iris::Vector to_light(0.0, 0.866025, 0.5);

  iris::reflectors::MockReflector reflector;

  iris::Bsdf::SampleResult bsdf_sample{reflector, to_light, std::nullopt,
                                       static_cast<iris::visual_t>(1.0)};

  iris::bxdfs::MockBxdf bxdf;
  EXPECT_CALL(bxdf, IsDiffuse(testing::IsNull()))
      .WillRepeatedly(testing::Return(true));

  iris::Vector surface_normal(0.0, 0.0, 1.0);
  iris::RayTracer::RayTracer::SurfaceIntersection intersection{
      iris::Bsdf(bxdf, surface_normal, surface_normal),
      iris::HitPoint(trace_ray.Endpoint(1.0),
                     iris::PositionError(0.0, 0.0, 0.0), surface_normal),
      std::nullopt, surface_normal, surface_normal};

  iris::lights::MockLight light;
  EXPECT_CALL(light, Emission(testing::_, testing::_, testing::_, testing::_))
      .WillOnce(testing::DoAll(testing::SetArgPointee<3, iris::visual_t>(-1.0),
                               testing::Return(nullptr)));

  auto* result =
      FromBsdfSample(bsdf_sample, light, trace_ray, intersection,
                     iris::testing::GetAlwaysVisibleVisibilityTester(),
                     iris::testing::GetSpectralAllocator());
  EXPECT_EQ(nullptr, result);
}

TEST(FromBsdfSample, WithEmission) {
  iris::Ray trace_ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(0.0, 0.0, 1.0));

  iris::Vector to_light(0.0, 0.866025, 0.5);

  iris::reflectors::MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(1.0))
      .WillOnce(testing::Return(static_cast<iris::visual_t>(0.5)));

  iris::Bsdf::SampleResult bsdf_sample{reflector, to_light, std::nullopt,
                                       static_cast<iris::visual_t>(1.0)};

  iris::bxdfs::MockBxdf bxdf;
  EXPECT_CALL(bxdf, IsDiffuse(testing::IsNull()))
      .WillRepeatedly(testing::Return(true));

  iris::Vector surface_normal(0.0, 0.0, 1.0);
  iris::RayTracer::RayTracer::SurfaceIntersection intersection{
      iris::Bsdf(bxdf, surface_normal, surface_normal),
      iris::HitPoint(trace_ray.Endpoint(1.0),
                     iris::PositionError(0.0, 0.0, 0.0), surface_normal),
      std::nullopt, surface_normal, surface_normal};

  iris::spectra::MockSpectrum spectrum;
  EXPECT_CALL(spectrum, Intensity(testing::_))
      .WillOnce(testing::Return(static_cast<iris::visual_t>(1.0)));

  iris::lights::MockLight light;
  EXPECT_CALL(light, Emission(testing::_, testing::_, testing::_, testing::_))
      .WillOnce(testing::DoAll(testing::SetArgPointee<3, iris::visual_t>(2.0),
                               testing::Return(&spectrum)));

  auto* result =
      FromBsdfSample(bsdf_sample, light, trace_ray, intersection,
                     iris::testing::GetAlwaysVisibleVisibilityTester(),
                     iris::testing::GetSpectralAllocator());
  ASSERT_NE(nullptr, result);
  EXPECT_NEAR(0.05, result->Intensity(1.0), 0.001);
}

TEST(EstimateDirectLighting, NoSamples) {
  iris::Ray trace_ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(0.0, 0.0, 1.0));
  iris::HitPoint hit_point(trace_ray.Endpoint(1.0),
                           iris::PositionError(0.0, 0.0, 0.0),
                           iris::Vector(0.0, 0.0, 1.0));
  iris::Vector surface_normal(0.0, 0.0, 1.0);
  iris::bxdfs::MockBxdf bxdf;
  iris::RayTracer::RayTracer::SurfaceIntersection intersection{
      iris::Bsdf(bxdf, surface_normal, surface_normal), hit_point, std::nullopt,
      surface_normal, surface_normal};

  iris::Vector to_light(0.0, 0.866025, 0.5);

  EXPECT_CALL(bxdf, IsDiffuse(testing::IsNull()))
      .WillRepeatedly(testing::Return(true));
  EXPECT_CALL(bxdf, SampleDiffuse(-trace_ray.direction, testing::_, testing::_))
      .WillOnce(testing::Return(to_light));
  EXPECT_CALL(bxdf, Pdf(testing::_, testing::_, testing::_, testing::_))
      .WillOnce(testing::Return(static_cast<iris::visual_t>(0.0)));

  iris::lights::MockLight light;
  EXPECT_CALL(light, Sample(testing::_, testing::_, testing::_, testing::_))
      .WillOnce(testing::Return(std::nullopt));

  iris::random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2)).Times(2);

  auto* result = EstimateDirectLighting(
      light, trace_ray, intersection, iris::Sampler(rng), iris::Sampler(rng),
      iris::testing::GetAlwaysVisibleVisibilityTester(),
      iris::testing::GetSpectralAllocator());
  ASSERT_EQ(nullptr, result);
}

TEST(EstimateDirectLighting, DeltaBsdf) {
  iris::Ray trace_ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(0.0, 0.0, 1.0));
  iris::HitPoint hit_point(trace_ray.Endpoint(1.0),
                           iris::PositionError(0.0, 0.0, 0.0),
                           iris::Vector(0.0, 0.0, 1.0));
  iris::Vector surface_normal(0.0, 0.0, 1.0);
  iris::bxdfs::MockBxdf bxdf;
  iris::RayTracer::RayTracer::SurfaceIntersection intersection{
      iris::Bsdf(bxdf, surface_normal, surface_normal), hit_point, std::nullopt,
      surface_normal, surface_normal};

  iris::reflectors::MockReflector reflector;

  iris::Vector to_light(0.0, 0.866025, 0.5);
  EXPECT_CALL(bxdf, IsDiffuse(testing::IsNull()))
      .WillRepeatedly(testing::Return(true));
  EXPECT_CALL(bxdf, SampleDiffuse(-trace_ray.direction, testing::_, testing::_))
      .WillOnce(testing::Return(to_light));
  EXPECT_CALL(bxdf, Pdf(testing::_, testing::_, testing::_, testing::_))
      .WillOnce(testing::Return(1.0));
  EXPECT_CALL(bxdf, Reflectance(testing::_, testing::_, testing::_, testing::_))
      .WillRepeatedly(testing::Return(&reflector));

  iris::lights::MockLight light;

  iris::random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2)).Times(2);

  auto* result = EstimateDirectLighting(
      light, trace_ray, intersection, iris::Sampler(rng), iris::Sampler(rng),
      iris::testing::GetAlwaysVisibleVisibilityTester(),
      iris::testing::GetSpectralAllocator());
  EXPECT_EQ(nullptr, result);
}

TEST(EstimateDirectLighting, DeltaLight) {
  iris::Ray trace_ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(0.0, 0.0, 1.0));
  iris::HitPoint hit_point(trace_ray.Endpoint(1.0),
                           iris::PositionError(0.0, 0.0, 0.0),
                           iris::Vector(0.0, 0.0, 1.0));
  iris::Vector surface_normal(0.0, 0.0, 1.0);
  iris::bxdfs::MockBxdf bxdf;
  iris::RayTracer::RayTracer::SurfaceIntersection intersection{
      iris::Bsdf(bxdf, surface_normal, surface_normal), hit_point, std::nullopt,
      surface_normal, surface_normal};

  iris::Vector to_light(0.0, 0.866025, 0.5);

  iris::reflectors::MockReflector reflector;

  EXPECT_CALL(bxdf, IsDiffuse(testing::IsNull()))
      .WillRepeatedly(testing::Return(true));
  EXPECT_CALL(bxdf, SampleDiffuse(-trace_ray.direction, testing::_, testing::_))
      .WillOnce(testing::Return(to_light));
  EXPECT_CALL(bxdf, Pdf(testing::_, testing::_, testing::_, testing::_))
      .WillRepeatedly(testing::Return(static_cast<iris::visual_t>(1.0)));
  EXPECT_CALL(bxdf, Reflectance(testing::_, testing::_, testing::_, testing::_))
      .WillRepeatedly(testing::Return(&reflector));

  iris::spectra::MockSpectrum spectrum;
  iris::Light::SampleResult light_sample{spectrum, to_light, std::nullopt};

  iris::lights::MockLight light;
  EXPECT_CALL(light, Sample(testing::_, testing::_, testing::_, testing::_))
      .WillOnce(testing::Return(light_sample));

  iris::random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2)).Times(2);

  auto* result = EstimateDirectLighting(
      light, trace_ray, intersection, iris::Sampler(rng), iris::Sampler(rng),
      iris::testing::GetAlwaysVisibleVisibilityTester(),
      iris::testing::GetSpectralAllocator());
  ASSERT_NE(nullptr, result);

  EXPECT_CALL(reflector, Reflectance(1.0))
      .WillOnce(testing::Return(static_cast<iris::visual_t>(0.25)));
  EXPECT_CALL(spectrum, Intensity(testing::_))
      .WillOnce(testing::Return(static_cast<iris::visual_t>(1.0)));
  EXPECT_NEAR(0.125, result->Intensity(1.0), 0.001);
}

TEST(EstimateDirectLighting, FullTest) {
  iris::Ray trace_ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(0.0, 0.0, 1.0));
  iris::HitPoint hit_point(trace_ray.Endpoint(1.0),
                           iris::PositionError(0.0, 0.0, 0.0),
                           iris::Vector(0.0, 0.0, 1.0));
  iris::Vector surface_normal(0.0, 0.0, 1.0);
  iris::bxdfs::MockBxdf bxdf;
  iris::RayTracer::RayTracer::SurfaceIntersection intersection{
      iris::Bsdf(bxdf, surface_normal, surface_normal), hit_point, std::nullopt,
      surface_normal, surface_normal};

  iris::Vector to_light(0.0, 0.866025, 0.5);

  iris::reflectors::MockReflector reflector;

  EXPECT_CALL(bxdf, IsDiffuse(testing::IsNull()))
      .WillRepeatedly(testing::Return(true));
  EXPECT_CALL(bxdf, SampleDiffuse(-trace_ray.direction, testing::_, testing::_))
      .WillOnce(testing::Return(to_light));
  EXPECT_CALL(bxdf, Pdf(testing::_, testing::_, testing::_, testing::_))
      .WillRepeatedly(testing::Return(static_cast<iris::visual_t>(1.0)));
  EXPECT_CALL(bxdf, Reflectance(testing::_, testing::_, testing::_, testing::_))
      .WillRepeatedly(testing::Return(&reflector));

  iris::spectra::MockSpectrum spectrum;
  iris::Light::SampleResult light_sample{spectrum, to_light,
                                         static_cast<iris::visual_t>(1.0)};

  iris::lights::MockLight light;
  EXPECT_CALL(light, Sample(testing::_, testing::_, testing::_, testing::_))
      .WillOnce(testing::Return(light_sample));
  EXPECT_CALL(light, Emission(testing::_, testing::_, testing::_, testing::_))
      .WillOnce(testing::DoAll(testing::SetArgPointee<3, iris::visual_t>(1.0),
                               testing::Return(&spectrum)));

  iris::random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2)).Times(2);

  auto* result = EstimateDirectLighting(
      light, trace_ray, intersection, iris::Sampler(rng), iris::Sampler(rng),
      iris::testing::GetAlwaysVisibleVisibilityTester(),
      iris::testing::GetSpectralAllocator());
  ASSERT_NE(nullptr, result);

  EXPECT_CALL(reflector, Reflectance(1.0))
      .WillRepeatedly(testing::Return(static_cast<iris::visual_t>(0.25)));
  EXPECT_CALL(spectrum, Intensity(testing::_))
      .WillRepeatedly(testing::Return(static_cast<iris::visual_t>(1.0)));
  EXPECT_NEAR(0.125, result->Intensity(1.0), 0.001);
}

TEST(SampleDirectLighting, NoSamples) {
  iris::Ray trace_ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(0.0, 0.0, 1.0));

  iris::HitPoint hit_point(trace_ray.Endpoint(1.0),
                           iris::PositionError(0.0, 0.0, 0.0),
                           iris::Vector(0.0, 0.0, 1.0));
  iris::Vector surface_normal(0.0, 0.0, 1.0);
  iris::bxdfs::MockBxdf bxdf;
  iris::RayTracer::RayTracer::SurfaceIntersection intersection{
      iris::Bsdf(bxdf, surface_normal, surface_normal), hit_point, std::nullopt,
      surface_normal, surface_normal};

  iris::random::MockRandom rng;

  auto* result = SampleDirectLighting(
      iris::testing::GetEmptyLightSampler(), trace_ray, intersection, rng,
      iris::testing::GetAlwaysVisibleVisibilityTester(),
      iris::testing::GetSpectralAllocator());
  EXPECT_EQ(result, nullptr);
}

TEST(SampleDirectLighting, OneZeroPdfSample) {
  iris::Ray trace_ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(0.0, 0.0, 1.0));
  iris::HitPoint hit_point(trace_ray.Endpoint(1.0),
                           iris::PositionError(0.0, 0.0, 0.0),
                           iris::Vector(0.0, 0.0, 1.0));
  iris::Vector surface_normal(0.0, 0.0, 1.0);
  iris::bxdfs::MockBxdf bxdf;
  iris::RayTracer::RayTracer::SurfaceIntersection intersection{
      iris::Bsdf(bxdf, surface_normal, surface_normal), hit_point, std::nullopt,
      surface_normal, surface_normal};

  iris::lights::MockLight light;

  iris::random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2)).Times(2);

  iris::testing::LightSampleListEntry list[] = {
      {&light, static_cast<iris::visual_t>(0.0)},
  };

  iris::testing::ScopedListLightSampler(list, [&](auto& light_sampler) {
    auto* result =
        SampleDirectLighting(light_sampler, trace_ray, intersection, rng,
                             iris::testing::GetAlwaysVisibleVisibilityTester(),
                             iris::testing::GetSpectralAllocator());
    ASSERT_EQ(nullptr, result);
  });
}

TEST(SampleDirectLighting, OneDeltaLight) {
  iris::Ray trace_ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(0.0, 0.0, 1.0));
  iris::HitPoint hit_point(trace_ray.Endpoint(1.0),
                           iris::PositionError(0.0, 0.0, 0.0),
                           iris::Vector(0.0, 0.0, 1.0));
  iris::Vector surface_normal(0.0, 0.0, 1.0);
  iris::bxdfs::MockBxdf bxdf;
  iris::RayTracer::RayTracer::SurfaceIntersection intersection{
      iris::Bsdf(bxdf, surface_normal, surface_normal), hit_point, std::nullopt,
      surface_normal, surface_normal};

  iris::Vector to_light(0.0, 0.866025, 0.5);

  iris::reflectors::MockReflector reflector;

  EXPECT_CALL(bxdf, IsDiffuse(testing::IsNull()))
      .WillRepeatedly(testing::Return(true));
  EXPECT_CALL(bxdf, SampleDiffuse(-trace_ray.direction, testing::_, testing::_))
      .WillOnce(testing::Return(to_light));
  EXPECT_CALL(bxdf, Pdf(testing::_, testing::_, testing::_, testing::_))
      .WillRepeatedly(testing::Return(static_cast<iris::visual_t>(1.0)));
  EXPECT_CALL(bxdf, Reflectance(testing::_, testing::_, testing::_, testing::_))
      .WillRepeatedly(testing::Return(&reflector));

  iris::spectra::MockSpectrum spectrum;
  iris::Light::SampleResult light_sample{spectrum, to_light, std::nullopt};

  iris::lights::MockLight light;
  EXPECT_CALL(light, Sample(testing::_, testing::_, testing::_, testing::_))
      .WillOnce(testing::Return(light_sample));

  iris::random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2)).Times(2);

  iris::testing::LightSampleListEntry list[] = {
      {&light, std::nullopt},
  };

  iris::testing::ScopedListLightSampler(list, [&](auto& light_sampler) {
    auto* result =
        SampleDirectLighting(light_sampler, trace_ray, intersection, rng,
                             iris::testing::GetAlwaysVisibleVisibilityTester(),
                             iris::testing::GetSpectralAllocator());
    ASSERT_NE(nullptr, result);

    EXPECT_CALL(reflector, Reflectance(1.0))
        .WillOnce(testing::Return(static_cast<iris::visual_t>(0.25)));
    EXPECT_CALL(spectrum, Intensity(testing::_))
        .WillOnce(testing::Return(static_cast<iris::visual_t>(1.0)));
    EXPECT_NEAR(0.125, result->Intensity(1.0), 0.001);
  });
}

TEST(SampleDirectLighting, OneProbabilisticDeltaLight) {
  iris::Ray trace_ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(0.0, 0.0, 1.0));
  iris::HitPoint hit_point(trace_ray.Endpoint(1.0),
                           iris::PositionError(0.0, 0.0, 0.0),
                           iris::Vector(0.0, 0.0, 1.0));
  iris::Vector surface_normal(0.0, 0.0, 1.0);
  iris::bxdfs::MockBxdf bxdf;
  iris::RayTracer::RayTracer::SurfaceIntersection intersection{
      iris::Bsdf(bxdf, surface_normal, surface_normal), hit_point, std::nullopt,
      surface_normal, surface_normal};

  iris::Vector to_light(0.0, 0.866025, 0.5);

  iris::reflectors::MockReflector reflector;

  EXPECT_CALL(bxdf, IsDiffuse(testing::IsNull()))
      .WillRepeatedly(testing::Return(true));
  EXPECT_CALL(bxdf, SampleDiffuse(-trace_ray.direction, testing::_, testing::_))
      .WillOnce(testing::Return(to_light));
  EXPECT_CALL(bxdf, Pdf(testing::_, testing::_, testing::_, testing::_))
      .WillRepeatedly(testing::Return(static_cast<iris::visual_t>(1.0)));
  EXPECT_CALL(bxdf, Reflectance(testing::_, testing::_, testing::_, testing::_))
      .WillRepeatedly(testing::Return(&reflector));

  iris::spectra::MockSpectrum spectrum;
  iris::Light::SampleResult light_sample{spectrum, to_light, std::nullopt};

  iris::lights::MockLight light;
  EXPECT_CALL(light, Sample(testing::_, testing::_, testing::_, testing::_))
      .WillOnce(testing::Return(light_sample));

  iris::random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2)).Times(2);

  iris::testing::LightSampleListEntry list[] = {
      {&light, static_cast<iris::visual_t>(0.5)},
  };

  iris::testing::ScopedListLightSampler(list, [&](auto& light_sampler) {
    auto* result =
        SampleDirectLighting(light_sampler, trace_ray, intersection, rng,
                             iris::testing::GetAlwaysVisibleVisibilityTester(),
                             iris::testing::GetSpectralAllocator());
    ASSERT_NE(nullptr, result);

    EXPECT_CALL(reflector, Reflectance(1.0))
        .WillOnce(testing::Return(static_cast<iris::visual_t>(0.25)));
    EXPECT_CALL(spectrum, Intensity(testing::_))
        .WillOnce(testing::Return(static_cast<iris::visual_t>(1.0)));
    EXPECT_NEAR(0.25, result->Intensity(1.0), 0.001);
  });
}

TEST(SampleDirectLighting, TwoDeltaLights) {
  iris::Ray trace_ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(0.0, 0.0, 1.0));
  iris::HitPoint hit_point(trace_ray.Endpoint(1.0),
                           iris::PositionError(0.0, 0.0, 0.0),
                           iris::Vector(0.0, 0.0, 1.0));
  iris::Vector surface_normal(0.0, 0.0, 1.0);
  iris::bxdfs::MockBxdf bxdf;
  iris::RayTracer::RayTracer::SurfaceIntersection intersection{
      iris::Bsdf(bxdf, surface_normal, surface_normal), hit_point, std::nullopt,
      surface_normal, surface_normal};

  iris::Vector to_light(0.0, 0.866025, 0.5);

  iris::reflectors::MockReflector reflector;

  EXPECT_CALL(bxdf, IsDiffuse(testing::IsNull()))
      .WillRepeatedly(testing::Return(true));
  EXPECT_CALL(bxdf, SampleDiffuse(-trace_ray.direction, testing::_, testing::_))
      .WillRepeatedly(testing::Return(to_light));
  EXPECT_CALL(bxdf, Pdf(testing::_, testing::_, testing::_, testing::_))
      .WillRepeatedly(testing::Return(static_cast<iris::visual_t>(1.0)));
  EXPECT_CALL(bxdf, Reflectance(testing::_, testing::_, testing::_, testing::_))
      .WillRepeatedly(testing::Return(&reflector));

  iris::spectra::MockSpectrum spectrum;
  iris::Light::SampleResult light_sample{spectrum, to_light, std::nullopt};

  iris::lights::MockLight light;
  EXPECT_CALL(light, Sample(testing::_, testing::_, testing::_, testing::_))
      .WillRepeatedly(testing::Return(light_sample));

  iris::random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2)).Times(4);

  iris::testing::LightSampleListEntry list[] = {
      {&light, std::nullopt},
      {&light, static_cast<iris::visual_t>(0.5)},
  };

  iris::testing::ScopedListLightSampler(list, [&](auto& light_sampler) {
    auto* result =
        SampleDirectLighting(light_sampler, trace_ray, intersection, rng,
                             iris::testing::GetAlwaysVisibleVisibilityTester(),
                             iris::testing::GetSpectralAllocator());
    ASSERT_NE(nullptr, result);

    EXPECT_CALL(reflector, Reflectance(1.0))
        .WillRepeatedly(testing::Return(static_cast<iris::visual_t>(0.25)));
    EXPECT_CALL(spectrum, Intensity(testing::_))
        .WillRepeatedly(testing::Return(static_cast<iris::visual_t>(1.0)));
    EXPECT_NEAR(0.375, result->Intensity(1.0), 0.001);
  });
}