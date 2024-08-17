#include "iris/bsdf.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/bxdfs/mock_bxdf.h"
#include "iris/internal/arena.h"
#include "iris/random/mock_random.h"
#include "iris/reflectors/mock_reflector.h"

static const iris::Vector kTrueIncoming =
    iris::Normalize(iris::Vector(0.0, -1.0, -1.0));
static const iris::Vector kTrueBrdfOutgoing =
    iris::Normalize(iris::Vector(0.0, -1.0, 1.0));
static const iris::Vector kTrueBtdfOutgoing =
    iris::Normalize(iris::Vector(0.0, -1.0, -1.0));

static const iris::Vector kSurfaceNormal =
    iris::Normalize(iris::Vector(0.0, 0.0, 1.0));

static const iris::Vector kIncoming =
    iris::Normalize(iris::Vector(1.0, 0.0, 1.0));
static const iris::Vector kBrdfOutgoing =
    iris::Normalize(iris::Vector(-1.0, 0.0, 1.0));
static const iris::Vector kBtdfOutgoing =
    iris::Normalize(iris::Vector(-1.0, 0.0, -1.0));

static const iris::Vector kInvalid(1.0, 0.0, 0.0);

TEST(BsdfTest, SampleIncomingZeroDP) {
  iris::internal::Arena arena;
  iris::SpectralAllocator allocator(arena);

  iris::bxdfs::MockBxdf bxdf;

  iris::random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));

  iris::Bsdf bsdf(bxdf, kSurfaceNormal, kSurfaceNormal);
  auto result =
      bsdf.Sample(kInvalid, std::nullopt, iris::Sampler(rng), allocator);
  EXPECT_FALSE(result);
}

TEST(BsdfTest, SampleFails) {
  iris::internal::Arena arena;
  iris::SpectralAllocator allocator(arena);

  iris::bxdfs::MockBxdf bxdf;
  EXPECT_CALL(bxdf, Sample(kIncoming, testing::Eq(std::nullopt), kSurfaceNormal,
                           testing::_))
      .WillOnce(testing::Return(std::nullopt));

  iris::random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));

  iris::Bsdf bsdf(bxdf, kSurfaceNormal, kSurfaceNormal);
  auto result =
      bsdf.Sample(kTrueIncoming, std::nullopt, iris::Sampler(rng), allocator);
  EXPECT_FALSE(result);
}

TEST(BsdfTest, SampleOutgoingZeroDP) {
  iris::internal::Arena arena;
  iris::SpectralAllocator allocator(arena);

  iris::bxdfs::MockBxdf bxdf;
  EXPECT_CALL(bxdf, Sample(kIncoming, testing::Eq(std::nullopt), kSurfaceNormal,
                           testing::_))
      .WillOnce(testing::Return(
          iris::Bxdf::SampleResult{kInvalid, std::nullopt, &bxdf, 1.0}));

  iris::random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));

  iris::Bsdf bsdf(bxdf, kSurfaceNormal, kSurfaceNormal);
  auto result =
      bsdf.Sample(kTrueIncoming, std::nullopt, iris::Sampler(rng), allocator);
  EXPECT_FALSE(result);
}

TEST(BsdfTest, SampleZeroPdf) {
  iris::internal::Arena arena;
  iris::SpectralAllocator allocator(arena);

  iris::bxdfs::MockBxdf bxdf;
  EXPECT_CALL(bxdf, Sample(kIncoming, testing::Eq(std::nullopt), kSurfaceNormal,
                           testing::_))
      .WillOnce(testing::Return(
          iris::Bxdf::SampleResult{kBtdfOutgoing, std::nullopt, &bxdf, 1.0}));
  EXPECT_CALL(bxdf, Pdf(kIncoming, kBtdfOutgoing, kSurfaceNormal, &bxdf,
                        iris::Bxdf::Hemisphere::BTDF))
      .WillOnce(testing::Return(static_cast<iris::visual_t>(0.0)));

  iris::random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));

  iris::Bsdf bsdf(bxdf, kSurfaceNormal, kSurfaceNormal);
  auto result =
      bsdf.Sample(kTrueIncoming, std::nullopt, iris::Sampler(rng), allocator);
  EXPECT_FALSE(result);
}

