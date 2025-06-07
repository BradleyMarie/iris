#include "iris/integrators/path_integrator.h"

#include <memory>
#include <optional>
#include <variant>

#include "googletest/include/gtest/gtest.h"
#include "iris/albedo_matchers/mock_albedo_matcher.h"
#include "iris/bxdf.h"
#include "iris/bxdfs/mock_bxdf.h"
#include "iris/environmental_lights/mock_environmental_light.h"
#include "iris/float.h"
#include "iris/integrator.h"
#include "iris/lights/mock_light.h"
#include "iris/point.h"
#include "iris/random/mock_random.h"
#include "iris/ray.h"
#include "iris/ray_differential.h"
#include "iris/reflectors/mock_reflector.h"
#include "iris/spectra/mock_spectrum.h"
#include "iris/spectrum.h"
#include "iris/testing/light_sampler.h"
#include "iris/testing/ray_tracer.h"
#include "iris/testing/spectral_allocator.h"
#include "iris/testing/visibility_tester.h"
#include "iris/vector.h"

namespace iris {
namespace integrators {
namespace {

using ::iris::albedo_matchers::MockAlbedoMatcher;
using ::iris::bxdfs::MockBxdf;
using ::iris::environmental_lights::MockEnvironmentalLight;
using ::iris::lights::MockLight;
using ::iris::random::MockRandom;
using ::iris::reflectors::MockReflector;
using ::iris::spectra::MockSpectrum;
using ::iris::testing::GetAlwaysVisibleVisibilityTester;
using ::iris::testing::GetEmptyLightSampler;
using ::iris::testing::GetNoHitsRayTracer;
using ::iris::testing::GetSpectralAllocator;
using ::iris::testing::LightSampleListEntry;
using ::iris::testing::RayTracerPathNode;
using ::iris::testing::ScopedHitsRayTracer;
using ::iris::testing::ScopedListLightSampler;
using ::iris::testing::ScopedNoHitsRayTracer;
using ::testing::_;
using ::testing::DoAll;
using ::testing::NotNull;
using ::testing::Ref;
using ::testing::Return;
using ::testing::SetArgPointee;

constexpr RayDifferential kTraceRay(Ray(Point(0.0, 0.0, 1.0),
                                        Vector(0.0, 0.0, 1.0)));

TEST(PathIntegratorTest, NoHits) {
  MockAlbedoMatcher albedo_matcher;
  MockRandom rng;

  std::unique_ptr<Integrator> integrator = MakePathIntegrator(1.0, 1.0, 1u, 8u);
  EXPECT_EQ(nullptr,
            integrator->Integrate(kTraceRay, GetNoHitsRayTracer(),
                                  GetEmptyLightSampler(),
                                  GetAlwaysVisibleVisibilityTester(),
                                  albedo_matcher, GetSpectralAllocator(), rng));
}

TEST(PathIntegratorTest, HitsEmissiveWithNoReflectance) {
  MockAlbedoMatcher albedo_matcher;

  MockSpectrum spectrum;
  MockEnvironmentalLight environmental_light;
  EXPECT_CALL(environmental_light, Emission(_, _, _))
      .WillOnce(Return(&spectrum));

  MockRandom rng;

  std::unique_ptr<Integrator> integrator = MakePathIntegrator(1.0, 1.0, 1u, 8u);
  ScopedNoHitsRayTracer(environmental_light, [&](RayTracer& ray_tracer) {
    EXPECT_EQ(&spectrum, integrator->Integrate(
                             kTraceRay, ray_tracer, GetEmptyLightSampler(),
                             GetAlwaysVisibleVisibilityTester(), albedo_matcher,
                             GetSpectralAllocator(), rng));
  });
}

TEST(PathIntegratorTest, BounceLimit) {
  MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(_))
      .WillRepeatedly(Return(static_cast<visual_t>(0.5)));

  MockAlbedoMatcher albedo_matcher;
  EXPECT_CALL(albedo_matcher, Match(_))
      .WillRepeatedly(Return(static_cast<visual_t>(0.5)));

