#include "iris/integrators/path_integrator.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/albedo_matchers/mock_albedo_matcher.h"
#include "iris/bxdfs/mock_bxdf.h"
#include "iris/environmental_lights/mock_environmental_light.h"
#include "iris/lights/mock_light.h"
#include "iris/random/mock_random.h"
#include "iris/reflectors/mock_reflector.h"
#include "iris/spectra/mock_spectrum.h"
#include "iris/testing/light_sampler.h"
#include "iris/testing/ray_tracer.h"
#include "iris/testing/spectral_allocator.h"
#include "iris/testing/visibility_tester.h"

static const iris::RayDifferential kTraceRay(
    iris::Ray(iris::Point(0.0, 0.0, 1.0), iris::Vector(0.0, 0.0, 1.0)));

TEST(PathIntegratorTest, NoHits) {
  iris::albedo_matchers::MockAlbedoMatcher albedo_matcher;
  iris::random::MockRandom rng;

  iris::integrators::PathIntegrator integrator(1.0, 1.0, 1u, 8u);
  EXPECT_EQ(nullptr,
            integrator.Integrate(
                kTraceRay, iris::testing::GetNoHitsRayTracer(),
                iris::testing::GetEmptyLightSampler(),
                iris::testing::GetAlwaysVisibleVisibilityTester(),
                albedo_matcher, iris::testing::GetSpectralAllocator(), rng));
}

TEST(PathIntegratorTest, HitsEmissiveWithNoReflectance) {
  iris::albedo_matchers::MockAlbedoMatcher albedo_matcher;

  iris::spectra::MockSpectrum spectrum;
  iris::environmental_lights::MockEnvironmentalLight environmental_light;
  EXPECT_CALL(environmental_light, Emission(testing::_, testing::_, testing::_))
      .WillOnce(testing::Return(&spectrum));

  iris::random::MockRandom rng;

  iris::integrators::PathIntegrator integrator(1.0, 1.0, 1u, 8u);
  iris::testing::ScopedNoHitsRayTracer(
      environmental_light, [&](auto& ray_tracer) {
        EXPECT_EQ(
            &spectrum,
            integrator.Integrate(
                kTraceRay, ray_tracer, iris::testing::GetEmptyLightSampler(),
                iris::testing::GetAlwaysVisibleVisibilityTester(),
                albedo_matcher, iris::testing::GetSpectralAllocator(), rng));
      });
}

TEST(PathIntegratorTest, BounceLimit) {
  iris::reflectors::MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(testing::_))
      .WillRepeatedly(testing::Return(static_cast<iris::visual_t>(0.5)));

  iris::albedo_matchers::MockAlbedoMatcher albedo_matcher;
  EXPECT_CALL(albedo_matcher, Match(testing::_))
      .WillRepeatedly(testing::Return(static_cast<iris::visual_t>(0.5)));

  iris::bxdfs::MockBxdf specular0;
  EXPECT_CALL(specular0, IsDiffuse(testing::IsNull()))
      .WillRepeatedly(testing::Return(false));
  EXPECT_CALL(specular0, Sample(testing::_, testing::_, testing::_, testing::_))
      .WillRepeatedly(testing::Return(iris::Bxdf::SampleResult{
          iris::Vector(0.0, 0.0, -1.0), std::nullopt, &specular0, 1.0}));
  EXPECT_CALL(specular0, Pdf(testing::_, testing::_, testing::_, testing::_))
      .WillRepeatedly(testing::Return(1.0));
  EXPECT_CALL(specular0,
              Reflectance(testing::_, testing::_, testing::_, testing::_))
      .WillRepeatedly(testing::Return(&reflector));

  iris::spectra::MockSpectrum spectrum;
  EXPECT_CALL(spectrum, Intensity(testing::_))
      .WillRepeatedly(testing::Return(static_cast<iris::visual_t>(1.0)));

  iris::random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2)).Times(1);

  iris::testing::RayTracerPathNode path[] = {
      {1.0, nullptr, &specular0, iris::Vector(0.0, 0.0, -1.0),
       iris::Vector(0.0, 0.0, -1.0)},
      {1.0, &spectrum, nullptr, iris::Vector(0.0, 0.0, -1.0),
       iris::Vector(0.0, 0.0, -1.0)}};

  iris::integrators::PathIntegrator integrator0(1.0, 1.0, 0u, 0u);
  iris::testing::ScopedHitsRayTracer(nullptr, path, [&](auto& ray_tracer) {
    EXPECT_EQ(nullptr,
              integrator0.Integrate(
                  kTraceRay, ray_tracer, iris::testing::GetEmptyLightSampler(),
                  iris::testing::GetAlwaysVisibleVisibilityTester(),
                  albedo_matcher, iris::testing::GetSpectralAllocator(), rng));
  });

  iris::integrators::PathIntegrator integrator1(1.0, 1.0, 1u, 1u);
  iris::testing::ScopedHitsRayTracer(nullptr, path, [&](auto& ray_tracer) {
    auto* result = integrator1.Integrate(
        kTraceRay, ray_tracer, iris::testing::GetEmptyLightSampler(),
        iris::testing::GetAlwaysVisibleVisibilityTester(), albedo_matcher,
        iris::testing::GetSpectralAllocator(), rng);
    EXPECT_TRUE(result);
    EXPECT_EQ(0.5, result->Intensity(1.0));
  });
}