TEST(BsdfTest, SampleNegativePdf) {
  iris::internal::Arena arena;
  iris::SpectralAllocator allocator(arena);

  iris::bxdfs::MockBxdf bxdf;
  EXPECT_CALL(bxdf, Sample(kIncoming, testing::Eq(std::nullopt), kSurfaceNormal,
                           testing::_))
      .WillOnce(testing::Return(
          iris::Bxdf::SampleResult{kBtdfOutgoing, std::nullopt, &bxdf, 1.0}));
  EXPECT_CALL(bxdf, Pdf(kIncoming, kBtdfOutgoing, kSurfaceNormal, &bxdf,
                        iris::Bxdf::Hemisphere::BTDF))
      .WillOnce(testing::Return(static_cast<iris::visual_t>(-1.0)));

  iris::random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));

  iris::Bsdf bsdf(bxdf, kSurfaceNormal, kSurfaceNormal);
  auto result =
      bsdf.Sample(kTrueIncoming, std::nullopt, iris::Sampler(rng), allocator);
  EXPECT_FALSE(result);
}

TEST(BsdfTest, SampleNoReflector) {
  iris::internal::Arena arena;
  iris::SpectralAllocator allocator(arena);

  iris::bxdfs::MockBxdf bxdf;
  EXPECT_CALL(bxdf, Sample(kIncoming, testing::Eq(std::nullopt), kSurfaceNormal,
                           testing::_))
      .WillOnce(testing::Return(
          iris::Bxdf::SampleResult{kBtdfOutgoing, std::nullopt, &bxdf, 1.0}));
  EXPECT_CALL(bxdf, Pdf(kIncoming, kBtdfOutgoing, kSurfaceNormal, &bxdf,
                        iris::Bxdf::Hemisphere::BTDF))
      .WillOnce(testing::Return(static_cast<iris::visual_t>(0.5)));
  EXPECT_CALL(bxdf, Reflectance(kIncoming, kBtdfOutgoing, &bxdf,
                                iris::Bxdf::Hemisphere::BTDF, testing::_))
      .WillOnce(testing::Return(nullptr));

  iris::random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));

  iris::Bsdf bsdf(bxdf, kSurfaceNormal, kSurfaceNormal);
  auto result =
      bsdf.Sample(kTrueIncoming, std::nullopt, iris::Sampler(rng), allocator);
  EXPECT_FALSE(result);
}

TEST(BsdfTest, SampleRespectsWeight) {
  iris::internal::Arena arena;
  iris::SpectralAllocator allocator(arena);
  iris::reflectors::MockReflector reflector;

  iris::bxdfs::MockBxdf bxdf;
  EXPECT_CALL(bxdf, Sample(kIncoming, testing::Eq(std::nullopt), kSurfaceNormal,
                           testing::_))
      .WillOnce(testing::Return(
          iris::Bxdf::SampleResult{kBtdfOutgoing, std::nullopt, &bxdf, 0.5}));
  EXPECT_CALL(bxdf, Pdf(kIncoming, kBtdfOutgoing, kSurfaceNormal, &bxdf,
                        iris::Bxdf::Hemisphere::BTDF))
      .WillOnce(testing::Return(static_cast<iris::visual_t>(0.5)));
  EXPECT_CALL(bxdf, Reflectance(kIncoming, kBtdfOutgoing, &bxdf,
                                iris::Bxdf::Hemisphere::BTDF, testing::_))
      .WillOnce(testing::Return(&reflector));

  iris::random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));

  iris::Bsdf bsdf(bxdf, kSurfaceNormal, kSurfaceNormal, true);
  auto result =
      bsdf.Sample(kTrueIncoming, std::nullopt, iris::Sampler(rng), allocator);
  EXPECT_TRUE(result);
  EXPECT_EQ(&reflector, &result->reflector);
  EXPECT_EQ(kTrueBtdfOutgoing, result->direction);
  EXPECT_FALSE(result->differentials);
  EXPECT_EQ(0.25, result->pdf);
  EXPECT_TRUE(result->diffuse);
}

