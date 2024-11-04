#include "iris/bxdfs/specular_bxdf.h"

#include <cmath>

#include "googletest/include/gtest/gtest.h"
#include "iris/random/mock_random.h"
#include "iris/reflectors/mock_reflector.h"
#include "iris/testing/spectral_allocator.h"

namespace iris {
namespace bxdfs {
namespace {

using ::iris::random::MockRandom;
using ::iris::reflectors::MockReflector;
using ::iris::testing::GetSpectralAllocator;
using ::testing::_;
using ::testing::Return;

TEST(SpecularBrdfTest, Sample) {
  MockReflector reflector;
  MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));
  Sampler sampler(rng);

  SpecularBrdf bxdf(reflector, FresnelNoOp());
  std::optional<Bxdf::SpecularSample> result = bxdf.SampleSpecular(
      Vector(1.0, 1.0, 1.0), std::nullopt, Vector(0.0, 0.0, 1.0), sampler,
      GetSpectralAllocator());
  EXPECT_EQ(result->hemisphere, Bxdf::Hemisphere::BRDF);
  EXPECT_EQ(Vector(-1.0, -1.0, 1.0), result->direction);
  EXPECT_EQ(&result->reflectance, &reflector);
  EXPECT_FALSE(result->differentials);
  EXPECT_EQ(result->pdf, 1.0);
}

TEST(SpecularBrdfTest, SampleWithDerivatives) {
  MockReflector reflector;
  MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));
  Sampler sampler(rng);

  SpecularBrdf bxdf(reflector, FresnelNoOp());
  std::optional<Bxdf::SpecularSample> result = bxdf.SampleSpecular(
      Vector(1.0, 1.0, 1.0), {{Vector(1.0, 0.5, 1.0), Vector(0.5, 1.0, 1.0)}},
      Vector(0.0, 0.0, 1.0), sampler, GetSpectralAllocator());
  EXPECT_EQ(result->hemisphere, Bxdf::Hemisphere::BRDF);
  EXPECT_EQ(Vector(-1.0, -1.0, 1.0), result->direction);
  EXPECT_EQ(&result->reflectance, &reflector);
  ASSERT_TRUE(result->differentials);
  EXPECT_EQ(Vector(-1.0, -0.5, 1.0), result->differentials->dx);
  EXPECT_EQ(Vector(-0.5, -1.0, 1.0), result->differentials->dy);
  EXPECT_EQ(result->pdf, 1.0);
}

TEST(SpecularBtdfTest, SampleFront) {
  MockReflector reflector;
  MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));
  Sampler sampler(rng);

  SpecularBtdf bxdf(reflector, 1.0, 1.0, FresnelNoOp());
  std::optional<Bxdf::SpecularSample> result = bxdf.SampleSpecular(
      Vector(1.0, 1.0, 1.0), std::nullopt, Vector(0.0, 0.0, 1.0), sampler,
      GetSpectralAllocator());
  EXPECT_EQ(result->hemisphere, Bxdf::Hemisphere::BTDF);
  EXPECT_EQ(Vector(-1.0, -1.0, -1.0), result->direction);
  EXPECT_EQ(&result->reflectance, &reflector);
  EXPECT_FALSE(result->differentials);
  EXPECT_EQ(result->pdf, 1.0);
}

TEST(SpecularBtdfTest, SampleBack) {
  MockReflector reflector;
  MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));
  Sampler sampler(rng);

  SpecularBtdf bxdf(reflector, 1.0, 1.0, FresnelNoOp());
  std::optional<Bxdf::SpecularSample> result = bxdf.SampleSpecular(
      Vector(1.0, 1.0, -1.0), std::nullopt, Vector(0.0, 0.0, 1.0), sampler,
      GetSpectralAllocator());
  EXPECT_EQ(result->hemisphere, Bxdf::Hemisphere::BTDF);
  EXPECT_EQ(Vector(-1.0, -1.0, 1.0), result->direction);
  EXPECT_EQ(&result->reflectance, &reflector);
  EXPECT_FALSE(result->differentials);
  EXPECT_EQ(result->pdf, 1.0);
}

TEST(SpecularBtdfTest, SampleWithDerivatives) {
  MockReflector reflector;
  MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));
  Sampler sampler(rng);

  SpecularBtdf bxdf(reflector, 1.0, 1.0, FresnelNoOp());
  std::optional<Bxdf::SpecularSample> result = bxdf.SampleSpecular(
      Vector(1.0, 1.0, 1.0), {{Vector(1.0, 0.5, 1.0), Vector(0.5, 1.0, 1.0)}},
      Vector(0.0, 0.0, 1.0), sampler, GetSpectralAllocator());
  EXPECT_EQ(result->hemisphere, Bxdf::Hemisphere::BTDF);
  EXPECT_EQ(Vector(-1.0, -1.0, -1.0), result->direction);
  EXPECT_EQ(&result->reflectance, &reflector);
  ASSERT_TRUE(result->differentials);
  EXPECT_EQ(Vector(-1.0, -0.5, -1.0), result->differentials->dx);
  EXPECT_EQ(Vector(-0.5, -1.0, -1.0), result->differentials->dy);
  EXPECT_EQ(result->pdf, 1.0);
}

