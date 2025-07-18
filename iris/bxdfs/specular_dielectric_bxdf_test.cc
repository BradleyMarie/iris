#include "iris/bxdfs/specular_dielectric_bxdf.h"

#include <cmath>
#include <variant>

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
using ::iris::testing::GetSpectralAllocator;
using ::testing::_;
using ::testing::Return;

TEST(SpecularBxdfTest, Null) {
  MockReflector reflector;
  EXPECT_TRUE(MakeSpecularDielectricBxdf(GetBxdfAllocator(), &reflector,
                                         &reflector, 1.0, 1.0));
  EXPECT_TRUE(MakeSpecularDielectricBxdf(GetBxdfAllocator(), nullptr,
                                         &reflector, 1.0, 1.0));
  EXPECT_TRUE(MakeSpecularDielectricBxdf(GetBxdfAllocator(), &reflector,
                                         nullptr, 1.0, 1.0));
  EXPECT_FALSE(MakeSpecularDielectricBxdf(GetBxdfAllocator(), nullptr, nullptr,
                                          1.0, 1.0));
  EXPECT_FALSE(MakeSpecularDielectricBxdf(GetBxdfAllocator(), nullptr, nullptr,
                                          1.0, 1.0));
  EXPECT_FALSE(MakeSpecularDielectricBxdf(GetBxdfAllocator(), nullptr,
                                          &reflector, 0.0, 1.0));
  EXPECT_FALSE(MakeSpecularDielectricBxdf(GetBxdfAllocator(), nullptr,
                                          &reflector, 1.0, 0.0));
}

TEST(SpecularBxdfTest, SampleTransmittanceNoReflectance) {
  MockReflector reflector;
  MockReflector transmitter;
  MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));
  Sampler sampler(rng);

  const Bxdf* bxdf = MakeSpecularDielectricBxdf(GetBxdfAllocator(), nullptr,
                                                &transmitter, 1.0, 1.5);
  auto result = std::get<Bxdf::SpecularSample>(
      bxdf->Sample(Normalize(Vector(1.0, 1.0, 1.0)), std::nullopt,
                   Vector(0.0, 0.0, 1.0), sampler, GetSpectralAllocator()));
  EXPECT_EQ(result.hemisphere, Bxdf::Hemisphere::BTDF);
  EXPECT_NEAR(-0.3849, result.direction.x, 0.001);
  EXPECT_NEAR(-0.3849, result.direction.y, 0.001);
  EXPECT_NEAR(-0.8388, result.direction.z, 0.001);
  EXPECT_TRUE(result.reflectance);
  EXPECT_FALSE(result.differentials);
  EXPECT_EQ(result.pdf, 1.0);
  EXPECT_NEAR(result.etendue_conservation_scalar, 0.4444, 0.001);
}

TEST(SpecularBxdfTest, SampleTransmittanceFront) {
  MockReflector reflector;
  MockReflector transmitter;
  MockRandom rng;
  EXPECT_CALL(rng, NextGeometric()).WillOnce(Return(1.0));
  EXPECT_CALL(rng, DiscardGeometric(1));
  Sampler sampler(rng);

  const Bxdf* bxdf = MakeSpecularDielectricBxdf(GetBxdfAllocator(), &reflector,
                                                &transmitter, 1.0, 1.5);
  auto result = std::get<Bxdf::SpecularSample>(
      bxdf->Sample(Normalize(Vector(1.0, 1.0, 1.0)), std::nullopt,
                   Vector(0.0, 0.0, 1.0), sampler, GetSpectralAllocator()));
  EXPECT_EQ(result.hemisphere, Bxdf::Hemisphere::BTDF);
  EXPECT_NEAR(-0.3849, result.direction.x, 0.001);
  EXPECT_NEAR(-0.3849, result.direction.y, 0.001);
  EXPECT_NEAR(-0.8388, result.direction.z, 0.001);
  EXPECT_TRUE(result.reflectance);
  EXPECT_FALSE(result.differentials);
  EXPECT_NEAR(result.pdf, 0.9310, 0.001);
  EXPECT_NEAR(result.etendue_conservation_scalar, 0.4444, 0.001);
}