TEST(PathIntegratorTest, NoBsdf) {
  iris::albedo_matchers::MockAlbedoMatcher albedo_matcher;

  iris::spectra::MockSpectrum spectrum;
  EXPECT_CALL(spectrum, Intensity(testing::_))
      .WillRepeatedly(testing::Return(static_cast<iris::visual_t>(1.0)));

  iris::random::MockRandom rng;

  iris::testing::RayTracerPathNode path[] = {
      {1.0, nullptr, nullptr, iris::Vector(0.0, 0.0, -1.0),
       iris::Vector(0.0, 0.0, -1.0)},
      {1.0, &spectrum, nullptr, iris::Vector(0.0, 0.0, -1.0),
       iris::Vector(0.0, 0.0, -1.0)}};

  iris::integrators::PathIntegrator integrator(1.0, 1.0, 3u, 3u);
  iris::testing::ScopedHitsRayTracer(nullptr, path, [&](auto& ray_tracer) {
    EXPECT_EQ(nullptr,
              integrator.Integrate(
                  kTraceRay, ray_tracer, iris::testing::GetEmptyLightSampler(),
                  iris::testing::GetAlwaysVisibleVisibilityTester(),
                  albedo_matcher, iris::testing::GetSpectralAllocator(), rng));
  });
}

TEST(PathIntegratorTest, BsdfSampleFails) {
  iris::reflectors::MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(testing::_))
      .WillRepeatedly(testing::Return(static_cast<iris::visual_t>(0.5)));

  iris::albedo_matchers::MockAlbedoMatcher albedo_matcher;
  EXPECT_CALL(albedo_matcher, Match(testing::_))
      .WillRepeatedly(testing::Return(static_cast<iris::visual_t>(0.5)));

  iris::bxdfs::MockBxdf specular0;
  EXPECT_CALL(specular0, Sample(testing::_, testing::_, testing::_, testing::_))
      .WillRepeatedly(testing::Return(iris::Bxdf::SampleResult{
          iris::Vector(0.0, 0.0, -1.0), std::nullopt, &specular0, 1.0}));
  EXPECT_CALL(specular0, Pdf(testing::_, testing::_, testing::_, testing::_))
      .WillRepeatedly(testing::Return(static_cast<iris::visual_t>(0.0)));

  iris::spectra::MockSpectrum spectrum;
  EXPECT_CALL(spectrum, Intensity(testing::_))
      .WillRepeatedly(testing::Return(static_cast<iris::visual_t>(1.0)));

  iris::random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2)).Times(1);

  iris::testing::RayTracerPathNode path[] = {
      {1.0, nullptr, &specular0, iris::Vector(0.0, 0.0, -1.0),
       iris::Vector(0.0, 0.0, -1.0)},
      {1.0, &spectrum, nullptr, iris::Vector(0.0, 0.0, -1.0),
       iris::Vector(0.0, 0.0, -1.0)}};

  iris::integrators::PathIntegrator integrator0(1.0, 1.0, 3u, 3u);
  iris::testing::ScopedHitsRayTracer(nullptr, path, [&](auto& ray_tracer) {
    EXPECT_EQ(nullptr,
              integrator0.Integrate(
                  kTraceRay, ray_tracer, iris::testing::GetEmptyLightSampler(),
                  iris::testing::GetAlwaysVisibleVisibilityTester(),
                  albedo_matcher, iris::testing::GetSpectralAllocator(), rng));
  });
}