TEST(SpecularBxdfTest, SampleTransmittanceFront) {
  MockReflector reflector;
  MockReflector transmitter;
  MockRandom rng;
  EXPECT_CALL(rng, NextGeometric()).WillOnce(Return(1.0));
  EXPECT_CALL(rng, DiscardGeometric(1));
  Sampler sampler(rng);

  SpecularBxdf bxdf(reflector, transmitter, 1.0, 1.0);
  std::optional<Bxdf::SpecularSample> result = bxdf.SampleSpecular(
      Vector(1.0, 1.0, 1.0), std::nullopt, Vector(0.0, 0.0, 1.0), sampler,
      GetSpectralAllocator());
  EXPECT_EQ(result->hemisphere, Bxdf::Hemisphere::BTDF);
  EXPECT_EQ(Vector(-1.0, -1.0, -1.0), result->direction);
  EXPECT_EQ(&result->reflectance, &transmitter);
  EXPECT_FALSE(result->differentials);
  EXPECT_EQ(result->pdf, 1.0);
}

TEST(SpecularBxdfTest, SampleTransmittanceBack) {
  MockReflector reflector;
  MockReflector transmitter;
  MockRandom rng;
  EXPECT_CALL(rng, NextGeometric()).WillOnce(Return(1.0));
  EXPECT_CALL(rng, DiscardGeometric(1));
  Sampler sampler(rng);

  SpecularBxdf bxdf(reflector, transmitter, 1.0, 1.0);
  std::optional<Bxdf::SpecularSample> result = bxdf.SampleSpecular(
      Vector(1.0, 1.0, -1.0), std::nullopt, Vector(0.0, 0.0, 1.0), sampler,
      GetSpectralAllocator());
  EXPECT_EQ(result->hemisphere, Bxdf::Hemisphere::BTDF);
  EXPECT_EQ(Vector(-1.0, -1.0, 1.0), result->direction);
  EXPECT_EQ(&result->reflectance, &transmitter);
  EXPECT_FALSE(result->differentials);
  EXPECT_EQ(result->pdf, 1.0);
}

TEST(SpecularBxdfTest, SampleTransmittanceWithDerivatives) {
  MockReflector reflector;
  MockReflector transmitter;
  MockRandom rng;
  EXPECT_CALL(rng, NextGeometric()).WillOnce(Return(1.0));
  EXPECT_CALL(rng, DiscardGeometric(1));
  Sampler sampler(rng);

  SpecularBxdf bxdf(reflector, transmitter, 1.0, 1.0);
  std::optional<Bxdf::SpecularSample> result = bxdf.SampleSpecular(
      Vector(1.0, 1.0, 1.0), {{Vector(1.0, 0.5, 1.0), Vector(0.5, 1.0, 1.0)}},
      Vector(0.0, 0.0, 1.0), sampler, GetSpectralAllocator());
  EXPECT_EQ(result->hemisphere, Bxdf::Hemisphere::BTDF);
  EXPECT_EQ(Vector(-1.0, -1.0, -1.0), result->direction);
  EXPECT_EQ(&result->reflectance, &transmitter);
  ASSERT_TRUE(result->differentials);
  EXPECT_EQ(Vector(-1.0, -0.5, -1.0), result->differentials->dx);
  EXPECT_EQ(Vector(-0.5, -1.0, -1.0), result->differentials->dy);
  EXPECT_EQ(result->pdf, 1.0);
}

TEST(SpecularBxdfTest, SampleReflectance) {
  MockReflector reflector;
  MockReflector transmitter;
  MockRandom rng;
  EXPECT_CALL(rng, NextGeometric()).WillOnce(Return(0.0));
  EXPECT_CALL(rng, DiscardGeometric(1));
  Sampler sampler(rng);

  SpecularBxdf bxdf(reflector, transmitter, 1.0, 1.5);
  std::optional<Bxdf::SpecularSample> result = bxdf.SampleSpecular(
      Vector(1.0, 1.0, 1.0), std::nullopt, Vector(0.0, 0.0, 1.0), sampler,
      GetSpectralAllocator());
  EXPECT_EQ(result->hemisphere, Bxdf::Hemisphere::BRDF);
  EXPECT_EQ(Vector(-1.0, -1.0, 1.0), result->direction);
  EXPECT_FALSE(result->differentials);
  EXPECT_NEAR(result->pdf, 0.04, 0.001);
}

TEST(SpecularBxdfTest, SampleReflectanceWithDerivatives) {
  MockReflector reflector;
  MockReflector transmitter;
  MockRandom rng;
  EXPECT_CALL(rng, NextGeometric()).WillOnce(Return(0.0));
  EXPECT_CALL(rng, DiscardGeometric(1));
  Sampler sampler(rng);

  SpecularBxdf bxdf(reflector, transmitter, 1.0, 1.5);
  std::optional<Bxdf::SpecularSample> result = bxdf.SampleSpecular(
      Vector(1.0, 1.0, 1.0), {{Vector(1.0, 0.5, 1.0), Vector(0.5, 1.0, 1.0)}},
      Vector(0.0, 0.0, 1.0), sampler, GetSpectralAllocator());
  EXPECT_EQ(result->hemisphere, Bxdf::Hemisphere::BRDF);
  EXPECT_EQ(Vector(-1.0, -1.0, 1.0), result->direction);
  ASSERT_TRUE(result->differentials);
  EXPECT_EQ(Vector(-1.0, -0.5, 1.0), result->differentials->dx);
  EXPECT_EQ(Vector(-0.5, -1.0, 1.0), result->differentials->dy);
  EXPECT_NEAR(result->pdf, 0.04, 0.001);
}

}  // namespace
}  // namespace bxdfs
}  // namespace iris