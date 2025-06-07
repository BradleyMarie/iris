#include "iris/integrators/internal/sample_direct_lighting.h"

#include <optional>

#include "googletest/include/gtest/gtest.h"
#include "iris/bxdf.h"
#include "iris/bxdfs/mock_bxdf.h"
#include "iris/float.h"
#include "iris/hit_point.h"
#include "iris/light.h"
#include "iris/lights/mock_light.h"
#include "iris/position_error.h"
#include "iris/random/mock_random.h"
#include "iris/ray.h"
#include "iris/ray_tracer.h"
#include "iris/reflectors/mock_reflector.h"
#include "iris/spectra/mock_spectrum.h"
#include "iris/spectrum.h"
#include "iris/testing/light_sampler.h"
#include "iris/testing/spectral_allocator.h"
#include "iris/testing/visibility_tester.h"
#include "iris/vector.h"

namespace iris {
namespace integrators {
namespace internal {
namespace {

using ::iris::bxdfs::MockBxdf;
using ::iris::integrators::internal::internal::DeltaLight;
using ::iris::integrators::internal::internal::FromBsdfSample;
using ::iris::integrators::internal::internal::FromLightSample;
using ::iris::integrators::internal::internal::PowerHeuristic;
using ::iris::lights::MockLight;
using ::iris::random::MockRandom;
using ::iris::reflectors::MockReflector;
using ::iris::spectra::MockSpectrum;
using ::iris::testing::GetAlwaysVisibleVisibilityTester;
using ::iris::testing::GetSpectralAllocator;
using ::iris::testing::LightSampleListEntry;
using ::iris::testing::ScopedListLightSampler;
using ::testing::_;
using ::testing::DoAll;
using ::testing::NotNull;
using ::testing::Return;
using ::testing::SetArgPointee;

TEST(PowerHeuristict, Test) {
  EXPECT_NEAR(0.5, PowerHeuristic(1.0, 1.0), 0.001);
  EXPECT_NEAR(0.2, PowerHeuristic(1.0, 2.0), 0.001);
}

TEST(DeltaLight, NoReflectance) {
  MockSpectrum spectrum;
  Vector to_light(1.0, 0.0, 0.0);

  Light::SampleResult light_sample{spectrum, to_light, std::nullopt};

  MockBxdf bxdf;
  EXPECT_CALL(bxdf, IsDiffuse(NotNull()))
      .WillOnce(DoAll(SetArgPointee<0>(1.0), Return(true)));
  EXPECT_CALL(bxdf, PdfDiffuse(_, _, _, _)).WillOnce(Return(0.0));

  RayTracer::RayTracer::SurfaceIntersection intersection{
      Bsdf(bxdf, to_light, to_light),
      HitPoint(Point(0.0, 0.0, 0.0), PositionError(0.0, 0.0, 0.0), to_light),
      std::nullopt, to_light, to_light};

  EXPECT_EQ(nullptr,
            DeltaLight(light_sample, Ray(Point(0.0, 0.0, 0.0), to_light),
                       intersection, GetSpectralAllocator()));
}

TEST(DeltaLight, WithReflectance) {
  Ray trace_ray(Point(0.0, 0.0, 0.0), Vector(0.0, 0.0, 1.0));

  MockSpectrum spectrum;
  EXPECT_CALL(spectrum, Intensity(_)).WillOnce(Return(1.0));

  Vector to_light(0.0, 0.866025, 0.5);

  Light::SampleResult light_sample{spectrum, to_light, std::nullopt};

  MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(1.0))
      .WillOnce(Return(static_cast<visual_t>(0.25)));

  MockBxdf bxdf;
  EXPECT_CALL(bxdf, IsDiffuse(NotNull()))
      .WillOnce(DoAll(SetArgPointee<0>(1.0), Return(true)));
  EXPECT_CALL(bxdf, PdfDiffuse(_, _, _, _))
      .WillOnce(Return(static_cast<visual_t>(100.0)));
  EXPECT_CALL(bxdf, ReflectanceDiffuse(_, _, _, _))
      .WillOnce(Return(&reflector));