TEST(SpecularBxdfTest, SampleTransmittanceBack) {
  MockReflector reflector;
  MockReflector transmitter;
  MockRandom rng;
  EXPECT_CALL(rng, NextGeometric()).WillOnce(Return(1.0));
  EXPECT_CALL(rng, DiscardGeometric(1));
  Sampler sampler(rng);

  const Bxdf* bxdf = MakeSpecularDielectricBxdf(GetBxdfAllocator(), &reflector,
                                                &transmitter, 1.5, 1.0);
  auto result = std::get<Bxdf::SpecularSample>(
      bxdf->Sample(Normalize(Vector(1.0, 1.0, -1.0)), std::nullopt,
                   Vector(0.0, 0.0, 1.0), sampler, GetSpectralAllocator()));
  EXPECT_EQ(result.hemisphere, Bxdf::Hemisphere::BTDF);
  EXPECT_NEAR(-0.3849, result.direction.x, 0.001);
  EXPECT_NEAR(-0.3849, result.direction.y, 0.001);
  EXPECT_NEAR(0.8388, result.direction.z, 0.001);
  EXPECT_TRUE(result.reflectance);
  EXPECT_FALSE(result.differentials);
  EXPECT_NEAR(result.pdf, 0.9310, 0.001);
  EXPECT_NEAR(result.etendue_conservation_scalar, 0.4444, 0.001);
}

TEST(SpecularBxdfTest, SampleTransmittanceWithDerivatives) {
  MockReflector reflector;
  MockReflector transmitter;
  MockRandom rng;
  EXPECT_CALL(rng, NextGeometric()).WillOnce(Return(1.0));
  EXPECT_CALL(rng, DiscardGeometric(1));
  Sampler sampler(rng);

  const Bxdf* bxdf = MakeSpecularDielectricBxdf(GetBxdfAllocator(), &reflector,
                                                &transmitter, 1.0, 1.5);
  auto result = std::get<Bxdf::SpecularSample>(bxdf->Sample(
      Normalize(Vector(1.0, 1.0, 1.0)),
      {{Normalize(Vector(1.0, 0.5, 1.0)), Normalize(Vector(0.5, 1.0, 1.0))}},
      Vector(0.0, 0.0, 1.0), sampler, GetSpectralAllocator()));
  EXPECT_EQ(result.hemisphere, Bxdf::Hemisphere::BTDF);
  EXPECT_NEAR(-0.3849, result.direction.x, 0.001);
  EXPECT_NEAR(-0.3849, result.direction.y, 0.001);
  EXPECT_NEAR(-0.8388, result.direction.z, 0.001);
  EXPECT_TRUE(result.reflectance);
  ASSERT_TRUE(result.differentials);
  EXPECT_NEAR(-0.4444, result.differentials->dx.x, 0.001);
  EXPECT_NEAR(-0.2222, result.differentials->dx.y, 0.001);
  EXPECT_NEAR(-0.8678, result.differentials->dx.z, 0.001);
  EXPECT_NEAR(-0.2222, result.differentials->dy.x, 0.001);
  EXPECT_NEAR(-0.4444, result.differentials->dy.y, 0.001);
  EXPECT_NEAR(-0.8678, result.differentials->dy.z, 0.001);
  EXPECT_NEAR(result.pdf, 0.9310, 0.001);
  EXPECT_NEAR(result.etendue_conservation_scalar, 0.4444, 0.001);
}

