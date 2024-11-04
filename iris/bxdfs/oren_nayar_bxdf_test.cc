#include "iris/bxdfs/oren_nayar_bxdf.h"

#include <numbers>

#include "googletest/include/gtest/gtest.h"
#include "iris/random/mock_random.h"
#include "iris/reflectors/mock_reflector.h"
#include "iris/testing/spectral_allocator.h"

namespace iris {
namespace bxdfs {
namespace {

using ::iris::random::MockRandom;
using ::iris::reflectors::MockReflector;
using ::testing::_;
using ::testing::Return;

TEST(OrenNayarBrdfTest, SampleDiffuseAligned) {
  MockReflector reflector;
  MockRandom rng;
  EXPECT_CALL(rng, NextGeometric()).Times(2).WillRepeatedly(Return(0.0));
  Sampler sampler(rng);

  OrenNayarBrdf bxdf(reflector, 0.1);
  std::optional<Vector> result =
      bxdf.SampleDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0), sampler);
  ASSERT_TRUE(result);
  EXPECT_NEAR(result->x, -0.707106709, 0.0001);
  EXPECT_NEAR(result->y, -0.707106709, 0.0001);
  EXPECT_NEAR(result->z, 0.0003452669, 0.0001);
}

TEST(OrenNayarBrdfTest, SampleDiffuseUnaligned) {
  MockReflector reflector;
  MockRandom rng;
  EXPECT_CALL(rng, NextGeometric()).Times(2).WillRepeatedly(Return(0.0));
  Sampler sampler(rng);

  OrenNayarBrdf bxdf(reflector, 0.1);
  std::optional<Vector> result = bxdf.SampleDiffuse(
      Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, -1.0), sampler);
  ASSERT_TRUE(result);
  EXPECT_NEAR(result->x, 0.707106709, 0.0001);
  EXPECT_NEAR(result->y, 0.707106709, 0.0001);
  EXPECT_NEAR(result->z, -0.0003452669, 0.0001);
}

TEST(OrenNayarBrdfTest, DiffusePdfTransmitted) {
  MockReflector reflector;
  OrenNayarBrdf bxdf(reflector, 0.1);
  EXPECT_EQ(0.0,
            bxdf.PdfDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, -1.0),
                            Vector(0.0, 0.0, -1.0), Bxdf::Hemisphere::BTDF));
}

TEST(OrenNayarBrdfTest, DiffusePdfReflected) {
  MockReflector reflector;
  OrenNayarBrdf bxdf(reflector, 0.1);
  EXPECT_NEAR(std::numbers::inv_pi,
              bxdf.PdfDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0),
                              Vector(0.0, 0.0, -1.0), Bxdf::Hemisphere::BRDF),
              0.001);
}

TEST(OrenNayarBrdfTest, ReflectanceBtdf) {
  MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(_)).WillRepeatedly(Return(1.0));

  OrenNayarBrdf bxdf(reflector, 0.1);
  const Reflector* result = bxdf.ReflectanceDiffuse(
      Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0), Bxdf::Hemisphere::BTDF,
      testing::GetSpectralAllocator());
  ASSERT_FALSE(result);
}

TEST(OrenNayarBrdfTest, ReflectanceTransmitted) {
  MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(_)).WillRepeatedly(Return(1.0));

  OrenNayarBrdf bxdf(reflector, 0.1);
  const Reflector* result = bxdf.ReflectanceDiffuse(
      Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, -1.0), Bxdf::Hemisphere::BTDF,
      testing::GetSpectralAllocator());
  ASSERT_FALSE(result);
}

TEST(OrenNayarBrdfTest, ReflectanceVertical) {
  MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(_)).WillRepeatedly(Return(1.0));

  OrenNayarBrdf bxdf(reflector, 0.1);
  const Reflector* result = bxdf.ReflectanceDiffuse(
      Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0), Bxdf::Hemisphere::BRDF,
      testing::GetSpectralAllocator());
  ASSERT_TRUE(result);
  EXPECT_NEAR(std::numbers::inv_pi, result->Reflectance(1.0), 0.0001);
}

TEST(OrenNayarBrdfTest, ReflectanceIncomingDominant) {
  MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(_)).WillRepeatedly(Return(1.0));

  OrenNayarBrdf bxdf(reflector, 10.0);
  const Reflector* result = bxdf.ReflectanceDiffuse(
      Normalize(Vector(0.0, 1.0, 2.0)), Normalize(Vector(0.0, 1.0, 1.0)),
      Bxdf::Hemisphere::BRDF, testing::GetSpectralAllocator());
  ASSERT_TRUE(result);
  EXPECT_NEAR(0.31766638, result->Reflectance(1.0), 0.0001);
}

TEST(OrenNayarBrdfTest, ReflectanceOutgoingDominant) {
  MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(_)).WillRepeatedly(Return(1.0));

  OrenNayarBrdf bxdf(reflector, 10.0);
  const Reflector* result = bxdf.ReflectanceDiffuse(
      Normalize(Vector(0.0, 1.0, 1.0)), Normalize(Vector(0.0, 1.0, 2.0)),
      Bxdf::Hemisphere::BRDF, testing::GetSpectralAllocator());
  ASSERT_TRUE(result);
  EXPECT_NEAR(0.31766638, result->Reflectance(1.0), 0.0001);
}

}  // namespace
}  // namespace bxdfs
}  // namespace iris