TEST(BsdfTest, SampleBtdf) {
  iris::internal::Arena arena;
  iris::SpectralAllocator allocator(arena);
  iris::reflectors::MockReflector reflector;

  iris::bxdfs::MockBxdf bxdf;
  EXPECT_CALL(bxdf, Sample(kIncoming, testing::Eq(std::nullopt), kSurfaceNormal,
                           testing::_))
      .WillOnce(testing::Return(
          iris::Bxdf::SampleResult{kBtdfOutgoing, std::nullopt, &bxdf, 1.0}));
  EXPECT_CALL(bxdf, Pdf(kIncoming, kBtdfOutgoing, kSurfaceNormal, &bxdf,
                        iris::Bxdf::Hemisphere::BTDF))
      .WillOnce(testing::Return(static_cast<iris::visual_t>(0.5)));
  EXPECT_CALL(bxdf, Reflectance(kIncoming, kBtdfOutgoing, &bxdf,
                                iris::Bxdf::Hemisphere::BTDF, testing::_))
      .WillOnce(testing::Return(&reflector));

  iris::random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));

  iris::Bsdf bsdf(bxdf, kSurfaceNormal, kSurfaceNormal, true);
  auto result =
      bsdf.Sample(kTrueIncoming, std::nullopt, iris::Sampler(rng), allocator);
  EXPECT_TRUE(result);
  EXPECT_EQ(&reflector, &result->reflector);
  EXPECT_EQ(kTrueBtdfOutgoing, result->direction);
  EXPECT_FALSE(result->differentials);
  EXPECT_EQ(0.5, result->pdf);
  EXPECT_TRUE(result->diffuse);
}

TEST(BsdfTest, SampleBrdf) {
  iris::internal::Arena arena;
  iris::SpectralAllocator allocator(arena);
  iris::reflectors::MockReflector reflector;

  iris::bxdfs::MockBxdf bxdf;
  EXPECT_CALL(bxdf, Sample(kIncoming, testing::Eq(std::nullopt), kSurfaceNormal,
                           testing::_))
      .WillOnce(testing::Return(
          iris::Bxdf::SampleResult{kBrdfOutgoing, std::nullopt, &bxdf, 1.0}));
  EXPECT_CALL(bxdf, Pdf(kIncoming, kBrdfOutgoing, kSurfaceNormal, &bxdf,
                        iris::Bxdf::Hemisphere::BRDF))
      .WillOnce(testing::Return(static_cast<iris::visual_t>(0.5)));
  EXPECT_CALL(bxdf, Reflectance(kIncoming, kBrdfOutgoing, &bxdf,
                                iris::Bxdf::Hemisphere::BRDF, testing::_))
      .WillOnce(testing::Return(&reflector));

  iris::random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));

  iris::Bsdf bsdf(bxdf, kSurfaceNormal, kSurfaceNormal, true);
  auto result =
      bsdf.Sample(kTrueIncoming, std::nullopt, iris::Sampler(rng), allocator);
  EXPECT_TRUE(result);
  EXPECT_EQ(&reflector, &result->reflector);
  EXPECT_EQ(kTrueBrdfOutgoing, result->direction);
  EXPECT_EQ(0.5, result->pdf);
  EXPECT_TRUE(result->diffuse);
}

TEST(BsdfTest, SampleNoPdf) {
  iris::internal::Arena arena;
  iris::SpectralAllocator allocator(arena);
  iris::reflectors::MockReflector reflector;

  iris::bxdfs::MockBxdf bxdf;
  EXPECT_CALL(bxdf, Sample(kIncoming, testing::Eq(std::nullopt), kSurfaceNormal,
                           testing::_))
      .WillOnce(testing::Return(
          iris::Bxdf::SampleResult{kBtdfOutgoing, std::nullopt, &bxdf, 1.0}));
  EXPECT_CALL(bxdf, Pdf(kIncoming, kBtdfOutgoing, kSurfaceNormal, &bxdf,
                        iris::Bxdf::Hemisphere::BTDF))
      .WillOnce(testing::Return(std::nullopt));
  EXPECT_CALL(bxdf, Reflectance(kIncoming, kBtdfOutgoing, &bxdf,
                                iris::Bxdf::Hemisphere::BTDF, testing::_))
      .WillOnce(testing::Return(&reflector));

  iris::random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));

  iris::Bsdf bsdf(bxdf, kSurfaceNormal, kSurfaceNormal, true);
  auto result =
      bsdf.Sample(kTrueIncoming, std::nullopt, iris::Sampler(rng), allocator);
  EXPECT_TRUE(result);
  EXPECT_EQ(&reflector, &result->reflector);
  EXPECT_EQ(kTrueBtdfOutgoing, result->direction);
  EXPECT_FALSE(result->differentials);
  EXPECT_EQ(1.0, result->pdf);
  EXPECT_FALSE(result->diffuse);
}

