#include "iris/bxdfs/disney_bxdf.h"

#include <numbers>

#include "googletest/include/gtest/gtest.h"
#include "iris/random/mock_random.h"
#include "iris/reflectors/mock_reflector.h"
#include "iris/testing/bxdf_allocator.h"
#include "iris/testing/spectral_allocator.h"

namespace iris {
namespace bxdfs {
namespace {

using ::iris::random::MockRandom;
using ::iris::reflectors::MockReflector;
using ::iris::testing::GetBxdfAllocator;
using ::testing::_;
using ::testing::Return;

TEST(DisneyDiffuseBrdfTest, NullReflector) {
  EXPECT_FALSE(MakeDisneyDiffuseBrdf(GetBxdfAllocator(), nullptr));
}

TEST(DisneyDiffuseBrdfTest, SampleDiffuseAligned) {
  MockReflector reflector;
  MockRandom rng;
  EXPECT_CALL(rng, NextGeometric()).Times(2).WillRepeatedly(Return(0.0));
  Sampler sampler(rng);

  const Bxdf* bxdf = MakeDisneyDiffuseBrdf(GetBxdfAllocator(), &reflector);
  std::optional<Vector> result = bxdf->SampleDiffuse(
      Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0), sampler);
  ASSERT_TRUE(result);
  EXPECT_NEAR(result->x, -0.707106709, 0.0001);
  EXPECT_NEAR(result->y, -0.707106709, 0.0001);
  EXPECT_NEAR(result->z, 0.0003452669, 0.0001);
}

TEST(DisneyDiffuseBrdfTest, SampleDiffuseUnaligned) {
  MockReflector reflector;
  MockRandom rng;
  EXPECT_CALL(rng, NextGeometric()).Times(2).WillRepeatedly(Return(0.0));
  Sampler sampler(rng);

  const Bxdf* bxdf = MakeDisneyDiffuseBrdf(GetBxdfAllocator(), &reflector);
  std::optional<Vector> result = bxdf->SampleDiffuse(
      Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, -1.0), sampler);
  ASSERT_TRUE(result);
  EXPECT_NEAR(result->x, 0.707106709, 0.0001);
  EXPECT_NEAR(result->y, 0.707106709, 0.0001);
  EXPECT_NEAR(result->z, -0.000345266, 0.0001);
}

TEST(DisneyDiffuseBrdfTest, PdfDiffuseTransmitted) {
  MockReflector reflector;
  const Bxdf* bxdf = MakeDisneyDiffuseBrdf(GetBxdfAllocator(), &reflector);
  EXPECT_EQ(0.0,
            bxdf->PdfDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, -1.0),
                             Vector(0.0, 0.0, -1.0), Bxdf::Hemisphere::BTDF));
}

TEST(DisneyDiffuseBrdfTest, PdfDiffuseReflected) {
  MockReflector reflector;
  const Bxdf* bxdf = MakeDisneyDiffuseBrdf(GetBxdfAllocator(), &reflector);
  EXPECT_NEAR(std::numbers::inv_pi,
              bxdf->PdfDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0),
                               Vector(0.0, 0.0, -1.0), Bxdf::Hemisphere::BRDF),
              0.001);
}

TEST(DisneyDiffuseBrdfTest, ReflectanceBtdf) {
  MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(_)).WillRepeatedly(Return(1.0));

  const Bxdf* bxdf = MakeDisneyDiffuseBrdf(GetBxdfAllocator(), &reflector);
  ASSERT_FALSE(bxdf->ReflectanceDiffuse(
      Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0), Bxdf::Hemisphere::BTDF,
      testing::GetSpectralAllocator()));
}

TEST(DisneyDiffuseBrdfTest, ReflectanceTransmitted) {
  MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(_)).WillRepeatedly(Return(1.0));

  const Bxdf* bxdf = MakeDisneyDiffuseBrdf(GetBxdfAllocator(), &reflector);
  EXPECT_FALSE(bxdf->ReflectanceDiffuse(
      Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, -1.0), Bxdf::Hemisphere::BTDF,
      testing::GetSpectralAllocator()));
}

TEST(DisneyDiffuseBrdfTest, ReflectanceIncident) {
  MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(_)).WillRepeatedly(Return(1.0));

  const Bxdf* bxdf = MakeDisneyDiffuseBrdf(GetBxdfAllocator(), &reflector);
  const Reflector* result = bxdf->ReflectanceDiffuse(
      Vector(0.8660254, 0.0, 0.5), Vector(0.0, 0.0, 1.0),
      Bxdf::Hemisphere::BRDF, testing::GetSpectralAllocator());

  ASSERT_TRUE(result);
  EXPECT_NEAR(result->Reflectance(1.0), 0.313336, 0.0001);
}

TEST(DisneySubsurfaceBrdfTest, NullReflector) {
  EXPECT_FALSE(MakeDisneySubsurfaceBrdf(GetBxdfAllocator(), nullptr, 0.5));
}