TEST(PathIntegratorTest, TwoSpecularBouncesHitsEmissive) {
  iris::reflectors::MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(testing::_))
      .WillRepeatedly(testing::Return(static_cast<iris::visual_t>(0.5)));

  iris::albedo_matchers::MockAlbedoMatcher albedo_matcher;
  EXPECT_CALL(albedo_matcher, Match(testing::_))
      .WillRepeatedly(testing::Return(static_cast<iris::visual_t>(0.5)));

  iris::bxdfs::MockBxdf specular0, specular1;
  EXPECT_CALL(specular0, IsDiffuse(testing::IsNull()))
      .WillRepeatedly(testing::Return(false));
  EXPECT_CALL(specular1, IsDiffuse(testing::IsNull()))
      .WillRepeatedly(testing::Return(false));
  EXPECT_CALL(specular0, Sample(testing::_, testing::_, testing::_, testing::_))
      .WillRepeatedly(testing::Return(iris::Bxdf::SampleResult{
          iris::Vector(0.0, 0.0, -1.0), std::nullopt, &specular0, 1.0}));
  EXPECT_CALL(specular1, Sample(testing::_, testing::_, testing::_, testing::_))
      .WillRepeatedly(testing::Return(iris::Bxdf::SampleResult{
          iris::Vector(0.0, 0.0, -1.0), std::nullopt, &specular1, 1.0}));
  EXPECT_CALL(specular0, Pdf(testing::_, testing::_, testing::_, testing::_))
      .WillRepeatedly(testing::Return(1.0));
  EXPECT_CALL(specular1, Pdf(testing::_, testing::_, testing::_, testing::_))
      .WillRepeatedly(testing::Return(1.0));
  EXPECT_CALL(specular0,
              Reflectance(testing::_, testing::_, testing::_, testing::_))
      .WillRepeatedly(testing::Return(&reflector));
  EXPECT_CALL(specular1,
              Reflectance(testing::_, testing::_, testing::_, testing::_))
      .WillRepeatedly(testing::Return(&reflector));

  iris::spectra::MockSpectrum spectrum;
  EXPECT_CALL(spectrum, Intensity(testing::_))
      .WillRepeatedly(testing::Return(static_cast<iris::visual_t>(1.0)));

  iris::random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2)).Times(2);

  iris::testing::RayTracerPathNode path[] = {
      {1.0, nullptr, &specular0, iris::Vector(0.0, 0.0, -1.0),
       iris::Vector(0.0, 0.0, -1.0)},
      {1.0, nullptr, &specular1, iris::Vector(0.0, 0.0, -1.0),
       iris::Vector(0.0, 0.0, -1.0)},
      {1.0, &spectrum, nullptr, iris::Vector(0.0, 0.0, -1.0),
       iris::Vector(0.0, 0.0, -1.0)}};

  iris::integrators::PathIntegrator integrator(1.0, 1.0, 4u, 8u);
  iris::testing::ScopedHitsRayTracer(nullptr, path, [&](auto& ray_tracer) {
    auto* result = integrator.Integrate(
        kTraceRay, ray_tracer, iris::testing::GetEmptyLightSampler(),
        iris::testing::GetAlwaysVisibleVisibilityTester(), albedo_matcher,
        iris::testing::GetSpectralAllocator(), rng);
    EXPECT_TRUE(result);
    EXPECT_EQ(0.25, result->Intensity(1.0));
  });
}