  MockBxdf specular;
  EXPECT_CALL(specular, IsDiffuse(NotNull()))
      .WillRepeatedly(DoAll(SetArgPointee<0>(0.0), Return(false)));
  EXPECT_CALL(specular, Sample(_, _, _, _, _))
      .WillRepeatedly(Return(
          Bxdf::SpecularSample{Bxdf::Hemisphere::BTDF, Vector(0.0, 0.0, -1.0),
                               &reflector, std::nullopt, 1.0}));

  MockSpectrum spectrum;
  EXPECT_CALL(spectrum, Intensity(_))
      .WillRepeatedly(Return(static_cast<visual_t>(1.0)));

  MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2)).Times(1);

  RayTracerPathNode path[] = {
      {1.0, nullptr, &specular, Vector(0.0, 0.0, -1.0), Vector(0.0, 0.0, -1.0)},
      {1.0, &spectrum, nullptr, Vector(0.0, 0.0, -1.0),
       Vector(0.0, 0.0, -1.0)}};

  std::unique_ptr<Integrator> integrator0 =
      MakePathIntegrator(1.0, 1.0, 0u, 0u);
  ScopedHitsRayTracer(nullptr, path, [&](RayTracer& ray_tracer) {
    EXPECT_EQ(nullptr, integrator0->Integrate(
                           kTraceRay, ray_tracer, GetEmptyLightSampler(),
                           GetAlwaysVisibleVisibilityTester(), albedo_matcher,
                           GetSpectralAllocator(), rng));
  });

  std::unique_ptr<Integrator> integrator1 =
      MakePathIntegrator(1.0, 1.0, 1u, 1u);
  ScopedHitsRayTracer(nullptr, path, [&](RayTracer& ray_tracer) {
    const Spectrum* result =
        integrator1->Integrate(kTraceRay, ray_tracer, GetEmptyLightSampler(),
                               GetAlwaysVisibleVisibilityTester(),
                               albedo_matcher, GetSpectralAllocator(), rng);
    EXPECT_TRUE(result);
    EXPECT_EQ(0.5, result->Intensity(1.0));
  });
}

TEST(PathIntegratorTest, NoBsdf) {
  MockAlbedoMatcher albedo_matcher;

  MockSpectrum spectrum;
  EXPECT_CALL(spectrum, Intensity(_))
      .WillRepeatedly(Return(static_cast<visual_t>(1.0)));

  MockRandom rng;

  RayTracerPathNode path[] = {
      {1.0, nullptr, nullptr, Vector(0.0, 0.0, -1.0), Vector(0.0, 0.0, -1.0)},
      {1.0, &spectrum, nullptr, Vector(0.0, 0.0, -1.0),
       Vector(0.0, 0.0, -1.0)}};

  std::unique_ptr<Integrator> integrator = MakePathIntegrator(1.0, 1.0, 3u, 3u);
  ScopedHitsRayTracer(nullptr, path, [&](RayTracer& ray_tracer) {
    EXPECT_EQ(nullptr, integrator->Integrate(
                           kTraceRay, ray_tracer, GetEmptyLightSampler(),
                           GetAlwaysVisibleVisibilityTester(), albedo_matcher,
                           GetSpectralAllocator(), rng));
  });
}