  Vector surface_normal(0.0, 0.0, 1.0);
  RayTracer::RayTracer::SurfaceIntersection intersection{
      Bsdf(bxdf, surface_normal, surface_normal),
      HitPoint(trace_ray.Endpoint(1.0), PositionError(0.0, 0.0, 0.0),
               surface_normal),
      std::nullopt, surface_normal, surface_normal};

  const Spectrum* result =
      DeltaLight(light_sample, trace_ray, intersection, GetSpectralAllocator());
  ASSERT_NE(nullptr, result);
  EXPECT_NEAR(0.125, result->Intensity(1.0), 0.001);
}

TEST(FromLightSample, NoReflectance) {
  Ray trace_ray(Point(0.0, 0.0, 0.0), Vector(0.0, 0.0, 1.0));

  MockSpectrum spectrum;

  Vector to_light(0.0, 0.866025, 0.5);

  Light::SampleResult light_sample{spectrum, to_light, std::nullopt};

  MockBxdf bxdf;
  EXPECT_CALL(bxdf, IsDiffuse(NotNull()))
      .WillOnce(DoAll(SetArgPointee<0>(1.0), Return(true)));
  EXPECT_CALL(bxdf, PdfDiffuse(_, _, _, _))
      .WillOnce(Return(static_cast<visual_t>(100.0)));
  EXPECT_CALL(bxdf, ReflectanceDiffuse(_, _, _, _)).WillOnce(Return(nullptr));

  Vector surface_normal(0.0, 0.0, 1.0);
  RayTracer::RayTracer::SurfaceIntersection intersection{
      Bsdf(bxdf, surface_normal, surface_normal),
      HitPoint(trace_ray.Endpoint(1.0), PositionError(0.0, 0.0, 0.0),
               surface_normal),
      std::nullopt, surface_normal, surface_normal};

  const Spectrum* result = FromLightSample(
      light_sample, trace_ray, intersection, GetAlwaysVisibleVisibilityTester(),
      GetSpectralAllocator());
  EXPECT_EQ(nullptr, result);
}

TEST(FromLightSample, WithReflectance) {
  Ray trace_ray(Point(0.0, 0.0, 0.0), Vector(0.0, 0.0, 1.0));

  MockSpectrum spectrum;
  EXPECT_CALL(spectrum, Intensity(_)).WillOnce(Return(1.0));

  Vector to_light(0.0, 0.866025, 0.5);

  Light::SampleResult light_sample{spectrum, to_light, 1.0};

  MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(1.0))
      .WillOnce(Return(static_cast<visual_t>(0.5)));

  MockBxdf bxdf;
  EXPECT_CALL(bxdf, IsDiffuse(NotNull()))
      .WillOnce(DoAll(SetArgPointee<0>(1.0), Return(true)));
  EXPECT_CALL(bxdf, PdfDiffuse(_, _, _, _))
      .WillOnce(Return(static_cast<visual_t>(2.0)));
  EXPECT_CALL(bxdf, ReflectanceDiffuse(_, _, _, _))
      .WillOnce(Return(&reflector));

  Vector surface_normal(0.0, 0.0, 1.0);
  RayTracer::RayTracer::SurfaceIntersection intersection{
      Bsdf(bxdf, surface_normal, surface_normal),
      HitPoint(trace_ray.Endpoint(1.0), PositionError(0.0, 0.0, 0.0),
               surface_normal),
      std::nullopt, surface_normal, surface_normal};

