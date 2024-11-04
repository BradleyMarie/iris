#include "iris/integrators/internal/sample_indirect_lighting.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/bxdfs/mock_bxdf.h"
#include "iris/random/mock_random.h"
#include "iris/reflectors/mock_reflector.h"
#include "iris/testing/spectral_allocator.h"

namespace iris {
namespace integrators {
namespace internal {
namespace {

using ::iris::bxdfs::MockBxdf;
using ::iris::random::MockRandom;
using ::iris::reflectors::MockReflector;
using ::iris::testing::GetSpectralAllocator;
using ::testing::_;
using ::testing::DoAll;
using ::testing::Eq;
using ::testing::Not;
using ::testing::NotNull;
using ::testing::Return;
using ::testing::SetArgPointee;

constexpr PositionError kError = PositionError(0.0, 0.0, 0.0);
constexpr Point kOrigin = Point(0.0, 0.0, 0.0);
constexpr Point kDxOrigin = Point(1.0, 0.0, 0.0);
constexpr Point kDyOrigin = Point(0.0, 1.0, 0.0);

static const Vector kDirection = Normalize(Vector(0.0, 0.0, 1.0));
static const Vector kOutgoing = Normalize(Vector(0.0, 0.0, -1.0));
static const Vector kSurfaceNormal = Normalize(Vector(0.0, 0.0, -1.0));

TEST(SampleIndirectLighting, NoSample) {
  MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2)).Times(1);

  MockBxdf bxdf;
  EXPECT_CALL(bxdf, IsDiffuse(NotNull()))
      .WillRepeatedly(DoAll(SetArgPointee<0>(1.0), Return(true)));
  EXPECT_CALL(bxdf, Sample(_, Eq(std::nullopt), _, _, _))
      .WillOnce(Return(std::monostate()));

  Bsdf bsdf(bxdf, kSurfaceNormal, kSurfaceNormal);

  RayTracer::SurfaceIntersection intersection{
      bsdf, HitPoint(kOrigin, kError, kSurfaceNormal), std::nullopt,
      kSurfaceNormal, kSurfaceNormal};

  RayDifferential initial_ray(Ray(kOrigin, kDirection));
  RayDifferential actual_ray(Ray(kOrigin, kDirection));

  EXPECT_FALSE(SampleIndirectLighting(intersection, Sampler(rng),
                                      GetSpectralAllocator(), actual_ray));
  EXPECT_EQ(initial_ray, actual_ray);
}

TEST(SampleIndirectLighting, Sample) {
  MockReflector reflector;

  MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2)).Times(1);

  MockBxdf bxdf;
  EXPECT_CALL(bxdf, IsDiffuse(NotNull()))
      .WillRepeatedly(DoAll(SetArgPointee<0>(1.0), Return(true)));
  EXPECT_CALL(bxdf, Sample(_, Eq(std::nullopt), _, _, _))
      .WillOnce(Return(Bxdf::DiffuseSample{kOutgoing}));
  EXPECT_CALL(bxdf, PdfDiffuse(_, _, _, _)).WillOnce(Return(1.0));
  EXPECT_CALL(bxdf, ReflectanceDiffuse(_, _, _, _))
      .WillOnce(Return(&reflector));

  Bsdf bsdf(bxdf, kSurfaceNormal, kSurfaceNormal);

  RayTracer::SurfaceIntersection intersection{
      bsdf, HitPoint(kOrigin, kError, kSurfaceNormal), std::nullopt,
      kSurfaceNormal, kSurfaceNormal};

  RayDifferential actual_ray(Ray(kOrigin, kDirection));
  std::optional<Bsdf::SampleResult> indirect = SampleIndirectLighting(
      intersection, Sampler(rng), GetSpectralAllocator(), actual_ray);
  ASSERT_TRUE(indirect);
  EXPECT_EQ(&reflector, &indirect->reflector);

  RayDifferential expected_ray(
      Ray(Point(std::nextafter(static_cast<geometric>(0.0),
                               static_cast<geometric>(-2.0)),
                std::nextafter(static_cast<geometric>(0.0),
                               static_cast<geometric>(-2.0)),
                std::nextafter(static_cast<geometric>(0.0),
                               static_cast<geometric>(2.0))),
          kDirection));
  EXPECT_EQ(expected_ray, actual_ray);
}

