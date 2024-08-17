#include "iris/integrators/internal/sample_indirect_lighting.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/bxdfs/mock_bxdf.h"
#include "iris/random/mock_random.h"
#include "iris/reflectors/mock_reflector.h"
#include "iris/testing/spectral_allocator.h"

static const iris::PositionError kError = iris::PositionError(0.0, 0.0, 0.0);

static const iris::Point kOrigin = iris::Point(0.0, 0.0, 0.0);
static const iris::Point kDxOrigin = iris::Point(1.0, 0.0, 0.0);
static const iris::Point kDyOrigin = iris::Point(0.0, 1.0, 0.0);
static const iris::Vector kDirection =
    iris::Normalize(iris::Vector(0.0, 0.0, 1.0));

static const iris::Vector kOutgoing =
    iris::Normalize(iris::Vector(0.0, 0.0, -1.0));
static const iris::Vector kSurfaceNormal =
    iris::Normalize(iris::Vector(0.0, 0.0, -1.0));

TEST(SampleIndirectLighting, NoSample) {
  iris::random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2)).Times(1);

  iris::bxdfs::MockBxdf bxdf;
  EXPECT_CALL(bxdf, IsDiffuse()).WillRepeatedly(testing::Return(false));
  EXPECT_CALL(bxdf, Sample(testing::_, testing::Eq(std::nullopt), testing::_,
                           testing::_))
      .WillOnce(testing::Return(std::nullopt));

  iris::Bsdf bsdf(bxdf, kSurfaceNormal, kSurfaceNormal);

  iris::RayTracer::SurfaceIntersection intersection{
      bsdf, iris::HitPoint(kOrigin, kError, kSurfaceNormal), std::nullopt,
      kSurfaceNormal, kSurfaceNormal};

  iris::RayDifferential initial_ray(iris::Ray(kOrigin, kDirection));
  iris::RayDifferential actual_ray(iris::Ray(kOrigin, kDirection));

  EXPECT_FALSE(iris::integrators::internal::SampleIndirectLighting(
      intersection, iris::Sampler(rng), iris::testing::GetSpectralAllocator(),
      actual_ray));
  EXPECT_EQ(initial_ray, actual_ray);
}

TEST(SampleIndirectLighting, Sample) {
  iris::reflectors::MockReflector reflector;

  iris::random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2)).Times(1);

  iris::bxdfs::MockBxdf bxdf;
  EXPECT_CALL(bxdf, IsDiffuse()).WillRepeatedly(testing::Return(false));
  EXPECT_CALL(bxdf, Sample(testing::_, testing::Eq(std::nullopt), testing::_,
                           testing::_))
      .WillRepeatedly(testing::Return(
          iris::Bxdf::SampleResult{kOutgoing, std::nullopt, &bxdf, 1.0}));
  EXPECT_CALL(bxdf,
              Pdf(testing::_, testing::_, testing::_, testing::_, testing::_))
      .WillRepeatedly(testing::Return(std::nullopt));
  EXPECT_CALL(bxdf, Reflectance(testing::_, testing::_, testing::_, testing::_,
                                testing::_))
      .WillRepeatedly(testing::Return(&reflector));

  iris::Bsdf bsdf(bxdf, kSurfaceNormal, kSurfaceNormal);

  iris::RayTracer::SurfaceIntersection intersection{
      bsdf, iris::HitPoint(kOrigin, kError, kSurfaceNormal), std::nullopt,
      kSurfaceNormal, kSurfaceNormal};

  iris::RayDifferential actual_ray(iris::Ray(kOrigin, kDirection));
  auto indirect = iris::integrators::internal::SampleIndirectLighting(
      intersection, iris::Sampler(rng), iris::testing::GetSpectralAllocator(),
      actual_ray);
  ASSERT_TRUE(indirect);
  EXPECT_EQ(&reflector, &indirect->reflector);

  iris::RayDifferential expected_ray(
      iris::Ray(iris::Point(std::nextafter(static_cast<iris::geometric>(0.0),
                                           static_cast<iris::geometric>(-2.0)),
                            std::nextafter(static_cast<iris::geometric>(0.0),
                                           static_cast<iris::geometric>(-2.0)),
                            std::nextafter(static_cast<iris::geometric>(0.0),
                                           static_cast<iris::geometric>(2.0))),
                kDirection));
  EXPECT_EQ(expected_ray, actual_ray);
}

