#include "iris/integrators/internal/sample_indirect_lighting.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/bxdfs/mock_bxdf.h"
#include "iris/random/mock_random.h"
#include "iris/reflectors/mock_reflector.h"
#include "iris/testing/spectral_allocator.h"

TEST(SampleIndirectLighting, NoSample) {
  iris::random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2)).Times(1);

  iris::bxdfs::MockBxdf bxdf;
  EXPECT_CALL(bxdf, Sample(testing::_, testing::Eq(std::nullopt), testing::_,
                           testing::_))
      .WillOnce(testing::Return(
          iris::Bxdf::SampleResult{iris::Vector(1.0, 0.0, 0.0)}));
  EXPECT_CALL(bxdf,
              Pdf(testing::_, testing::_, testing::_, testing::_, testing::_))
      .WillOnce(testing::Return(static_cast<iris::visual_t>(0.0)));

  iris::Bsdf bsdf(bxdf, iris::Vector(0.0, 1.0, 0.0),
                  iris::Vector(0.0, 1.0, 0.0));

  iris::RayTracer::SurfaceIntersection intersection{
      bsdf, iris::Point(1.0, 0.0, 0.0), std::nullopt,
      iris::Vector(0.0, 1.0, 0.0), iris::Vector(0.0, 1.0, 0.0)};

  iris::RayDifferential initial_ray(
      iris::Ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(0.0, 0.0, 1.0)));
  iris::RayDifferential actual_ray(
      iris::Ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(0.0, 0.0, 1.0)));

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
  EXPECT_CALL(bxdf, Sample(testing::_, testing::Eq(std::nullopt), testing::_,
                           testing::_))
      .WillRepeatedly(testing::Return(
          iris::Bxdf::SampleResult{iris::Vector(0.0, 0.0, 1.0)}));
  EXPECT_CALL(bxdf,
              Pdf(testing::_, testing::_, testing::_, testing::_, testing::_))
      .WillRepeatedly(testing::Return(std::nullopt));
  EXPECT_CALL(bxdf, Reflectance(testing::_, testing::_, testing::_, testing::_,
                                testing::_))
      .WillRepeatedly(testing::Return(&reflector));

  iris::Bsdf bsdf(bxdf, iris::Vector(0.0, 1.0, 0.0),
                  iris::Vector(0.0, 1.0, 0.0));

  iris::RayTracer::SurfaceIntersection intersection{
      bsdf, iris::Point(1.0, 0.0, 0.0), std::nullopt,
      iris::Vector(0.0, 1.0, 0.0), iris::Vector(0.0, 1.0, 0.0)};

  iris::RayDifferential actual_ray(
      iris::Ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(0.0, 0.0, 1.0)));
  auto indirect = iris::integrators::internal::SampleIndirectLighting(
      intersection, iris::Sampler(rng), iris::testing::GetSpectralAllocator(),
      actual_ray);
  ASSERT_TRUE(indirect);
  EXPECT_EQ(&reflector, &indirect->reflector);

  iris::RayDifferential expected_ray(
      iris::Ray(iris::Point(1.0, 0.0, 0.0), iris::Vector(0.0, 1.0, 0.0)));
  EXPECT_EQ(expected_ray, actual_ray);
}