TEST(BsdfTest, SampleWithInputDerivatives) {
  iris::internal::Arena arena;
  iris::SpectralAllocator allocator(arena);
  iris::reflectors::MockReflector reflector;

  iris::bxdfs::MockBxdf bxdf;
  EXPECT_CALL(bxdf, Sample(kIncoming, testing::Not(testing::Eq(std::nullopt)),
                           kSurfaceNormal, testing::_))
      .WillOnce(testing::Return(
          iris::Bxdf::SampleResult{kBtdfOutgoing, std::nullopt, &bxdf, 1.0}));
  EXPECT_CALL(bxdf, Pdf(kIncoming, kBtdfOutgoing, kSurfaceNormal, &bxdf,
                        iris::Bxdf::Hemisphere::BTDF))
      .WillOnce(testing::Return(std::nullopt));
  EXPECT_CALL(bxdf, Reflectance(kIncoming, kBtdfOutgoing, &bxdf,
                                iris::Bxdf::Hemisphere::BTDF, testing::_))
      .WillOnce(testing::Return(&reflector));

  iris::random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));

  iris::Bsdf bsdf(bxdf, kSurfaceNormal, kSurfaceNormal, true);
  auto result = bsdf.Sample(kTrueIncoming, {{kIncoming, kIncoming}},
                            iris::Sampler(rng), allocator);
  EXPECT_TRUE(result);
  EXPECT_EQ(&reflector, &result->reflector);
  EXPECT_EQ(kTrueBtdfOutgoing, result->direction);
  EXPECT_FALSE(result->differentials);
  EXPECT_EQ(1.0, result->pdf);
  EXPECT_FALSE(result->diffuse);
}

TEST(BsdfTest, SampleWithOutputDerivatives) {
  iris::internal::Arena arena;
  iris::SpectralAllocator allocator(arena);
  iris::reflectors::MockReflector reflector;

  iris::bxdfs::MockBxdf bxdf;
  EXPECT_CALL(bxdf, Sample(kIncoming, testing::Not(testing::Eq(std::nullopt)),
                           kSurfaceNormal, testing::_))
      .WillOnce(testing::Return(iris::Bxdf::SampleResult{
          kBtdfOutgoing, {{kBtdfOutgoing, kBtdfOutgoing}}, &bxdf, 1.0}));
  EXPECT_CALL(bxdf, Pdf(kIncoming, kBtdfOutgoing, kSurfaceNormal, &bxdf,
                        iris::Bxdf::Hemisphere::BTDF))
      .WillOnce(testing::Return(std::nullopt));
  EXPECT_CALL(bxdf, Reflectance(kIncoming, kBtdfOutgoing, &bxdf,
                                iris::Bxdf::Hemisphere::BTDF, testing::_))
      .WillOnce(testing::Return(&reflector));

  iris::random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));

  iris::Bsdf bsdf(bxdf, kSurfaceNormal, kSurfaceNormal, true);
  auto result = bsdf.Sample(kTrueIncoming, {{kIncoming, kIncoming}},
                            iris::Sampler(rng), allocator);
  EXPECT_TRUE(result);
  EXPECT_EQ(&reflector, &result->reflector);
  EXPECT_EQ(kTrueBtdfOutgoing, result->direction);
  ASSERT_TRUE(result->differentials);
  EXPECT_EQ(kTrueBtdfOutgoing, result->differentials->dx);
  EXPECT_EQ(kTrueBtdfOutgoing, result->differentials->dy);
  EXPECT_EQ(1.0, result->pdf);
  EXPECT_FALSE(result->diffuse);
}