TEST(PathIntegratorTest, DiffuseBounceToSpecularBounceToEmissive) {
  iris::reflectors::MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(testing::_))
      .WillRepeatedly(testing::Return(static_cast<iris::visual_t>(0.5)));

  iris::albedo_matchers::MockAlbedoMatcher albedo_matcher;
  EXPECT_CALL(albedo_matcher, Match(testing::_))
      .WillRepeatedly(testing::Return(static_cast<iris::visual_t>(0.5)));

  iris::bxdfs::MockBxdf diffuse, specular;
  EXPECT_CALL(diffuse, IsDiffuse(testing::IsNull()))
      .WillRepeatedly(testing::Return(true));
  EXPECT_CALL(specular, IsDiffuse(testing::IsNull()))
      .WillRepeatedly(testing::Return(false));
  EXPECT_CALL(diffuse, Sample(testing::_, testing::_, testing::_, testing::_))
      .WillRepeatedly(testing::Return(iris::Bxdf::SampleResult{
          iris::Normalize(iris::Vector(1.0, 0.0, -1.0)), std::nullopt, &diffuse,
          1.0}));
  EXPECT_CALL(specular, Sample(testing::_, testing::_, testing::_, testing::_))
      .WillRepeatedly(testing::Return(iris::Bxdf::SampleResult{
          iris::Vector(0.0, 0.0, -1.0), std::nullopt, &specular, 1.0}));
  EXPECT_CALL(diffuse, Pdf(testing::_, testing::_, testing::_, testing::_))
      .WillRepeatedly(testing::Return(static_cast<iris::visual_t>(0.5)));
  EXPECT_CALL(specular, Pdf(testing::_, testing::_, testing::_, testing::_))
      .WillRepeatedly(testing::Return(1.0));
  EXPECT_CALL(diffuse,
              Reflectance(testing::_, testing::_, testing::_, testing::_))
      .WillRepeatedly(testing::Return(&reflector));
  EXPECT_CALL(specular,
              Reflectance(testing::_, testing::_, testing::_, testing::_))
      .WillRepeatedly(testing::Return(&reflector));

  iris::spectra::MockSpectrum spectrum;
  EXPECT_CALL(spectrum, Intensity(testing::_))
      .WillRepeatedly(testing::Return(static_cast<iris::visual_t>(1.0)));

  iris::random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2)).Times(2);

  iris::testing::RayTracerPathNode path[] = {
      {1.0, nullptr, &diffuse, iris::Vector(0.0, 0.0, -1.0),
       iris::Vector(0.0, 0.0, -1.0)},
      {1.0, nullptr, &specular, iris::Vector(0.0, 0.0, -1.0),
       iris::Vector(0.0, 0.0, -1.0)},
      {1.0, &spectrum, nullptr, iris::Vector(0.0, 0.0, -1.0),
       iris::Vector(0.0, 0.0, -1.0)}};

  iris::integrators::PathIntegrator integrator(1.0, 1.0, 4u, 8u);
  iris::testing::ScopedHitsRayTracer(nullptr, path, [&](auto& ray_tracer) {
    auto* result = integrator.Integrate(
        kTraceRay, ray_tracer, iris::testing::GetEmptyLightSampler(),
        iris::testing::GetAlwaysVisibleVisibilityTester(), albedo_matcher,
        iris::testing::GetSpectralAllocator(), rng);
    EXPECT_TRUE(result);
    EXPECT_NEAR(0.35355339, result->Intensity(1.0), 0.0001);
  });
}