TEST(SampleIndirectLighting, SampleWithOnlyRayDifferentials) {
  iris::reflectors::MockReflector reflector;

  iris::random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2)).Times(1);

  iris::bxdfs::MockBxdf bxdf;
  EXPECT_CALL(bxdf, Sample(testing::_, testing::Eq(std::nullopt), testing::_,
                           testing::_))
      .WillRepeatedly(testing::Return(iris::Bxdf::SampleResult{
          iris::Vector(0.0, 0.0, 1.0),
          {{iris::Vector(0.0, 1.0, 0.0), iris::Vector(0.0, 0.0, 1.0)}}}));
  EXPECT_CALL(bxdf,
              Pdf(testing::_, testing::_, testing::_, testing::_, testing::_))
      .WillRepeatedly(testing::Return(std::nullopt));
  EXPECT_CALL(bxdf, Reflectance(testing::_, testing::_, testing::_, testing::_,
                                testing::_))
      .WillRepeatedly(testing::Return(&reflector));

  iris::Bsdf bsdf(bxdf, iris::Vector(0.0, 1.0, 0.0),
                  iris::Vector(0.0, 1.0, 0.0));

  iris::RayTracer::SurfaceIntersection intersection{
      bsdf, iris::Point(1.0, 0.0, 0.0), std::nullopt,
      iris::Vector(0.0, 1.0, 0.0), iris::Vector(0.0, 1.0, 0.0)};

  iris::RayDifferential actual_ray(
      iris::Ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(1.0, 0.0, 0.0)),
      iris::Ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(0.0, 1.0, 0.0)),
      iris::Ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(0.0, 0.0, 1.0)));
  auto indirect = iris::integrators::internal::SampleIndirectLighting(
      intersection, iris::Sampler(rng), iris::testing::GetSpectralAllocator(),
      actual_ray);
  ASSERT_TRUE(indirect);
  EXPECT_EQ(&reflector, &indirect->reflector);
  EXPECT_TRUE(indirect->differentials);

  iris::RayDifferential expected_ray(
      iris::Ray(iris::Point(1.0, 0.0, 0.0), iris::Vector(0.0, 1.0, 0.0)));
  EXPECT_EQ(expected_ray, actual_ray);
}

TEST(SampleIndirectLighting, SampleWithOnlyIntersection) {
  iris::reflectors::MockReflector reflector;

  iris::random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2)).Times(1);

  iris::bxdfs::MockBxdf bxdf;
  EXPECT_CALL(bxdf, Sample(testing::_, testing::Eq(std::nullopt), testing::_,
                           testing::_))
      .WillRepeatedly(testing::Return(
          iris::Bxdf::SampleResult{iris::Vector(0.0, 0.0, 1.0)}));
  EXPECT_CALL(bxdf,
              Pdf(testing::_, testing::_, testing::_, testing::_, testing::_))
      .WillRepeatedly(testing::Return(std::nullopt));
  EXPECT_CALL(bxdf, Reflectance(testing::_, testing::_, testing::_, testing::_,
                                testing::_))
      .WillRepeatedly(testing::Return(&reflector));

  iris::Bsdf bsdf(bxdf, iris::Vector(0.0, 1.0, 0.0),
                  iris::Vector(0.0, 1.0, 0.0));

  iris::RayTracer::SurfaceIntersection intersection{
      bsdf, iris::Point(1.0, 0.0, 0.0),
      iris::RayTracer::Differentials{iris::Point(1.0, 1.0, 0.0),
                                     iris::Point(1.0, 0.0, 1.0)},
      iris::Vector(0.0, 1.0, 0.0), iris::Vector(0.0, 1.0, 0.0)};

  iris::RayDifferential actual_ray(
      iris::Ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(1.0, 0.0, 0.0)));
  auto indirect = iris::integrators::internal::SampleIndirectLighting(
      intersection, iris::Sampler(rng), iris::testing::GetSpectralAllocator(),
      actual_ray);
  ASSERT_TRUE(indirect);
  EXPECT_EQ(&reflector, &indirect->reflector);
  EXPECT_FALSE(indirect->differentials);

  iris::RayDifferential expected_ray(
      iris::Ray(iris::Point(1.0, 0.0, 0.0), iris::Vector(0.0, 1.0, 0.0)));
  EXPECT_EQ(expected_ray, actual_ray);
}