TEST(BsdfTest, ReflectanceIncomingZeroDp) {
  iris::internal::Arena arena;
  iris::SpectralAllocator allocator(arena);

  iris::bxdfs::MockBxdf bxdf;

  iris::Bsdf bsdf(bxdf, kSurfaceNormal, kSurfaceNormal, true);
  auto result = bsdf.Reflectance(kInvalid, kTrueBtdfOutgoing, allocator);
  EXPECT_FALSE(result);
}

TEST(BsdfTest, ReflectanceOutgoingZeroDp) {
  iris::internal::Arena arena;
  iris::SpectralAllocator allocator(arena);

  iris::bxdfs::MockBxdf bxdf;

  iris::Bsdf bsdf(bxdf, kSurfaceNormal, kSurfaceNormal, true);
  auto result = bsdf.Reflectance(kTrueIncoming, kInvalid, allocator);
  EXPECT_FALSE(result);
}

TEST(BsdfTest, ReflectanceZeroPdf) {
  iris::internal::Arena arena;
  iris::SpectralAllocator allocator(arena);

  iris::bxdfs::MockBxdf bxdf;
  EXPECT_CALL(bxdf, Pdf(kIncoming, kBtdfOutgoing, kSurfaceNormal, nullptr,
                        iris::Bxdf::Hemisphere::BTDF))
      .WillOnce(testing::Return(static_cast<iris::visual_t>(0.0)));

  iris::Bsdf bsdf(bxdf, kSurfaceNormal, kSurfaceNormal, true);
  auto result = bsdf.Reflectance(kTrueIncoming, kTrueBtdfOutgoing, allocator);
  EXPECT_FALSE(result);
}

TEST(BsdfTest, ReflectanceNegativePdf) {
  iris::internal::Arena arena;
  iris::SpectralAllocator allocator(arena);

  iris::bxdfs::MockBxdf bxdf;
  EXPECT_CALL(bxdf, Pdf(kIncoming, kBtdfOutgoing, kSurfaceNormal, nullptr,
                        iris::Bxdf::Hemisphere::BTDF))
      .WillOnce(testing::Return(static_cast<iris::visual_t>(-1.0)));

  iris::Bsdf bsdf(bxdf, kSurfaceNormal, kSurfaceNormal, true);
  auto result = bsdf.Reflectance(kTrueIncoming, kTrueBtdfOutgoing, allocator);
  EXPECT_FALSE(result);
}

TEST(BsdfTest, ReflectanceNoPdf) {
  iris::internal::Arena arena;
  iris::SpectralAllocator allocator(arena);

  iris::bxdfs::MockBxdf bxdf;
  EXPECT_CALL(bxdf, Pdf(kIncoming, kBtdfOutgoing, kSurfaceNormal, nullptr,
                        iris::Bxdf::Hemisphere::BTDF))
      .WillOnce(testing::Return(std::nullopt));

  iris::Bsdf bsdf(bxdf, kSurfaceNormal, kSurfaceNormal, true);
  auto result = bsdf.Reflectance(kTrueIncoming, kTrueBtdfOutgoing, allocator);
  EXPECT_FALSE(result);
}

TEST(BsdfTest, ReflectanceNoReflector) {
  iris::internal::Arena arena;
  iris::SpectralAllocator allocator(arena);

  iris::bxdfs::MockBxdf bxdf;
  EXPECT_CALL(bxdf, Pdf(kIncoming, kBtdfOutgoing, kSurfaceNormal, nullptr,
                        iris::Bxdf::Hemisphere::BTDF))
      .WillOnce(testing::Return(static_cast<iris::visual>(0.5)));
  EXPECT_CALL(bxdf, Reflectance(kIncoming, kBtdfOutgoing, nullptr,
                                iris::Bxdf::Hemisphere::BTDF, testing::_))
      .WillOnce(testing::Return(nullptr));

  iris::Bsdf bsdf(bxdf, kSurfaceNormal, kSurfaceNormal, true);
  auto result = bsdf.Reflectance(kTrueIncoming, kTrueBtdfOutgoing, allocator);
  EXPECT_FALSE(result);
}