TEST(PathIntegratorTest, SpecularBounceRouletteFails) {
  iris::reflectors::MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(testing::_))
      .WillRepeatedly(testing::Return(static_cast<iris::visual_t>(0.5)));

  iris::albedo_matchers::MockAlbedoMatcher albedo_matcher;
  EXPECT_CALL(albedo_matcher, Match(testing::_))
      .WillRepeatedly(testing::Return(static_cast<iris::visual_t>(0.5)));

  iris::bxdfs::MockBxdf specular;
  EXPECT_CALL(specular, IsDiffuse(testing::IsNull()))
      .WillRepeatedly(testing::Return(false));
  EXPECT_CALL(specular, Sample(testing::_, testing::_, testing::_, testing::_))
      .WillRepeatedly(testing::Return(iris::Bxdf::SampleResult{
          iris::Vector(0.0, 0.0, -1.0), std::nullopt, &specular, 1.0}));
  EXPECT_CALL(specular, Pdf(testing::_, testing::_, testing::_, testing::_))
      .WillRepeatedly(testing::Return(1.0));
  EXPECT_CALL(specular,
              Reflectance(testing::_, testing::_, testing::_, testing::_))
      .WillRepeatedly(testing::Return(&reflector));

  iris::spectra::MockSpectrum spectrum;
  EXPECT_CALL(spectrum, Intensity(testing::_))
      .WillRepeatedly(testing::Return(static_cast<iris::visual_t>(1.0)));

  iris::random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2)).Times(1);
  EXPECT_CALL(rng, NextVisual()).WillOnce(testing::Return(0.6));

  iris::testing::RayTracerPathNode path[] = {
      {1.0, nullptr, &specular, iris::Vector(0.0, 0.0, -1.0),
       iris::Vector(0.0, 0.0, -1.0)},
      {1.0, &spectrum, nullptr, iris::Vector(0.0, 0.0, -1.0),
       iris::Vector(0.0, 0.0, -1.0)}};

  iris::integrators::PathIntegrator integrator(1.0, 1.0, 0u, 1u);
  iris::testing::ScopedHitsRayTracer(nullptr, path, [&](auto& ray_tracer) {
    auto* result = integrator.Integrate(
        kTraceRay, ray_tracer, iris::testing::GetEmptyLightSampler(),
        iris::testing::GetAlwaysVisibleVisibilityTester(), albedo_matcher,
        iris::testing::GetSpectralAllocator(), rng);
    EXPECT_EQ(nullptr, result);
  });
}

TEST(PathIntegratorTest, OneSpecularBounceRoulettePasses) {
  iris::reflectors::MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(testing::_))
      .WillRepeatedly(testing::Return(static_cast<iris::visual_t>(0.5)));

  iris::albedo_matchers::MockAlbedoMatcher albedo_matcher;
  EXPECT_CALL(albedo_matcher, Match(testing::_))
      .WillRepeatedly(testing::Return(static_cast<iris::visual_t>(0.5)));

  iris::bxdfs::MockBxdf specular;
  EXPECT_CALL(specular, IsDiffuse(testing::IsNull()))
      .WillRepeatedly(testing::Return(false));
  EXPECT_CALL(specular, Sample(testing::_, testing::_, testing::_, testing::_))
      .WillRepeatedly(testing::Return(iris::Bxdf::SampleResult{
          iris::Vector(0.0, 0.0, -1.0), std::nullopt, &specular, 1.0}));
  EXPECT_CALL(specular, Pdf(testing::_, testing::_, testing::_, testing::_))
      .WillRepeatedly(testing::Return(1.0));
  EXPECT_CALL(specular,
              Reflectance(testing::_, testing::_, testing::_, testing::_))
      .WillRepeatedly(testing::Return(&reflector));

  iris::spectra::MockSpectrum spectrum;
  EXPECT_CALL(spectrum, Intensity(testing::_))
      .WillRepeatedly(testing::Return(static_cast<iris::visual_t>(1.0)));

  iris::random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2)).Times(1);
  EXPECT_CALL(rng, NextVisual()).WillOnce(testing::Return(0.4));

  iris::testing::RayTracerPathNode path[] = {
      {1.0, nullptr, &specular, iris::Vector(0.0, 0.0, -1.0),
       iris::Vector(0.0, 0.0, -1.0)},
      {1.0, &spectrum, nullptr, iris::Vector(0.0, 0.0, -1.0),
       iris::Vector(0.0, 0.0, -1.0)}};

  iris::integrators::PathIntegrator integrator(1.0, 1.0, 0u, 1u);
  iris::testing::ScopedHitsRayTracer(nullptr, path, [&](auto& ray_tracer) {
    auto* result = integrator.Integrate(
        kTraceRay, ray_tracer, iris::testing::GetEmptyLightSampler(),
        iris::testing::GetAlwaysVisibleVisibilityTester(), albedo_matcher,
        iris::testing::GetSpectralAllocator(), rng);
    ASSERT_TRUE(result);
    EXPECT_EQ(1.0, result->Intensity(1.0));
  });
}