TEST(SampleIndirectLighting, SampleWithOnlyRayDifferentials) {
  MockReflector reflector;

  MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2)).Times(1);

  MockBxdf bxdf;
  EXPECT_CALL(bxdf, IsDiffuse(NotNull()))
      .WillRepeatedly(DoAll(SetArgPointee<0>(1.0), Return(true)));
  EXPECT_CALL(bxdf, Sample(_, Eq(std::nullopt), _, _, _))
      .WillOnce(Return(Bxdf::DiffuseSample{kOutgoing}));
  EXPECT_CALL(bxdf, PdfDiffuse(_, _, _, _)).WillOnce(Return(1.0));
  EXPECT_CALL(bxdf, ReflectanceDiffuse(_, _, _, _))
      .WillOnce(Return(&reflector));

  Bsdf bsdf(bxdf, kSurfaceNormal, kSurfaceNormal);

  RayTracer::SurfaceIntersection intersection{
      bsdf, HitPoint(kOrigin, kError, kSurfaceNormal), std::nullopt,
      kSurfaceNormal, kSurfaceNormal};

  RayDifferential actual_ray(Ray(kOrigin, kDirection),
                             Ray(kDxOrigin, kDirection),
                             Ray(kDyOrigin, kDirection));
  std::optional<Bsdf::SampleResult> indirect = SampleIndirectLighting(
      intersection, Sampler(rng), GetSpectralAllocator(), actual_ray);
  ASSERT_TRUE(indirect);
  EXPECT_EQ(&reflector, &indirect->reflector);

  RayDifferential expected_ray(
      Ray(Point(std::nextafter(static_cast<geometric>(0.0),
                               static_cast<geometric>(-2.0)),
                std::nextafter(static_cast<geometric>(0.0),
                               static_cast<geometric>(-2.0)),
                std::nextafter(static_cast<geometric>(0.0),
                               static_cast<geometric>(2.0))),
          kDirection));
  EXPECT_EQ(expected_ray, actual_ray);
}

TEST(SampleIndirectLighting, SampleWithOnlyIntersection) {
  MockReflector reflector;

  MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2)).Times(1);

  MockBxdf bxdf;
  EXPECT_CALL(bxdf, IsDiffuse(NotNull()))
      .WillRepeatedly(DoAll(SetArgPointee<0>(1.0), Return(true)));
  EXPECT_CALL(bxdf, Sample(_, Eq(std::nullopt), _, _, _))
      .WillOnce(Return(Bxdf::DiffuseSample{kOutgoing}));
  EXPECT_CALL(bxdf, PdfDiffuse(_, _, _, _)).WillOnce(Return(1.0));
  EXPECT_CALL(bxdf, ReflectanceDiffuse(_, _, _, _))
      .WillOnce(Return(&reflector));

  Bsdf bsdf(bxdf, kSurfaceNormal, kSurfaceNormal);

  RayTracer::SurfaceIntersection intersection{
      bsdf, HitPoint(kOrigin, kError, kSurfaceNormal),
      RayTracer::Differentials{kDxOrigin, kDyOrigin}, kSurfaceNormal,
      kSurfaceNormal};

  RayDifferential actual_ray(Ray(kOrigin, kDirection));
  std::optional<Bsdf::SampleResult> indirect = SampleIndirectLighting(
      intersection, Sampler(rng), GetSpectralAllocator(), actual_ray);
  ASSERT_TRUE(indirect);
  EXPECT_EQ(&reflector, &indirect->reflector);
  EXPECT_FALSE(indirect->differentials);

  RayDifferential expected_ray(
      Ray(Point(std::nextafter(static_cast<geometric>(0.0),
                               static_cast<geometric>(-2.0)),
                std::nextafter(static_cast<geometric>(0.0),
                               static_cast<geometric>(-2.0)),
                std::nextafter(static_cast<geometric>(0.0),
                               static_cast<geometric>(2.0))),
          kDirection));
  EXPECT_EQ(expected_ray, actual_ray);
}