TEST(PathIntegratorTest, BsdfSampleFails) {
  MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(_))
      .WillRepeatedly(Return(static_cast<visual_t>(0.5)));

  MockAlbedoMatcher albedo_matcher;
  EXPECT_CALL(albedo_matcher, Match(_))
      .WillRepeatedly(Return(static_cast<visual_t>(0.5)));

  MockBxdf specular;
  EXPECT_CALL(specular, IsDiffuse(NotNull()))
      .WillRepeatedly(DoAll(SetArgPointee<0>(0.0), Return(false)));
  EXPECT_CALL(specular, Sample(_, _, _, _, _))
      .WillRepeatedly(Return(std::monostate()));

  MockSpectrum spectrum;
  EXPECT_CALL(spectrum, Intensity(_))
      .WillRepeatedly(Return(static_cast<visual_t>(1.0)));

  MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2)).Times(1);

  RayTracerPathNode path[] = {
      {1.0, nullptr, &specular, Vector(0.0, 0.0, -1.0), Vector(0.0, 0.0, -1.0)},
      {1.0, &spectrum, nullptr, Vector(0.0, 0.0, -1.0),
       Vector(0.0, 0.0, -1.0)}};

  std::unique_ptr<Integrator> integrator = MakePathIntegrator(1.0, 1.0, 3u, 3u);
  ScopedHitsRayTracer(nullptr, path, [&](RayTracer& ray_tracer) {
    EXPECT_EQ(nullptr, integrator->Integrate(
                           kTraceRay, ray_tracer, GetEmptyLightSampler(),
                           GetAlwaysVisibleVisibilityTester(), albedo_matcher,
                           GetSpectralAllocator(), rng));
  });
}

TEST(PathIntegratorTest, TwoSpecularBouncesHitsEmissive) {
  MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(_))
      .WillRepeatedly(Return(static_cast<visual_t>(0.5)));

  MockAlbedoMatcher albedo_matcher;
  EXPECT_CALL(albedo_matcher, Match(_))
      .WillRepeatedly(Return(static_cast<visual_t>(0.5)));

  MockBxdf specular0, specular1;
  EXPECT_CALL(specular0, IsDiffuse(NotNull()))
      .WillRepeatedly(DoAll(SetArgPointee<0>(0.0), Return(false)));
  EXPECT_CALL(specular1, IsDiffuse(NotNull()))
      .WillRepeatedly(DoAll(SetArgPointee<0>(0.0), Return(false)));
  EXPECT_CALL(specular0, Sample(_, _, _, _, _))
      .WillRepeatedly(Return(
          Bxdf::SpecularSample{Bxdf::Hemisphere::BTDF, Vector(0.0, 0.0, -1.0),
                               &reflector, std::nullopt, 1.0}));
  EXPECT_CALL(specular1, Sample(_, _, _, _, _))
      .WillRepeatedly(Return(
          Bxdf::SpecularSample{Bxdf::Hemisphere::BTDF, Vector(0.0, 0.0, -1.0),
                               &reflector, std::nullopt, 1.0}));

  MockSpectrum spectrum;
  EXPECT_CALL(spectrum, Intensity(_))
      .WillRepeatedly(Return(static_cast<visual_t>(1.0)));

  MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2)).Times(2);

  RayTracerPathNode path[] = {{1.0, nullptr, &specular0, Vector(0.0, 0.0, -1.0),
                               Vector(0.0, 0.0, -1.0)},
                              {1.0, nullptr, &specular1, Vector(0.0, 0.0, -1.0),
                               Vector(0.0, 0.0, -1.0)},
                              {1.0, &spectrum, nullptr, Vector(0.0, 0.0, -1.0),
                               Vector(0.0, 0.0, -1.0)}};

  std::unique_ptr<Integrator> integrator = MakePathIntegrator(1.0, 1.0, 4u, 8u);
  ScopedHitsRayTracer(nullptr, path, [&](RayTracer& ray_tracer) {
    const Spectrum* result =
        integrator->Integrate(kTraceRay, ray_tracer, GetEmptyLightSampler(),
                              GetAlwaysVisibleVisibilityTester(),
                              albedo_matcher, GetSpectralAllocator(), rng);
    EXPECT_TRUE(result);
    EXPECT_EQ(0.25, result->Intensity(1.0));
  });
}