TEST(SpecularBxdfTest, SampleReflectanceNoTransmittance) {
  MockReflector reflector;
  MockReflector transmitter;
  MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));
  Sampler sampler(rng);

  const Bxdf* bxdf = MakeSpecularDielectricBxdf(GetBxdfAllocator(), &reflector,
                                                nullptr, 1.0, 1.5);
  auto result = std::get<Bxdf::SpecularSample>(
      bxdf->Sample(Normalize(Vector(1.0, 1.0, 1.0)), std::nullopt,
                   Vector(0.0, 0.0, 1.0), sampler, GetSpectralAllocator()));
  EXPECT_EQ(result.hemisphere, Bxdf::Hemisphere::BRDF);
  EXPECT_NEAR(-0.5773, result.direction.x, 0.001);
  EXPECT_NEAR(-0.5773, result.direction.y, 0.001);
  EXPECT_NEAR(0.5773, result.direction.z, 0.001);
  EXPECT_FALSE(result.differentials);
  EXPECT_EQ(result.pdf, 1.0);
  EXPECT_EQ(result.etendue_conservation_scalar, 1.0);
}

TEST(SpecularBxdfTest, SampleReflectance) {
  MockReflector reflector;
  MockReflector transmitter;
  MockRandom rng;
  EXPECT_CALL(rng, NextGeometric()).WillOnce(Return(0.0));
  EXPECT_CALL(rng, DiscardGeometric(1));
  Sampler sampler(rng);

  const Bxdf* bxdf = MakeSpecularDielectricBxdf(GetBxdfAllocator(), &reflector,
                                                &transmitter, 1.0, 1.5);
  auto result = std::get<Bxdf::SpecularSample>(
      bxdf->Sample(Normalize(Vector(1.0, 1.0, 1.0)), std::nullopt,
                   Vector(0.0, 0.0, 1.0), sampler, GetSpectralAllocator()));
  EXPECT_EQ(result.hemisphere, Bxdf::Hemisphere::BRDF);
  EXPECT_NEAR(-0.5773, result.direction.x, 0.001);
  EXPECT_NEAR(-0.5773, result.direction.y, 0.001);
  EXPECT_NEAR(0.5773, result.direction.z, 0.001);
  EXPECT_FALSE(result.differentials);
  EXPECT_NEAR(result.pdf, 0.0689, 0.001);
  EXPECT_EQ(result.etendue_conservation_scalar, 1.0);
}

TEST(SpecularBxdfTest, SampleReflectanceWithDerivatives) {
  MockReflector reflector;
  MockReflector transmitter;
  MockRandom rng;
  EXPECT_CALL(rng, NextGeometric()).WillOnce(Return(0.0));
  EXPECT_CALL(rng, DiscardGeometric(1));
  Sampler sampler(rng);

  const Bxdf* bxdf = MakeSpecularDielectricBxdf(GetBxdfAllocator(), &reflector,
                                                &transmitter, 1.0, 1.5);
  auto result = std::get<Bxdf::SpecularSample>(bxdf->Sample(
      Normalize(Vector(1.0, 1.0, 1.0)),
      {{Normalize(Vector(1.0, 0.5, 1.0)), Normalize(Vector(0.5, 1.0, 1.0))}},
      Vector(0.0, 0.0, 1.0), sampler, GetSpectralAllocator()));
  EXPECT_EQ(result.hemisphere, Bxdf::Hemisphere::BRDF);
  EXPECT_NEAR(-0.5773, result.direction.x, 0.001);
  EXPECT_NEAR(-0.5773, result.direction.y, 0.001);
  EXPECT_NEAR(0.5773, result.direction.z, 0.001);
  ASSERT_TRUE(result.differentials);
  EXPECT_NEAR(-0.6666, result.differentials->dx.x, 0.001);
  EXPECT_NEAR(-0.3333, result.differentials->dx.y, 0.001);
  EXPECT_NEAR(0.6666, result.differentials->dx.z, 0.001);
  EXPECT_NEAR(-0.3333, result.differentials->dy.x, 0.001);
  EXPECT_NEAR(-0.6666, result.differentials->dy.y, 0.001);
  EXPECT_NEAR(0.6666, result.differentials->dy.z, 0.001);
  EXPECT_NEAR(result.pdf, 0.0689, 0.001);
  EXPECT_EQ(result.etendue_conservation_scalar, 1.0);
}

}  // namespace
}  // namespace bxdfs
}  // namespace iris