TEST(SampleIndirectLighting, SampleWithDifferentialsNoneReturned) {
  MockReflector reflector;

  MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2)).Times(1);

  MockBxdf bxdf;
  EXPECT_CALL(bxdf, IsDiffuse(NotNull()))
      .WillRepeatedly(DoAll(SetArgPointee<0>(1.0), Return(true)));
  EXPECT_CALL(bxdf, Sample(_, Not(Eq(std::nullopt)), _, _, _))
      .WillOnce(Return(Bxdf::DiffuseSample{kOutgoing}));
  EXPECT_CALL(bxdf, PdfDiffuse(_, _, _, _)).WillOnce(Return(1.0));
  EXPECT_CALL(bxdf, ReflectanceDiffuse(_, _, _, _))
      .WillOnce(Return(&reflector));

  Bsdf bsdf(bxdf, kSurfaceNormal, kSurfaceNormal);

  RayTracer::SurfaceIntersection intersection{
      bsdf, HitPoint(kOrigin, kError, kSurfaceNormal),
      RayTracer::Differentials{kDxOrigin, kDyOrigin}, kSurfaceNormal,
      kSurfaceNormal};

  RayDifferential actual_ray(Ray(kOrigin, kDirection),
                             Ray(kDxOrigin, kDirection),
                             Ray(kDyOrigin, kDirection));
  std::optional<Bsdf::SampleResult> indirect = SampleIndirectLighting(
      intersection, Sampler(rng), GetSpectralAllocator(), actual_ray);
  ASSERT_TRUE(indirect);
  EXPECT_EQ(&reflector, &indirect->reflector);
  EXPECT_FALSE(indirect->differentials);

  RayDifferential expected_ray(
      Ray(Point(std::nextafter(static_cast<geometric>(0.0),
                               static_cast<geometric>(-2.0)),
                std::nextafter(static_cast<geometric>(0.0),
                               static_cast<geometric>(-2.0)),
                std::nextafter(static_cast<geometric>(0.0),
                               static_cast<geometric>(2.0))),
          kDirection));
  EXPECT_EQ(expected_ray, actual_ray);
}

TEST(SampleIndirectLighting, SampleWithDifferentials) {
  MockReflector reflector;

  MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2)).Times(1);

  MockBxdf bxdf;
  EXPECT_CALL(bxdf, IsDiffuse(NotNull()))
      .WillRepeatedly(DoAll(SetArgPointee<0>(0.0), Return(false)));
  EXPECT_CALL(bxdf, Sample(_, Not(Eq(std::nullopt)), _, _, _))
      .WillOnce(Return(Bxdf::SpecularSample{Bxdf::Hemisphere::BTDF,
                                            kOutgoing,
                                            reflector,
                                            {{kOutgoing, kOutgoing}},
                                            1.0}));

  Bsdf bsdf(bxdf, kSurfaceNormal, kSurfaceNormal);

  RayTracer::SurfaceIntersection intersection{
      bsdf, HitPoint(kOrigin, kError, kSurfaceNormal),
      RayTracer::Differentials{kDxOrigin, kDyOrigin}, kSurfaceNormal,
      kSurfaceNormal};

  RayDifferential actual_ray(Ray(kOrigin, kDirection),
                             Ray(kDxOrigin, kDirection),
                             Ray(kDyOrigin, kDirection));
  std::optional<Bsdf::SampleResult> indirect = SampleIndirectLighting(
      intersection, Sampler(rng), GetSpectralAllocator(), actual_ray);
  ASSERT_TRUE(indirect);
  EXPECT_EQ(&reflector, &indirect->reflector);
  EXPECT_TRUE(indirect->differentials);

  RayDifferential expected_ray(
      Ray(Point(std::nextafter(static_cast<geometric>(0.0),
                               static_cast<geometric>(-2.0)),
                std::nextafter(static_cast<geometric>(0.0),
                               static_cast<geometric>(-2.0)),
                std::nextafter(static_cast<geometric>(0.0),
                               static_cast<geometric>(2.0))),
          kDirection),
      Ray(kDxOrigin, kDirection), Ray(kDyOrigin, kDirection));
  EXPECT_EQ(expected_ray, actual_ray);
}

}  // namespace
}  // namespace internal
}  // namespace integrators
}  // namespace iris