  const Spectrum* result = FromLightSample(
      light_sample, trace_ray, intersection, GetAlwaysVisibleVisibilityTester(),
      GetSpectralAllocator());
  ASSERT_NE(nullptr, result);
  EXPECT_NEAR(0.05, result->Intensity(1.0), 0.001);
}

TEST(FromBsdfSample, NoEmission) {
  Ray trace_ray(Point(0.0, 0.0, 0.0), Vector(0.0, 0.0, 1.0));

  Vector to_light(0.0, 0.866025, 0.5);

  MockReflector reflector;

  Bsdf::SampleResult bsdf_sample{reflector, to_light, std::nullopt, 1.0};

  MockBxdf bxdf;
  EXPECT_CALL(bxdf, IsDiffuse(NotNull()))
      .WillOnce(DoAll(SetArgPointee<0>(1.0), Return(true)));

  Vector surface_normal(0.0, 0.0, 1.0);
  RayTracer::RayTracer::SurfaceIntersection intersection{
      Bsdf(bxdf, surface_normal, surface_normal),
      HitPoint(trace_ray.Endpoint(1.0), PositionError(0.0, 0.0, 0.0),
               surface_normal),
      std::nullopt, surface_normal, surface_normal};

  MockLight light;
  EXPECT_CALL(light, Emission(_, _, _, _)).WillOnce(Return(nullptr));

  const Spectrum* result = FromBsdfSample(
      bsdf_sample, light, trace_ray, intersection,
      GetAlwaysVisibleVisibilityTester(), GetSpectralAllocator());
  EXPECT_EQ(nullptr, result);
}

TEST(FromBsdfSample, ZeroPdf) {
  Ray trace_ray(Point(0.0, 0.0, 0.0), Vector(0.0, 0.0, 1.0));

  Vector to_light(0.0, 0.866025, 0.5);

  MockReflector reflector;

  Bsdf::SampleResult bsdf_sample{reflector, to_light, std::nullopt, 1.0};

  MockBxdf bxdf;
  EXPECT_CALL(bxdf, IsDiffuse(NotNull()))
      .WillOnce(DoAll(SetArgPointee<0>(1.0), Return(true)));

  Vector surface_normal(0.0, 0.0, 1.0);
  RayTracer::RayTracer::SurfaceIntersection intersection{
      Bsdf(bxdf, surface_normal, surface_normal),
      HitPoint(trace_ray.Endpoint(1.0), PositionError(0.0, 0.0, 0.0),
               surface_normal),
      std::nullopt, surface_normal, surface_normal};

  MockLight light;
  EXPECT_CALL(light, Emission(_, _, _, _))
      .WillOnce(DoAll(SetArgPointee<3, visual_t>(0.0), Return(nullptr)));

  const Spectrum* result = FromBsdfSample(
      bsdf_sample, light, trace_ray, intersection,
      GetAlwaysVisibleVisibilityTester(), GetSpectralAllocator());
  EXPECT_EQ(nullptr, result);
}

TEST(FromBsdfSample, NegativePdf) {
  Ray trace_ray(Point(0.0, 0.0, 0.0), Vector(0.0, 0.0, 1.0));

  Vector to_light(0.0, 0.866025, 0.5);

  MockReflector reflector;

  Bsdf::SampleResult bsdf_sample{reflector, to_light, std::nullopt, 1.0};

  MockBxdf bxdf;
  EXPECT_CALL(bxdf, IsDiffuse(NotNull()))
      .WillOnce(DoAll(SetArgPointee<0>(1.0), Return(true)));

  Vector surface_normal(0.0, 0.0, 1.0);
  RayTracer::RayTracer::SurfaceIntersection intersection{
      Bsdf(bxdf, surface_normal, surface_normal),
      HitPoint(trace_ray.Endpoint(1.0), PositionError(0.0, 0.0, 0.0),
               surface_normal),
      std::nullopt, surface_normal, surface_normal};

  MockLight light;
  EXPECT_CALL(light, Emission(_, _, _, _))
      .WillOnce(DoAll(SetArgPointee<3, visual_t>(-1.0), Return(nullptr)));

  const Spectrum* result = FromBsdfSample(
      bsdf_sample, light, trace_ray, intersection,
      GetAlwaysVisibleVisibilityTester(), GetSpectralAllocator());
  EXPECT_EQ(nullptr, result);
}

TEST(FromBsdfSample, WithEmission) {
  Ray trace_ray(Point(0.0, 0.0, 0.0), Vector(0.0, 0.0, 1.0));

  Vector to_light(0.0, 0.866025, 0.5);

  MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(1.0))
      .WillOnce(Return(static_cast<visual_t>(0.5)));

  Bsdf::SampleResult bsdf_sample{reflector, to_light, std::nullopt, 1.0};

  MockBxdf bxdf;
  EXPECT_CALL(bxdf, IsDiffuse(NotNull()))
      .WillOnce(DoAll(SetArgPointee<0>(1.0), Return(true)));

  Vector surface_normal(0.0, 0.0, 1.0);
  RayTracer::RayTracer::SurfaceIntersection intersection{
      Bsdf(bxdf, surface_normal, surface_normal),
      HitPoint(trace_ray.Endpoint(1.0), PositionError(0.0, 0.0, 0.0),
               surface_normal),
      std::nullopt, surface_normal, surface_normal};