TEST(PathIntegratorTest, TwoSpecularBounceRoulettePasses) {
  iris::reflectors::MockReflector reflector0;
  EXPECT_CALL(reflector0, Reflectance(testing::_))
      .WillRepeatedly(testing::Return(0.4));

  iris::albedo_matchers::MockAlbedoMatcher albedo_matcher;
  EXPECT_CALL(albedo_matcher, Match(testing::Ref(reflector0)))
      .WillRepeatedly(testing::Return(0.4));

  iris::reflectors::MockReflector reflector1;
  EXPECT_CALL(reflector1, Reflectance(testing::_))
      .WillRepeatedly(testing::Return(0.01));

  EXPECT_CALL(albedo_matcher, Match(testing::Ref(reflector1)))
      .WillRepeatedly(testing::Return(0.01));

  iris::bxdfs::MockBxdf specular0, specular1;
  EXPECT_CALL(specular0, IsDiffuse(testing::IsNull()))
      .WillRepeatedly(testing::Return(false));
  EXPECT_CALL(specular1, IsDiffuse(testing::IsNull()))
      .WillRepeatedly(testing::Return(false));
  EXPECT_CALL(specular0, Sample(testing::_, testing::_, testing::_, testing::_))
      .WillRepeatedly(testing::Return(iris::Bxdf::SampleResult{
          iris::Vector(0.0, 0.0, -1.0), std::nullopt, &specular0, 1.0}));
  EXPECT_CALL(specular1, Sample(testing::_, testing::_, testing::_, testing::_))
      .WillRepeatedly(testing::Return(iris::Bxdf::SampleResult{
          iris::Vector(0.0, 0.0, -1.0), std::nullopt, &specular1, 1.0}));
  EXPECT_CALL(specular0, Pdf(testing::_, testing::_, testing::_, testing::_))
      .WillRepeatedly(testing::Return(1.0));
  EXPECT_CALL(specular1, Pdf(testing::_, testing::_, testing::_, testing::_))
      .WillRepeatedly(testing::Return(1.0));
  EXPECT_CALL(specular0,
              Reflectance(testing::_, testing::_, testing::_, testing::_))
      .WillRepeatedly(testing::Return(&reflector0));
  EXPECT_CALL(specular1,
              Reflectance(testing::_, testing::_, testing::_, testing::_))
      .WillRepeatedly(testing::Return(&reflector1));

  iris::spectra::MockSpectrum spectrum;
  EXPECT_CALL(spectrum, Intensity(testing::_))
      .WillRepeatedly(testing::Return(static_cast<iris::visual_t>(1.0)));

  iris::random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2)).Times(2);
  EXPECT_CALL(rng, NextVisual())
      .WillRepeatedly(testing::Return(static_cast<iris::visual_t>(0.0)));

  iris::testing::RayTracerPathNode path[] = {
      {1.0, nullptr, &specular0, iris::Vector(0.0, 0.0, -1.0),
       iris::Vector(0.0, 0.0, -1.0)},
      {1.0, nullptr, &specular1, iris::Vector(0.0, 0.0, -1.0),
       iris::Vector(0.0, 0.0, -1.0)},
      {1.0, &spectrum, nullptr, iris::Vector(0.0, 0.0, -1.0),
       iris::Vector(0.0, 0.0, -1.0)}};

  iris::integrators::PathIntegrator integrator(0.2, 1.0, 0u, 3u);
  iris::testing::ScopedHitsRayTracer(nullptr, path, [&](auto& ray_tracer) {
    auto* result = integrator.Integrate(
        kTraceRay, ray_tracer, iris::testing::GetEmptyLightSampler(),
        iris::testing::GetAlwaysVisibleVisibilityTester(), albedo_matcher,
        iris::testing::GetSpectralAllocator(), rng);
    ASSERT_TRUE(result);
    EXPECT_NEAR(1.0, result->Intensity(1.0), 0.001);
  });
}