TEST(DisneySubsurfaceBrdfTest, SampleDiffuseAligned) {
  MockReflector reflector;
  MockRandom rng;
  EXPECT_CALL(rng, NextGeometric()).Times(2).WillRepeatedly(Return(0.0));
  Sampler sampler(rng);

  const Bxdf* bxdf =
      MakeDisneySubsurfaceBrdf(GetBxdfAllocator(), &reflector, 0.5);
  std::optional<Vector> result = bxdf->SampleDiffuse(
      Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0), sampler);
  ASSERT_TRUE(result);
  EXPECT_NEAR(result->x, -0.707106709, 0.0001);
  EXPECT_NEAR(result->y, -0.707106709, 0.0001);
  EXPECT_NEAR(result->z, 0.0003452669, 0.0001);
}

TEST(DisneySubsurfaceBrdfTest, SampleDiffuseUnaligned) {
  MockReflector reflector;
  MockRandom rng;
  EXPECT_CALL(rng, NextGeometric()).Times(2).WillRepeatedly(Return(0.0));
  Sampler sampler(rng);

  const Bxdf* bxdf =
      MakeDisneySubsurfaceBrdf(GetBxdfAllocator(), &reflector, 0.5);
  std::optional<Vector> result = bxdf->SampleDiffuse(
      Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, -1.0), sampler);
  ASSERT_TRUE(result);
  EXPECT_NEAR(result->x, 0.707106709, 0.0001);
  EXPECT_NEAR(result->y, 0.707106709, 0.0001);
  EXPECT_NEAR(result->z, -0.000345266, 0.0001);
}

TEST(DisneySubsurfaceBrdfTest, PdfDiffuseTransmitted) {
  MockReflector reflector;
  const Bxdf* bxdf =
      MakeDisneySubsurfaceBrdf(GetBxdfAllocator(), &reflector, 0.5);
  EXPECT_EQ(0.0,
            bxdf->PdfDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, -1.0),
                             Vector(0.0, 0.0, -1.0), Bxdf::Hemisphere::BTDF));
}

TEST(DisneySubsurfaceBrdfTest, PdfDiffuseReflected) {
  MockReflector reflector;
  const Bxdf* bxdf =
      MakeDisneySubsurfaceBrdf(GetBxdfAllocator(), &reflector, 0.5);
  EXPECT_NEAR(std::numbers::inv_pi,
              bxdf->PdfDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0),
                               Vector(0.0, 0.0, -1.0), Bxdf::Hemisphere::BRDF),
              0.001);
}

TEST(DisneySubsurfaceBrdfTest, ReflectanceBtdf) {
  MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(_)).WillRepeatedly(Return(1.0));

  const Bxdf* bxdf =
      MakeDisneySubsurfaceBrdf(GetBxdfAllocator(), &reflector, 0.5);
  EXPECT_FALSE(bxdf->ReflectanceDiffuse(
      Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0), Bxdf::Hemisphere::BTDF,
      testing::GetSpectralAllocator()));
}

TEST(DisneySubsurfaceBrdfTest, ReflectanceTransmitted) {
  MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(_)).WillRepeatedly(Return(1.0));

  const Bxdf* bxdf =
      MakeDisneySubsurfaceBrdf(GetBxdfAllocator(), &reflector, 0.5);
  EXPECT_FALSE(bxdf->ReflectanceDiffuse(
      Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, -1.0), Bxdf::Hemisphere::BTDF,
      testing::GetSpectralAllocator()));
}

TEST(DisneySubsurfaceBrdfTest, ReflectanceNoHalfAngle) {
  MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(_)).WillRepeatedly(Return(1.0));

  const Bxdf* bxdf =
      MakeDisneySubsurfaceBrdf(GetBxdfAllocator(), &reflector, 0.5);
  EXPECT_FALSE(bxdf->ReflectanceDiffuse(
      Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, -1.0), Bxdf::Hemisphere::BRDF,
      testing::GetSpectralAllocator()));
}

TEST(DisneySubsurfaceBrdfTest, Reflectance) {
  MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(_)).WillRepeatedly(Return(1.0));

  const Bxdf* bxdf =
      MakeDisneySubsurfaceBrdf(GetBxdfAllocator(), &reflector, 0.5);

  Vector incoming(0.707106781, 0.0, 0.707106781);
  Vector outgoing(0.0, 0.0, -1.0);
  const Reflector* result =
      bxdf->ReflectanceDiffuse(incoming, outgoing, Bxdf::Hemisphere::BRDF,
                               testing::GetSpectralAllocator());
  ASSERT_TRUE(result);
  EXPECT_NEAR(result->Reflectance(1.0), 0.1989, 0.001);
}

}  // namespace
}  // namespace bxdfs
}  // namespace iris