  MockSpectrum spectrum;
  EXPECT_CALL(spectrum, Intensity(_)).WillOnce(Return(1.0));

  MockLight light;
  EXPECT_CALL(light, Emission(_, _, _, _))
      .WillOnce(DoAll(SetArgPointee<3, visual_t>(2.0), Return(&spectrum)));

  const Spectrum* result = FromBsdfSample(
      bsdf_sample, light, trace_ray, intersection,
      GetAlwaysVisibleVisibilityTester(), GetSpectralAllocator());
  ASSERT_NE(nullptr, result);
  EXPECT_NEAR(0.05, result->Intensity(1.0), 0.001);
}

TEST(EstimateDirectLighting, NoSamples) {
  Ray trace_ray(Point(0.0, 0.0, 0.0), Vector(0.0, 0.0, 1.0));
  HitPoint hit_point(trace_ray.Endpoint(1.0), PositionError(0.0, 0.0, 0.0),
                     Vector(0.0, 0.0, 1.0));
  Vector surface_normal(0.0, 0.0, 1.0);

  MockBxdf bxdf;
  EXPECT_CALL(bxdf, IsDiffuse(NotNull()))
      .WillOnce(DoAll(SetArgPointee<0>(1.0), Return(true)));

  RayTracer::RayTracer::SurfaceIntersection intersection{
      Bsdf(bxdf, surface_normal, surface_normal), hit_point, std::nullopt,
      surface_normal, surface_normal};

  Vector to_light(0.0, 0.866025, 0.5);

  EXPECT_CALL(bxdf, SampleDiffuse(-trace_ray.direction, _, _))
      .WillOnce(Return(to_light));
  EXPECT_CALL(bxdf, PdfDiffuse(_, _, _, _))
      .WillOnce(Return(static_cast<visual_t>(0.0)));

  MockLight light;
  EXPECT_CALL(light, Sample(_, _, _, _)).WillOnce(Return(std::nullopt));

  MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2)).Times(2);

  const Spectrum* result = EstimateDirectLighting(
      light, trace_ray, intersection, Sampler(rng), Sampler(rng),
      GetAlwaysVisibleVisibilityTester(), GetSpectralAllocator());
  ASSERT_EQ(nullptr, result);
}

TEST(EstimateDirectLighting, DeltaBsdf) {
  Ray trace_ray(Point(0.0, 0.0, 0.0), Vector(0.0, 0.0, 1.0));
  HitPoint hit_point(trace_ray.Endpoint(1.0), PositionError(0.0, 0.0, 0.0),
                     Vector(0.0, 0.0, 1.0));
  Vector surface_normal(0.0, 0.0, 1.0);

  MockBxdf bxdf;
  EXPECT_CALL(bxdf, IsDiffuse(NotNull()))
      .WillOnce(DoAll(SetArgPointee<0>(0.0), Return(false)));

  RayTracer::RayTracer::SurfaceIntersection intersection{
      Bsdf(bxdf, surface_normal, surface_normal), hit_point, std::nullopt,
      surface_normal, surface_normal};

  MockLight light;

  MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2)).Times(2);

  const Spectrum* result = EstimateDirectLighting(
      light, trace_ray, intersection, Sampler(rng), Sampler(rng),
      GetAlwaysVisibleVisibilityTester(), GetSpectralAllocator());
  EXPECT_EQ(nullptr, result);
}