TEST(SampleIndirectLighting, SampleWithOnlyRayDifferentials) {
  iris::reflectors::MockReflector reflector;

  iris::random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2)).Times(1);

  iris::bxdfs::MockBxdf bxdf;
  EXPECT_CALL(bxdf, IsDiffuse()).WillRepeatedly(testing::Return(true));
  EXPECT_CALL(bxdf, Sample(testing::_, testing::Eq(std::nullopt), testing::_,
                           testing::_))
      .WillRepeatedly(testing::Return(iris::Bxdf::SampleResult{
          kOutgoing, {{kOutgoing, kOutgoing}}, &bxdf, 1.0}));
  EXPECT_CALL(bxdf,
              Pdf(testing::_, testing::_, testing::_, testing::_, testing::_))
      .WillRepeatedly(testing::Return(std::nullopt));
  EXPECT_CALL(bxdf, Reflectance(testing::_, testing::_, testing::_, testing::_,
                                testing::_))
      .WillRepeatedly(testing::Return(&reflector));

  iris::Bsdf bsdf(bxdf, kSurfaceNormal, kSurfaceNormal);

  iris::RayTracer::SurfaceIntersection intersection{
      bsdf, iris::HitPoint(kOrigin, kError, kSurfaceNormal), std::nullopt,
      kSurfaceNormal, kSurfaceNormal};

  iris::RayDifferential actual_ray(iris::Ray(kOrigin, kDirection),
                                   iris::Ray(kDxOrigin, kDirection),
                                   iris::Ray(kDyOrigin, kDirection));
  auto indirect = iris::integrators::internal::SampleIndirectLighting(
      intersection, iris::Sampler(rng), iris::testing::GetSpectralAllocator(),
      actual_ray);
  ASSERT_TRUE(indirect);
  EXPECT_EQ(&reflector, &indirect->reflector);
  EXPECT_TRUE(indirect->differentials);

  iris::RayDifferential expected_ray(
      iris::Ray(iris::Point(std::nextafter(static_cast<iris::geometric>(0.0),
                                           static_cast<iris::geometric>(-2.0)),
                            std::nextafter(static_cast<iris::geometric>(0.0),
                                           static_cast<iris::geometric>(-2.0)),
                            std::nextafter(static_cast<iris::geometric>(0.0),
                                           static_cast<iris::geometric>(2.0))),
                kDirection));
  EXPECT_EQ(expected_ray, actual_ray);
}

TEST(SampleIndirectLighting, SampleWithOnlyIntersection) {
  iris::reflectors::MockReflector reflector;

  iris::random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2)).Times(1);

  iris::bxdfs::MockBxdf bxdf;
  EXPECT_CALL(bxdf, IsDiffuse()).WillRepeatedly(testing::Return(true));
  EXPECT_CALL(bxdf, Sample(testing::_, testing::Eq(std::nullopt), testing::_,
                           testing::_))
      .WillRepeatedly(testing::Return(
          iris::Bxdf::SampleResult{kOutgoing, std::nullopt, &bxdf, 1.0}));
  EXPECT_CALL(bxdf,
              Pdf(testing::_, testing::_, testing::_, testing::_, testing::_))
      .WillRepeatedly(testing::Return(std::nullopt));
  EXPECT_CALL(bxdf, Reflectance(testing::_, testing::_, testing::_, testing::_,
                                testing::_))
      .WillRepeatedly(testing::Return(&reflector));

  iris::Bsdf bsdf(bxdf, kSurfaceNormal, kSurfaceNormal);

  iris::RayTracer::SurfaceIntersection intersection{
      bsdf, iris::HitPoint(kOrigin, kError, kSurfaceNormal),
      iris::RayTracer::Differentials{kDxOrigin, kDyOrigin}, kSurfaceNormal,
      kSurfaceNormal};

  iris::RayDifferential actual_ray(iris::Ray(kOrigin, kDirection));
  auto indirect = iris::integrators::internal::SampleIndirectLighting(
      intersection, iris::Sampler(rng), iris::testing::GetSpectralAllocator(),
      actual_ray);
  ASSERT_TRUE(indirect);
  EXPECT_EQ(&reflector, &indirect->reflector);
  EXPECT_FALSE(indirect->differentials);

  iris::RayDifferential expected_ray(
      iris::Ray(iris::Point(std::nextafter(static_cast<iris::geometric>(0.0),
                                           static_cast<iris::geometric>(-2.0)),
                            std::nextafter(static_cast<iris::geometric>(0.0),
                                           static_cast<iris::geometric>(-2.0)),
                            std::nextafter(static_cast<iris::geometric>(0.0),
                                           static_cast<iris::geometric>(2.0))),
                kDirection));
  EXPECT_EQ(expected_ray, actual_ray);
}

