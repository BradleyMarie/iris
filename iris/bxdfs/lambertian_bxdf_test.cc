#include "iris/bxdfs/lambertian_bxdf.h"

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

TEST(LambertianBrdfTest, SampleDiffuseAligned) {
  MockReflector reflector;
  MockRandom rng;
  EXPECT_CALL(rng, NextGeometric()).Times(2).WillRepeatedly(Return(0.0));
  Sampler sampler(rng);

  LambertianBrdf bxdf(reflector);
  std::optional<Vector> result =
      bxdf.SampleDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0), sampler);
  ASSERT_TRUE(result);
  EXPECT_NEAR(result->x, -0.707106709, 0.0001);
  EXPECT_NEAR(result->y, -0.707106709, 0.0001);
  EXPECT_NEAR(result->z, 0.0003452669, 0.0001);
}

TEST(LambertianBrdfTest, SampleDiffuseUnaligned) {
  MockReflector reflector;
  MockRandom rng;
  EXPECT_CALL(rng, NextGeometric()).Times(2).WillRepeatedly(Return(0.0));
  Sampler sampler(rng);

  LambertianBrdf bxdf(reflector);
  std::optional<Vector> result = bxdf.SampleDiffuse(
      Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, -1.0), sampler);
  ASSERT_TRUE(result);
  EXPECT_NEAR(result->x, 0.707106709, 0.0001);
  EXPECT_NEAR(result->y, 0.707106709, 0.0001);
  EXPECT_NEAR(result->z, -0.000345266, 0.0001);
}

TEST(LambertianBrdfTest, DiffusePdfTransmitted) {
  MockReflector reflector;
  LambertianBrdf bxdf(reflector);
  EXPECT_EQ(0.0,
            bxdf.PdfDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, -1.0),
                            Vector(0.0, 0.0, -1.0), Bxdf::Hemisphere::BTDF));
}

TEST(LambertianBrdfTest, DiffusePdfReflected) {
  MockReflector reflector;
  LambertianBrdf bxdf(reflector);
  EXPECT_NEAR(std::numbers::inv_pi,
              bxdf.PdfDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0),
                              Vector(0.0, 0.0, -1.0), Bxdf::Hemisphere::BRDF),
              0.001);
}

TEST(LambertianBrdfTest, ReflectanceBtdf) {
  MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(_)).WillRepeatedly(Return(1.0));

  LambertianBrdf bxdf(reflector);
  const Reflector* result = bxdf.ReflectanceDiffuse(
      Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0), Bxdf::Hemisphere::BTDF,
      testing::GetSpectralAllocator());
  ASSERT_FALSE(result);
}

TEST(LambertianBrdfTest, ReflectanceTransmitted) {
  MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(_)).WillRepeatedly(Return(1.0));

  LambertianBrdf bxdf(reflector);
  const Reflector* result = bxdf.ReflectanceDiffuse(
      Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, -1.0), Bxdf::Hemisphere::BTDF,
      testing::GetSpectralAllocator());
  ASSERT_FALSE(result);
}

TEST(LambertianBrdfTest, Reflectance) {
  MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(_)).WillRepeatedly(Return(1.0));

  LambertianBrdf bxdf(reflector);
  const Reflector* result = bxdf.ReflectanceDiffuse(
      Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0), Bxdf::Hemisphere::BRDF,
      testing::GetSpectralAllocator());
  ASSERT_TRUE(result);
  EXPECT_NEAR(std::numbers::inv_pi, result->Reflectance(1.0), 0.0001);
}

TEST(LambertianBtdfTest, SampleDiffuseAligned) {
  MockReflector reflector;
  MockRandom rng;
  EXPECT_CALL(rng, NextGeometric()).Times(2).WillRepeatedly(Return(0.0));
  Sampler sampler(rng);

  LambertianBtdf bxdf(reflector);
  std::optional<Vector> result =
      bxdf.SampleDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0), sampler);
  ASSERT_TRUE(result);
  EXPECT_NEAR(result->x, -0.707106709, 0.0001);
  EXPECT_NEAR(result->y, -0.707106709, 0.0001);
  EXPECT_NEAR(result->z, -0.000345266, 0.0001);
}

TEST(LambertianBtdfTest, SampleDiffuseUnaligned) {
  MockReflector reflector;
  MockRandom rng;
  EXPECT_CALL(rng, NextGeometric()).Times(2).WillRepeatedly(Return(0.0));
  Sampler sampler(rng);

  LambertianBtdf bxdf(reflector);
  std::optional<Vector> result =
      bxdf.SampleDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0), sampler);
  ASSERT_TRUE(result);
  EXPECT_NEAR(result->x, -0.707106709, 0.0001);
  EXPECT_NEAR(result->y, -0.707106709, 0.0001);
  EXPECT_NEAR(result->z, -0.000345266, 0.0001);
}

TEST(LambertianBtdfTest, DiffusePdfReflected) {
  MockReflector reflector;
  LambertianBtdf bxdf(reflector);
  EXPECT_EQ(0.0,
            bxdf.PdfDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, -1.0),
                            Vector(0.0, 0.0, 1.0), Bxdf::Hemisphere::BRDF));
}

TEST(LambertianBtdfTest, DiffusePdfTransmitted) {
  MockReflector reflector;
  LambertianBtdf bxdf(reflector);
  EXPECT_NEAR(std::numbers::inv_pi,
              bxdf.PdfDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0),
                              Vector(0.0, 0.0, -1.0), Bxdf::Hemisphere::BTDF),
              0.001);
}

TEST(LambertianBtdfTest, ReflectanceBrdf) {
  MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(_)).WillRepeatedly(Return(1.0));

  LambertianBtdf bxdf(reflector);
  const Reflector* result = bxdf.ReflectanceDiffuse(
      Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0), Bxdf::Hemisphere::BRDF,
      testing::GetSpectralAllocator());
  ASSERT_FALSE(result);
}

TEST(LambertianBtdfTest, ReflectanceReflected) {
  MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(_)).WillRepeatedly(Return(1.0));

  LambertianBtdf bxdf(reflector);
  const Reflector* result = bxdf.ReflectanceDiffuse(
      Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0), Bxdf::Hemisphere::BRDF,
      testing::GetSpectralAllocator());
  ASSERT_FALSE(result);
}

TEST(LambertianBtdfTest, Reflectance) {
  MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(_)).WillRepeatedly(Return(1.0));

  LambertianBtdf bxdf(reflector);
  const Reflector* result = bxdf.ReflectanceDiffuse(
      Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, -1.0), Bxdf::Hemisphere::BTDF,
      testing::GetSpectralAllocator());
  ASSERT_TRUE(result);
  EXPECT_NEAR(std::numbers::inv_pi, result->Reflectance(1.0), 0.0001);
}

}  // namespace
}  // namespace bxdfs
}  // namespace iris