TEST(EstimateDirectLighting, DeltaLight) {
  Ray trace_ray(Point(0.0, 0.0, 0.0), Vector(0.0, 0.0, 1.0));
  HitPoint hit_point(trace_ray.Endpoint(1.0), PositionError(0.0, 0.0, 0.0),
                     Vector(0.0, 0.0, 1.0));
  Vector surface_normal(0.0, 0.0, 1.0);

  MockBxdf bxdf;
  EXPECT_CALL(bxdf, IsDiffuse(NotNull()))
      .WillOnce(DoAll(SetArgPointee<0>(1.0), Return(true)));

  RayTracer::RayTracer::SurfaceIntersection intersection{
      Bsdf(bxdf, surface_normal, surface_normal), hit_point, std::nullopt,
      surface_normal, surface_normal};

  Vector to_light(0.0, 0.866025, 0.5);

  MockReflector reflector;

  EXPECT_CALL(bxdf, PdfDiffuse(_, _, _, _)).WillRepeatedly(Return(1.0));
  EXPECT_CALL(bxdf, ReflectanceDiffuse(_, _, _, _))
      .WillRepeatedly(Return(&reflector));

  MockSpectrum spectrum;
  Light::SampleResult light_sample{spectrum, to_light, std::nullopt};

  MockLight light;
  EXPECT_CALL(light, Sample(_, _, _, _)).WillOnce(Return(light_sample));

  MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2)).Times(2);

  const Spectrum* result = EstimateDirectLighting(
      light, trace_ray, intersection, Sampler(rng), Sampler(rng),
      GetAlwaysVisibleVisibilityTester(), GetSpectralAllocator());
  ASSERT_NE(nullptr, result);

  EXPECT_CALL(reflector, Reflectance(1.0))
      .WillOnce(Return(static_cast<visual_t>(0.25)));
  EXPECT_CALL(spectrum, Intensity(_)).WillOnce(Return(1.0));
  EXPECT_NEAR(0.125, result->Intensity(1.0), 0.001);
}

TEST(EstimateDirectLighting, FullTest) {
  Ray trace_ray(Point(0.0, 0.0, 0.0), Vector(0.0, 0.0, 1.0));
  HitPoint hit_point(trace_ray.Endpoint(1.0), PositionError(0.0, 0.0, 0.0),
                     Vector(0.0, 0.0, 1.0));
  Vector surface_normal(0.0, 0.0, 1.0);

  MockBxdf bxdf;
  EXPECT_CALL(bxdf, IsDiffuse(NotNull()))
      .WillOnce(DoAll(SetArgPointee<0>(1.0), Return(true)));

  RayTracer::RayTracer::SurfaceIntersection intersection{
      Bsdf(bxdf, surface_normal, surface_normal), hit_point, std::nullopt,
      surface_normal, surface_normal};

  Vector to_light(0.0, 0.866025, 0.5);

  MockReflector reflector;

  EXPECT_CALL(bxdf, SampleDiffuse(-trace_ray.direction, _, _))
      .WillOnce(Return(to_light));
  EXPECT_CALL(bxdf, PdfDiffuse(_, _, _, _)).WillRepeatedly(Return(1.0));
  EXPECT_CALL(bxdf, ReflectanceDiffuse(_, _, _, _))
      .WillRepeatedly(Return(&reflector));

  MockSpectrum spectrum;
  Light::SampleResult light_sample{spectrum, to_light, 1.0};

  MockLight light;
  EXPECT_CALL(light, Sample(_, _, _, _)).WillOnce(Return(light_sample));
  EXPECT_CALL(light, Emission(_, _, _, _))
      .WillOnce(DoAll(SetArgPointee<3, visual_t>(1.0), Return(&spectrum)));

  MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2)).Times(2);

  const Spectrum* result = EstimateDirectLighting(
      light, trace_ray, intersection, Sampler(rng), Sampler(rng),
      GetAlwaysVisibleVisibilityTester(), GetSpectralAllocator());
  ASSERT_NE(nullptr, result);

  EXPECT_CALL(reflector, Reflectance(1.0))
      .WillRepeatedly(Return(static_cast<visual_t>(0.25)));
  EXPECT_CALL(spectrum, Intensity(_)).WillRepeatedly(Return(1.0));
  EXPECT_NEAR(0.125, result->Intensity(1.0), 0.001);
}

TEST(SampleDirectLighting, NoSamples) {
  Ray trace_ray(Point(0.0, 0.0, 0.0), Vector(0.0, 0.0, 1.0));

  HitPoint hit_point(trace_ray.Endpoint(1.0), PositionError(0.0, 0.0, 0.0),
                     Vector(0.0, 0.0, 1.0));
  Vector surface_normal(0.0, 0.0, 1.0);

  MockBxdf bxdf;
  EXPECT_CALL(bxdf, IsDiffuse(NotNull()))
      .WillOnce(DoAll(SetArgPointee<0>(1.0), Return(true)));

  RayTracer::RayTracer::SurfaceIntersection intersection{
      Bsdf(bxdf, surface_normal, surface_normal), hit_point, std::nullopt,
      surface_normal, surface_normal};

  MockRandom rng;

  const Spectrum* result = SampleDirectLighting(
      testing::GetEmptyLightSampler(), trace_ray, intersection, rng,
      GetAlwaysVisibleVisibilityTester(), GetSpectralAllocator());
  EXPECT_EQ(result, nullptr);
}