TEST(SampleIndirectLighting, SampleWithDifferentialsNoneReturned) {
  iris::reflectors::MockReflector reflector;

  iris::random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2)).Times(1);

  iris::bxdfs::MockBxdf bxdf;
  EXPECT_CALL(bxdf, IsDiffuse()).WillRepeatedly(testing::Return(true));
  EXPECT_CALL(bxdf, Sample(testing::_, testing::Not(testing::Eq(std::nullopt)),
                           testing::_, testing::_))
      .WillRepeatedly(testing::Return(
          iris::Bxdf::SampleResult{kOutgoing, std::nullopt, &bxdf, 1.0}));
  EXPECT_CALL(bxdf,
              Pdf(testing::_, testing::_, testing::_, testing::_, testing::_))
      .WillRepeatedly(testing::Return(std::nullopt));
  EXPECT_CALL(bxdf, Reflectance(testing::_, testing::_, testing::_, testing::_,
                                testing::_))
      .WillRepeatedly(testing::Return(&reflector));

  iris::Bsdf bsdf(bxdf, kSurfaceNormal, kSurfaceNormal);

  iris::RayTracer::SurfaceIntersection intersection{
      bsdf, iris::HitPoint(kOrigin, kError, kSurfaceNormal),
      iris::RayTracer::Differentials{kDxOrigin, kDyOrigin}, kSurfaceNormal,
      kSurfaceNormal};

  iris::RayDifferential actual_ray(iris::Ray(kOrigin, kDirection),
                                   iris::Ray(kDxOrigin, kDirection),
                                   iris::Ray(kDyOrigin, kDirection));
  auto indirect = iris::integrators::internal::SampleIndirectLighting(
      intersection, iris::Sampler(rng), iris::testing::GetSpectralAllocator(),
      actual_ray);
  ASSERT_TRUE(indirect);
  EXPECT_EQ(&reflector, &indirect->reflector);
  EXPECT_FALSE(indirect->differentials);

  iris::RayDifferential expected_ray(
      iris::Ray(iris::Point(std::nextafter(static_cast<iris::geometric>(0.0),
                                           static_cast<iris::geometric>(-2.0)),
                            std::nextafter(static_cast<iris::geometric>(0.0),
                                           static_cast<iris::geometric>(-2.0)),
                            std::nextafter(static_cast<iris::geometric>(0.0),
                                           static_cast<iris::geometric>(2.0))),
                kDirection));
  EXPECT_EQ(expected_ray, actual_ray);
}

TEST(SampleIndirectLighting, SampleWithDifferentials) {
  iris::reflectors::MockReflector reflector;

  iris::random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2)).Times(1);

  iris::bxdfs::MockBxdf bxdf;
  EXPECT_CALL(bxdf, IsDiffuse()).WillRepeatedly(testing::Return(false));
  EXPECT_CALL(bxdf, Sample(testing::_, testing::Not(testing::Eq(std::nullopt)),
                           testing::_, testing::_))
      .WillRepeatedly(testing::Return(iris::Bxdf::SampleResult{
          kOutgoing, {{kOutgoing, kOutgoing}}, &bxdf, 1.0}));
  EXPECT_CALL(bxdf,
              Pdf(testing::_, testing::_, testing::_, testing::_, testing::_))
      .WillRepeatedly(testing::Return(std::nullopt));
  EXPECT_CALL(bxdf, Reflectance(testing::_, testing::_, testing::_, testing::_,
                                testing::_))
      .WillRepeatedly(testing::Return(&reflector));

  iris::Bsdf bsdf(bxdf, kSurfaceNormal, kSurfaceNormal);

  iris::RayTracer::SurfaceIntersection intersection{
      bsdf, iris::HitPoint(kOrigin, kError, kSurfaceNormal),
      iris::RayTracer::Differentials{kDxOrigin, kDyOrigin}, kSurfaceNormal,
      kSurfaceNormal};

  iris::RayDifferential actual_ray(iris::Ray(kOrigin, kDirection),
                                   iris::Ray(kDxOrigin, kDirection),
                                   iris::Ray(kDyOrigin, kDirection));
  auto indirect = iris::integrators::internal::SampleIndirectLighting(
      intersection, iris::Sampler(rng), iris::testing::GetSpectralAllocator(),
      actual_ray);
  ASSERT_TRUE(indirect);
  EXPECT_EQ(&reflector, &indirect->reflector);
  EXPECT_TRUE(indirect->differentials);

  iris::RayDifferential expected_ray(
      iris::Ray(iris::Point(std::nextafter(static_cast<iris::geometric>(0.0),
                                           static_cast<iris::geometric>(-2.0)),
                            std::nextafter(static_cast<iris::geometric>(0.0),
                                           static_cast<iris::geometric>(-2.0)),
                            std::nextafter(static_cast<iris::geometric>(0.0),
                                           static_cast<iris::geometric>(2.0))),
                kDirection),
      iris::Ray(kDxOrigin, kDirection), iris::Ray(kDyOrigin, kDirection));
  EXPECT_EQ(expected_ray, actual_ray);
}