TEST(SampleIndirectLighting, SampleWithDifferentialsNoneReturned) {
  iris::reflectors::MockReflector reflector;

  iris::random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2)).Times(1);

  iris::bxdfs::MockBxdf bxdf;
  EXPECT_CALL(bxdf, Sample(testing::_, testing::Not(testing::Eq(std::nullopt)),
                           testing::_, testing::_))
      .WillRepeatedly(testing::Return(
          iris::Bxdf::SampleResult{iris::Vector(0.0, 0.0, 1.0)}));
  EXPECT_CALL(bxdf,
              Pdf(testing::_, testing::_, testing::_, testing::_, testing::_))
      .WillRepeatedly(testing::Return(std::nullopt));
  EXPECT_CALL(bxdf, Reflectance(testing::_, testing::_, testing::_, testing::_,
                                testing::_))
      .WillRepeatedly(testing::Return(&reflector));

  iris::Bsdf bsdf(bxdf, iris::Vector(0.0, 1.0, 0.0),
                  iris::Vector(0.0, 1.0, 0.0));

  iris::RayTracer::SurfaceIntersection intersection{
      bsdf, iris::Point(1.0, 0.0, 0.0),
      iris::RayTracer::Differentials{iris::Point(1.0, 1.0, 0.0),
                                     iris::Point(1.0, 0.0, 1.0)},
      iris::Vector(0.0, 1.0, 0.0), iris::Vector(0.0, 1.0, 0.0)};

  iris::RayDifferential actual_ray(
      iris::Ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(1.0, 0.0, 0.0)),
      iris::Ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(0.0, 1.0, 0.0)),
      iris::Ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(0.0, 0.0, 1.0)));
  auto indirect = iris::integrators::internal::SampleIndirectLighting(
      intersection, iris::Sampler(rng), iris::testing::GetSpectralAllocator(),
      actual_ray);
  ASSERT_TRUE(indirect);
  EXPECT_EQ(&reflector, &indirect->reflector);
  EXPECT_FALSE(indirect->differentials);

  iris::RayDifferential expected_ray(
      iris::Ray(iris::Point(1.0, 0.0, 0.0), iris::Vector(0.0, 1.0, 0.0)));
  EXPECT_EQ(expected_ray, actual_ray);
}

TEST(SampleIndirectLighting, SampleWithDifferentials) {
  iris::reflectors::MockReflector reflector;

  iris::random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2)).Times(1);

  iris::bxdfs::MockBxdf bxdf;
  EXPECT_CALL(bxdf, Sample(testing::_, testing::Not(testing::Eq(std::nullopt)),
                           testing::_, testing::_))
      .WillRepeatedly(testing::Return(iris::Bxdf::SampleResult{
          iris::Vector(0.0, 0.0, 1.0),
          {{iris::Vector(0.0, 0.0, 1.0), iris::Vector(0.0, 0.0, 1.0)}}}));
  EXPECT_CALL(bxdf,
              Pdf(testing::_, testing::_, testing::_, testing::_, testing::_))
      .WillRepeatedly(testing::Return(std::nullopt));
  EXPECT_CALL(bxdf, Reflectance(testing::_, testing::_, testing::_, testing::_,
                                testing::_))
      .WillRepeatedly(testing::Return(&reflector));

  iris::Bsdf bsdf(bxdf, iris::Vector(0.0, 1.0, 0.0),
                  iris::Vector(0.0, 1.0, 0.0));

  iris::RayTracer::SurfaceIntersection intersection{
      bsdf, iris::Point(1.0, 0.0, 0.0),
      iris::RayTracer::Differentials{iris::Point(1.0, 1.0, 0.0),
                                     iris::Point(1.0, 0.0, 1.0)},
      iris::Vector(0.0, 1.0, 0.0), iris::Vector(0.0, 1.0, 0.0)};

  iris::RayDifferential actual_ray(
      iris::Ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(1.0, 0.0, 0.0)),
      iris::Ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(0.0, 1.0, 0.0)),
      iris::Ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(0.0, 0.0, 1.0)));
  auto indirect = iris::integrators::internal::SampleIndirectLighting(
      intersection, iris::Sampler(rng), iris::testing::GetSpectralAllocator(),
      actual_ray);
  ASSERT_TRUE(indirect);
  EXPECT_EQ(&reflector, &indirect->reflector);
  EXPECT_TRUE(indirect->differentials);

  iris::RayDifferential expected_ray(
      iris::Ray(iris::Point(1.0, 0.0, 0.0), iris::Vector(0.0, 1.0, 0.0)),
      iris::Ray(iris::Point(1.0, 1.0, 0.0), iris::Vector(0.0, 1.0, 0.0)),
      iris::Ray(iris::Point(1.0, 0.0, 1.0), iris::Vector(0.0, 1.0, 0.0)));
  EXPECT_EQ(expected_ray, actual_ray);
}