TEST(SampleDirectLighting, OneZeroPdfSample) {
  Ray trace_ray(Point(0.0, 0.0, 0.0), Vector(0.0, 0.0, 1.0));
  HitPoint hit_point(trace_ray.Endpoint(1.0), PositionError(0.0, 0.0, 0.0),
                     Vector(0.0, 0.0, 1.0));
  Vector surface_normal(0.0, 0.0, 1.0);

  MockBxdf bxdf;
  EXPECT_CALL(bxdf, IsDiffuse(NotNull()))
      .WillOnce(DoAll(SetArgPointee<0>(1.0), Return(true)));

  RayTracer::RayTracer::SurfaceIntersection intersection{
      Bsdf(bxdf, surface_normal, surface_normal), hit_point, std::nullopt,
      surface_normal, surface_normal};

  MockLight light;

  MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2)).Times(2);

  LightSampleListEntry list[] = {
      {&light, static_cast<visual_t>(0.0)},
  };

  ScopedListLightSampler(list, [&](auto& light_sampler) {
    const Spectrum* result = SampleDirectLighting(
        light_sampler, trace_ray, intersection, rng,
        GetAlwaysVisibleVisibilityTester(), GetSpectralAllocator());
    ASSERT_EQ(nullptr, result);
  });
}

TEST(SampleDirectLighting, OneDeltaLight) {
  Ray trace_ray(Point(0.0, 0.0, 0.0), Vector(0.0, 0.0, 1.0));
  HitPoint hit_point(trace_ray.Endpoint(1.0), PositionError(0.0, 0.0, 0.0),
                     Vector(0.0, 0.0, 1.0));
  Vector surface_normal(0.0, 0.0, 1.0);

  MockBxdf bxdf;
  EXPECT_CALL(bxdf, IsDiffuse(NotNull()))
      .WillOnce(DoAll(SetArgPointee<0>(1.0), Return(true)));

  RayTracer::RayTracer::SurfaceIntersection intersection{
      Bsdf(bxdf, surface_normal, surface_normal), hit_point, std::nullopt,
      surface_normal, surface_normal};

  Vector to_light(0.0, 0.866025, 0.5);

  MockReflector reflector;

  EXPECT_CALL(bxdf, PdfDiffuse(_, _, _, _)).WillRepeatedly(Return(1.0));
  EXPECT_CALL(bxdf, ReflectanceDiffuse(_, _, _, _))
      .WillRepeatedly(Return(&reflector));

  MockSpectrum spectrum;
  Light::SampleResult light_sample{spectrum, to_light, std::nullopt};

  MockLight light;
  EXPECT_CALL(light, Sample(_, _, _, _)).WillOnce(Return(light_sample));

  MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2)).Times(2);

  LightSampleListEntry list[] = {
      {&light, std::nullopt},
  };

  ScopedListLightSampler(list, [&](auto& light_sampler) {
    const Spectrum* result = SampleDirectLighting(
        light_sampler, trace_ray, intersection, rng,
        GetAlwaysVisibleVisibilityTester(), GetSpectralAllocator());
    ASSERT_NE(nullptr, result);

    EXPECT_CALL(reflector, Reflectance(1.0))
        .WillOnce(Return(static_cast<visual_t>(0.25)));
    EXPECT_CALL(spectrum, Intensity(_)).WillOnce(Return(1.0));
    EXPECT_NEAR(0.125, result->Intensity(1.0), 0.001);
  });
}