TEST(PathIntegratorTest, DiffuseBounceToSpecularBounceToEmissive) {
  MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(_))
      .WillRepeatedly(Return(static_cast<visual_t>(0.5)));

  MockAlbedoMatcher albedo_matcher;
  EXPECT_CALL(albedo_matcher, Match(_))
      .WillRepeatedly(Return(static_cast<visual_t>(0.5)));

  MockBxdf diffuse, specular;
  EXPECT_CALL(diffuse, IsDiffuse(NotNull()))
      .WillRepeatedly(DoAll(SetArgPointee<0>(1.0), Return(true)));
  EXPECT_CALL(specular, IsDiffuse(NotNull()))
      .WillRepeatedly(DoAll(SetArgPointee<0>(0.0), Return(false)));
  EXPECT_CALL(diffuse, Sample(_, _, _, _, _))
      .WillRepeatedly(
          Return(Bxdf::DiffuseSample{Normalize(Vector(1.0, 0.0, -1.0))}));
  EXPECT_CALL(specular, Sample(_, _, _, _, _))
      .WillRepeatedly(Return(
          Bxdf::SpecularSample{Bxdf::Hemisphere::BTDF, Vector(0.0, 0.0, -1.0),
                               &reflector, std::nullopt, 1.0}));
  EXPECT_CALL(diffuse, PdfDiffuse(_, _, _, _))
      .WillRepeatedly(Return(static_cast<visual_t>(0.5)));
  EXPECT_CALL(diffuse, ReflectanceDiffuse(_, _, _, _))
      .WillRepeatedly(Return(&reflector));

  MockSpectrum spectrum;
  EXPECT_CALL(spectrum, Intensity(_))
      .WillRepeatedly(Return(static_cast<visual_t>(1.0)));

  MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2)).Times(2);

  RayTracerPathNode path[] = {
      {1.0, nullptr, &diffuse, Vector(0.0, 0.0, -1.0), Vector(0.0, 0.0, -1.0)},
      {1.0, nullptr, &specular, Vector(0.0, 0.0, -1.0), Vector(0.0, 0.0, -1.0)},
      {1.0, &spectrum, nullptr, Vector(0.0, 0.0, -1.0),
       Vector(0.0, 0.0, -1.0)}};

  std::unique_ptr<Integrator> integrator = MakePathIntegrator(1.0, 1.0, 4u, 8u);
  ScopedHitsRayTracer(nullptr, path, [&](RayTracer& ray_tracer) {
    const Spectrum* result =
        integrator->Integrate(kTraceRay, ray_tracer, GetEmptyLightSampler(),
                              GetAlwaysVisibleVisibilityTester(),
                              albedo_matcher, GetSpectralAllocator(), rng);
    EXPECT_TRUE(result);
    EXPECT_NEAR(0.35355339, result->Intensity(1.0), 0.0001);
  });
}

TEST(PathIntegratorTest, SpecularBounceRouletteFails) {
  MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(_))
      .WillRepeatedly(Return(static_cast<visual_t>(0.5)));

  MockAlbedoMatcher albedo_matcher;
  EXPECT_CALL(albedo_matcher, Match(_))
      .WillRepeatedly(Return(static_cast<visual_t>(0.5)));

  MockBxdf specular;
  EXPECT_CALL(specular, IsDiffuse(NotNull()))
      .WillRepeatedly(DoAll(SetArgPointee<0>(0.0), Return(false)));
  EXPECT_CALL(specular, Sample(_, _, _, _, _))
      .WillRepeatedly(Return(
          Bxdf::SpecularSample{Bxdf::Hemisphere::BTDF, Vector(0.0, 0.0, -1.0),
                               &reflector, std::nullopt, 1.0}));

  MockSpectrum spectrum;
  EXPECT_CALL(spectrum, Intensity(_))
      .WillRepeatedly(Return(static_cast<visual_t>(1.0)));

  MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2)).Times(1);
  EXPECT_CALL(rng, NextVisual()).WillOnce(Return(0.6));

  RayTracerPathNode path[] = {
      {1.0, nullptr, &specular, Vector(0.0, 0.0, -1.0), Vector(0.0, 0.0, -1.0)},
      {1.0, &spectrum, nullptr, Vector(0.0, 0.0, -1.0),
       Vector(0.0, 0.0, -1.0)}};

  std::unique_ptr<Integrator> integrator = MakePathIntegrator(1.0, 1.0, 0u, 1u);
  ScopedHitsRayTracer(nullptr, path, [&](RayTracer& ray_tracer) {
    const Spectrum* result =
        integrator->Integrate(kTraceRay, ray_tracer, GetEmptyLightSampler(),
                              GetAlwaysVisibleVisibilityTester(),
                              albedo_matcher, GetSpectralAllocator(), rng);
    EXPECT_EQ(nullptr, result);
  });
}