TEST(PathIntegratorTest, DirectLighting) {
  iris::reflectors::MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(testing::_))
      .WillRepeatedly(testing::Return(static_cast<iris::visual_t>(0.5)));

  iris::albedo_matchers::MockAlbedoMatcher albedo_matcher;

  iris::Vector to_light(0.0, 0.0, -1.0);

  iris::bxdfs::MockBxdf diffuse;
  EXPECT_CALL(diffuse, IsDiffuse(testing::IsNull()))
      .WillRepeatedly(testing::Return(true));
  EXPECT_CALL(diffuse, Sample(testing::_, testing::_, testing::_, testing::_))
      .WillRepeatedly(testing::Return(
          iris::Bxdf::SampleResult{to_light, std::nullopt, &diffuse, 1.0}));
  EXPECT_CALL(diffuse, Pdf(testing::_, testing::_, testing::_, testing::_))
      .WillRepeatedly(testing::Return(static_cast<iris::visual_t>(1.0)));
  EXPECT_CALL(diffuse,
              Reflectance(testing::_, testing::_, testing::_, testing::_))
      .WillRepeatedly(testing::Return(&reflector));

  iris::spectra::MockSpectrum spectrum;
  EXPECT_CALL(spectrum, Intensity(testing::_))
      .WillRepeatedly(testing::Return(static_cast<iris::visual_t>(1.0)));

  iris::Light::SampleResult light_sample{spectrum, to_light, std::nullopt};

  iris::lights::MockLight light;
  EXPECT_CALL(light, Sample(testing::_, testing::_, testing::_, testing::_))
      .WillRepeatedly(testing::Return(light_sample));

  iris::random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2)).Times(2);

  iris::testing::RayTracerPathNode path[] = {{1.0, nullptr, &diffuse,
                                              iris::Vector(0.0, 0.0, -1.0),
                                              iris::Vector(0.0, 0.0, -1.0)}};

  iris::integrators::PathIntegrator integrator0(1.0, 1.0, 0u, 0u);
  iris::testing::ScopedHitsRayTracer(nullptr, path, [&](auto& ray_tracer) {
    iris::testing::LightSampleListEntry list[] = {
        {&light, static_cast<iris::visual_t>(0.25)},
    };

    iris::testing::ScopedListLightSampler(list, [&](auto& light_sampler) {
      auto* result = integrator0.Integrate(
          kTraceRay, ray_tracer, light_sampler,
          iris::testing::GetAlwaysVisibleVisibilityTester(), albedo_matcher,
          iris::testing::GetSpectralAllocator(), rng);
      EXPECT_TRUE(result);
      EXPECT_EQ(2.0, result->Intensity(1.0));
    });
  });
}