TEST(SampleDirectLighting, OneProbabilisticDeltaLight) {
  Ray trace_ray(Point(0.0, 0.0, 0.0), Vector(0.0, 0.0, 1.0));
  HitPoint hit_point(trace_ray.Endpoint(1.0), PositionError(0.0, 0.0, 0.0),
                     Vector(0.0, 0.0, 1.0));
  Vector surface_normal(0.0, 0.0, 1.0);

  MockBxdf bxdf;
  EXPECT_CALL(bxdf, IsDiffuse(NotNull()))
      .WillOnce(DoAll(SetArgPointee<0>(1.0), Return(true)));

  RayTracer::RayTracer::SurfaceIntersection intersection{
      Bsdf(bxdf, surface_normal, surface_normal), hit_point, std::nullopt,
      surface_normal, surface_normal};

  Vector to_light(0.0, 0.866025, 0.5);

  MockReflector reflector;

  EXPECT_CALL(bxdf, PdfDiffuse(_, _, _, _)).WillRepeatedly(Return(1.0));
  EXPECT_CALL(bxdf, ReflectanceDiffuse(_, _, _, _))
      .WillRepeatedly(Return(&reflector));

  MockSpectrum spectrum;
  Light::SampleResult light_sample{spectrum, to_light, std::nullopt};

  MockLight light;
  EXPECT_CALL(light, Sample(_, _, _, _)).WillOnce(Return(light_sample));

  MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2)).Times(2);

  LightSampleListEntry list[] = {
      {&light, static_cast<visual_t>(0.5)},
  };

  ScopedListLightSampler(list, [&](auto& light_sampler) {
    const Spectrum* result = SampleDirectLighting(
        light_sampler, trace_ray, intersection, rng,
        GetAlwaysVisibleVisibilityTester(), GetSpectralAllocator());
    ASSERT_NE(nullptr, result);

    EXPECT_CALL(reflector, Reflectance(1.0))
        .WillOnce(Return(static_cast<visual_t>(0.25)));
    EXPECT_CALL(spectrum, Intensity(_)).WillOnce(Return(1.0));
    EXPECT_NEAR(0.25, result->Intensity(1.0), 0.001);
  });
}

TEST(SampleDirectLighting, TwoDeltaLights) {
  Ray trace_ray(Point(0.0, 0.0, 0.0), Vector(0.0, 0.0, 1.0));
  HitPoint hit_point(trace_ray.Endpoint(1.0), PositionError(0.0, 0.0, 0.0),
                     Vector(0.0, 0.0, 1.0));
  Vector surface_normal(0.0, 0.0, 1.0);

  MockBxdf bxdf;
  EXPECT_CALL(bxdf, IsDiffuse(NotNull()))
      .WillOnce(DoAll(SetArgPointee<0>(1.0), Return(true)));

  RayTracer::RayTracer::SurfaceIntersection intersection{
      Bsdf(bxdf, surface_normal, surface_normal), hit_point, std::nullopt,
      surface_normal, surface_normal};

  Vector to_light(0.0, 0.866025, 0.5);

  MockReflector reflector;

  EXPECT_CALL(bxdf, PdfDiffuse(_, _, _, _)).WillRepeatedly(Return(1.0));
  EXPECT_CALL(bxdf, ReflectanceDiffuse(_, _, _, _))
      .WillRepeatedly(Return(&reflector));

  MockSpectrum spectrum;
  Light::SampleResult light_sample{spectrum, to_light, std::nullopt};

  MockLight light;
  EXPECT_CALL(light, Sample(_, _, _, _)).WillRepeatedly(Return(light_sample));

  MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2)).Times(4);

  LightSampleListEntry list[] = {
      {&light, std::nullopt},
      {&light, static_cast<visual_t>(0.5)},
  };

  ScopedListLightSampler(list, [&](auto& light_sampler) {
    const Spectrum* result = SampleDirectLighting(
        light_sampler, trace_ray, intersection, rng,
        GetAlwaysVisibleVisibilityTester(), GetSpectralAllocator());
    ASSERT_NE(nullptr, result);

    EXPECT_CALL(reflector, Reflectance(1.0))
        .WillRepeatedly(Return(static_cast<visual_t>(0.25)));
    EXPECT_CALL(spectrum, Intensity(_)).WillRepeatedly(Return(1.0));
    EXPECT_NEAR(0.375, result->Intensity(1.0), 0.001);
  });
}

}  // namespace
}  // namespace internal
}  // namespace integrators
}  // namespace iris