TEST(PathIntegratorTest, OneSpecularBounceRoulettePasses) {
  MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(_))
      .WillRepeatedly(Return(static_cast<visual_t>(0.5)));

  MockAlbedoMatcher albedo_matcher;
  EXPECT_CALL(albedo_matcher, Match(_))
      .WillRepeatedly(Return(static_cast<visual_t>(0.5)));

  MockBxdf specular;
  EXPECT_CALL(specular, IsDiffuse(NotNull()))
      .WillRepeatedly(DoAll(SetArgPointee<0>(0.0), Return(false)));
  EXPECT_CALL(specular, Sample(_, _, _, _, _))
      .WillRepeatedly(Return(
          Bxdf::SpecularSample{Bxdf::Hemisphere::BTDF, Vector(0.0, 0.0, -1.0),
                               &reflector, std::nullopt, 1.0}));

  MockSpectrum spectrum;
  EXPECT_CALL(spectrum, Intensity(_))
      .WillRepeatedly(Return(static_cast<visual_t>(1.0)));

  MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2)).Times(1);
  EXPECT_CALL(rng, NextVisual()).WillOnce(Return(0.4));

  RayTracerPathNode path[] = {
      {1.0, nullptr, &specular, Vector(0.0, 0.0, -1.0), Vector(0.0, 0.0, -1.0)},
      {1.0, &spectrum, nullptr, Vector(0.0, 0.0, -1.0),
       Vector(0.0, 0.0, -1.0)}};

  std::unique_ptr<Integrator> integrator = MakePathIntegrator(1.0, 1.0, 0u, 1u);
  ScopedHitsRayTracer(nullptr, path, [&](RayTracer& ray_tracer) {
    const Spectrum* result =
        integrator->Integrate(kTraceRay, ray_tracer, GetEmptyLightSampler(),
                              GetAlwaysVisibleVisibilityTester(),
                              albedo_matcher, GetSpectralAllocator(), rng);
    ASSERT_TRUE(result);
    EXPECT_EQ(1.0, result->Intensity(1.0));
  });
}