TEST(BsdfTest, ReflectanceBtdf) {
  iris::internal::Arena arena;
  iris::SpectralAllocator allocator(arena);
  iris::reflectors::MockReflector reflector;

  iris::bxdfs::MockBxdf bxdf;
  EXPECT_CALL(bxdf, Pdf(kIncoming, kBtdfOutgoing, kSurfaceNormal, nullptr,
                        iris::Bxdf::Hemisphere::BTDF))
      .WillOnce(testing::Return(static_cast<iris::visual_t>(0.5)));
  EXPECT_CALL(bxdf, Reflectance(kIncoming, kBtdfOutgoing, nullptr,
                                iris::Bxdf::Hemisphere::BTDF, testing::_))
      .WillOnce(testing::Return(&reflector));

  iris::Bsdf bsdf(bxdf, kSurfaceNormal, kSurfaceNormal, true);
  auto result = bsdf.Reflectance(kTrueIncoming, kTrueBtdfOutgoing, allocator);
  EXPECT_TRUE(result);
  EXPECT_EQ(&reflector, &result->reflector);
  EXPECT_EQ(0.5, result->pdf);
}

TEST(BsdfTest, ReflectanceBrdf) {
  iris::internal::Arena arena;
  iris::SpectralAllocator allocator(arena);
  iris::reflectors::MockReflector reflector;

  iris::bxdfs::MockBxdf bxdf;
  EXPECT_CALL(bxdf, Pdf(kIncoming, kBrdfOutgoing, kSurfaceNormal, nullptr,
                        iris::Bxdf::Hemisphere::BRDF))
      .WillOnce(testing::Return(static_cast<iris::visual_t>(0.5)));
  EXPECT_CALL(bxdf, Reflectance(kIncoming, kBrdfOutgoing, nullptr,
                                iris::Bxdf::Hemisphere::BRDF, testing::_))
      .WillOnce(testing::Return(&reflector));

  iris::Bsdf bsdf(bxdf, kSurfaceNormal, kSurfaceNormal, true);
  auto result = bsdf.Reflectance(kTrueIncoming, kTrueBrdfOutgoing, allocator);
  EXPECT_TRUE(result);
  EXPECT_EQ(&reflector, &result->reflector);
  EXPECT_EQ(0.5, result->pdf);
}

TEST(BsdfTest, Normalize) {
  iris::internal::Arena arena;
  iris::SpectralAllocator allocator(arena);
  iris::reflectors::MockReflector reflector;

  iris::bxdfs::MockBxdf bxdf;
  EXPECT_CALL(bxdf, Sample(kIncoming, testing::_, kSurfaceNormal, testing::_))
      .WillOnce(testing::Return(
          iris::Bxdf::SampleResult{kBtdfOutgoing, std::nullopt, &bxdf, 1.0}));
  EXPECT_CALL(bxdf, Pdf(kIncoming, kBtdfOutgoing, kSurfaceNormal, &bxdf,
                        iris::Bxdf::Hemisphere::BTDF))
      .WillOnce(testing::Return(static_cast<iris::visual_t>(0.5)));
  EXPECT_CALL(bxdf, Reflectance(kIncoming, kBtdfOutgoing, &bxdf,
                                iris::Bxdf::Hemisphere::BTDF, testing::_))
      .WillOnce(testing::Return(&reflector));

  iris::random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));

  iris::Bsdf bsdf(bxdf, iris::Vector(0.0, 0.0, 2.0),
                  iris::Vector(0.0, 0.0, 2.0), true);
  auto result =
      bsdf.Sample(kTrueIncoming, std::nullopt, iris::Sampler(rng), allocator);
  EXPECT_TRUE(result);
  EXPECT_EQ(&reflector, &result->reflector);
  EXPECT_EQ(kTrueBtdfOutgoing, result->direction);
  EXPECT_EQ(0.5, result->pdf);
  EXPECT_TRUE(result->diffuse);
}