TEST(PathIntegratorTest, TwoSpecularBounceRoulettePasses) {
  MockReflector reflector0;
  EXPECT_CALL(reflector0, Reflectance(_)).WillRepeatedly(Return(0.4));

  MockAlbedoMatcher albedo_matcher;
  EXPECT_CALL(albedo_matcher, Match(Ref(reflector0)))
      .WillRepeatedly(Return(0.4));

  MockReflector reflector1;
  EXPECT_CALL(reflector1, Reflectance(_)).WillRepeatedly(Return(0.01));

  EXPECT_CALL(albedo_matcher, Match(Ref(reflector1)))
      .WillRepeatedly(Return(0.01));

  MockBxdf specular0, specular1;
  EXPECT_CALL(specular0, IsDiffuse(NotNull()))
      .WillRepeatedly(DoAll(SetArgPointee<0>(0.0), Return(false)));
  EXPECT_CALL(specular1, IsDiffuse(NotNull()))
      .WillRepeatedly(DoAll(SetArgPointee<0>(0.0), Return(false)));
  EXPECT_CALL(specular0, Sample(_, _, _, _, _))
      .WillRepeatedly(Return(
          Bxdf::SpecularSample{Bxdf::Hemisphere::BTDF, Vector(0.0, 0.0, -1.0),
                               &reflector0, std::nullopt, 1.0}));
  EXPECT_CALL(specular1, Sample(_, _, _, _, _))
      .WillRepeatedly(Return(
          Bxdf::SpecularSample{Bxdf::Hemisphere::BTDF, Vector(0.0, 0.0, -1.0),
                               &reflector1, std::nullopt, 1.0}));

  MockSpectrum spectrum;
  EXPECT_CALL(spectrum, Intensity(_))
      .WillRepeatedly(Return(static_cast<visual_t>(1.0)));

  MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2)).Times(2);
  EXPECT_CALL(rng, NextVisual())
      .WillRepeatedly(Return(static_cast<visual_t>(0.0)));

  RayTracerPathNode path[] = {{1.0, nullptr, &specular0, Vector(0.0, 0.0, -1.0),
                               Vector(0.0, 0.0, -1.0)},
                              {1.0, nullptr, &specular1, Vector(0.0, 0.0, -1.0),
                               Vector(0.0, 0.0, -1.0)},
                              {1.0, &spectrum, nullptr, Vector(0.0, 0.0, -1.0),
                               Vector(0.0, 0.0, -1.0)}};

  std::unique_ptr<Integrator> integrator = MakePathIntegrator(0.2, 1.0, 0u, 3u);
  ScopedHitsRayTracer(nullptr, path, [&](RayTracer& ray_tracer) {
    const Spectrum* result =
        integrator->Integrate(kTraceRay, ray_tracer, GetEmptyLightSampler(),
                              GetAlwaysVisibleVisibilityTester(),
                              albedo_matcher, GetSpectralAllocator(), rng);
    ASSERT_TRUE(result);
    EXPECT_NEAR(1.0, result->Intensity(1.0), 0.001);
  });
}

TEST(PathIntegratorTest, DirectLighting) {
  MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(_))
      .WillRepeatedly(Return(static_cast<visual_t>(0.5)));

  MockAlbedoMatcher albedo_matcher;

  Vector to_light(0.0, 0.0, -1.0);

  MockBxdf diffuse;
  EXPECT_CALL(diffuse, IsDiffuse(NotNull()))
      .WillRepeatedly(DoAll(SetArgPointee<0>(1.0), Return(true)));
  EXPECT_CALL(diffuse, Sample(_, _, _, _, _))
      .WillRepeatedly(Return(Bxdf::DiffuseSample{to_light}));
  EXPECT_CALL(diffuse, PdfDiffuse(_, _, _, _))
      .WillRepeatedly(Return(static_cast<visual_t>(1.0)));
  EXPECT_CALL(diffuse, ReflectanceDiffuse(_, _, _, _))
      .WillRepeatedly(Return(&reflector));

  MockSpectrum spectrum;
  EXPECT_CALL(spectrum, Intensity(_))
      .WillRepeatedly(Return(static_cast<visual_t>(1.0)));

  Light::SampleResult light_sample{spectrum, to_light, std::nullopt};

  MockLight light;
  EXPECT_CALL(light, Sample(_, _, _, _)).WillRepeatedly(Return(light_sample));

  MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2)).Times(2);

  RayTracerPathNode path[] = {
      {1.0, nullptr, &diffuse, Vector(0.0, 0.0, -1.0), Vector(0.0, 0.0, -1.0)}};

  std::unique_ptr<Integrator> integrator = MakePathIntegrator(1.0, 1.0, 0u, 0u);
  ScopedHitsRayTracer(nullptr, path, [&](RayTracer& ray_tracer) {
    LightSampleListEntry list[] = {
        {&light, static_cast<visual_t>(0.25)},
    };

    ScopedListLightSampler(list, [&](LightSampler& light_sampler) {
      const Spectrum* result =
          integrator->Integrate(kTraceRay, ray_tracer, light_sampler,
                                GetAlwaysVisibleVisibilityTester(),
                                albedo_matcher, GetSpectralAllocator(), rng);
      EXPECT_TRUE(result);
      EXPECT_EQ(2.0, result->Intensity(1.0));
    });
  });
}

}  // namespace
}  // namespace integrators
}  // namespace iris
