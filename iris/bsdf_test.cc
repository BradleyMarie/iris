#include "iris/bsdf.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/bxdfs/mock_bxdf.h"
#include "iris/internal/arena.h"
#include "iris/random/mock_random.h"
#include "iris/reflectors/mock_reflector.h"

TEST(BsdfTest, SampleFails) {
  iris::internal::Arena arena;
  iris::SpectralAllocator allocator(arena);

  iris::bxdfs::MockBxdf bxdf;
  EXPECT_CALL(bxdf,
              Sample(iris::Vector(0.0, 1.0, 0.0), testing::Eq(std::nullopt),
                     iris::Vector(0.0, 0.0, 1.0), testing::_))
      .WillOnce(testing::Return(std::nullopt));

  iris::random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));

  iris::Bsdf bsdf(bxdf, iris::Vector(0.0, 0.0, 1.0),
                  iris::Vector(0.0, 0.0, 1.0));
  auto result = bsdf.Sample(iris::Vector(1.0, 0.0, 0.0), std::nullopt,
                            iris::Sampler(rng), allocator);
  EXPECT_FALSE(result);
}

TEST(BsdfTest, SampleZeroPdf) {
  iris::internal::Arena arena;
  iris::SpectralAllocator allocator(arena);

  iris::bxdfs::MockBxdf bxdf;
  EXPECT_CALL(bxdf,
              Sample(iris::Vector(0.0, 1.0, 0.0), testing::Eq(std::nullopt),
                     iris::Vector(0.0, 0.0, 1.0), testing::_))
      .WillOnce(testing::Return(
          iris::Bxdf::SampleResult{iris::Vector(1.0, 0.0, 0.0)}));
  EXPECT_CALL(
      bxdf,
      Pdf(iris::Vector(0.0, 1.0, 0.0), iris::Vector(1.0, 0.0, 0.0),
          iris::Vector(0.0, 0.0, 1.0), nullptr, iris::Bxdf::Hemisphere::BTDF))
      .WillOnce(testing::Return(static_cast<iris::visual_t>(0.0)));

  iris::random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));

  iris::Bsdf bsdf(bxdf, iris::Vector(0.0, 0.0, 1.0),
                  iris::Vector(0.0, 0.0, 1.0));
  auto result = bsdf.Sample(iris::Vector(1.0, 0.0, 0.0), std::nullopt,
                            iris::Sampler(rng), allocator);
  EXPECT_FALSE(result);
}

TEST(BsdfTest, SampleNegativePdf) {
  iris::internal::Arena arena;
  iris::SpectralAllocator allocator(arena);

  iris::bxdfs::MockBxdf bxdf;
  EXPECT_CALL(bxdf,
              Sample(iris::Vector(0.0, 1.0, 0.0), testing::Eq(std::nullopt),
                     iris::Vector(0.0, 0.0, 1.0), testing::_))
      .WillOnce(testing::Return(
          iris::Bxdf::SampleResult{iris::Vector(1.0, 0.0, 0.0)}));
  EXPECT_CALL(
      bxdf,
      Pdf(iris::Vector(0.0, 1.0, 0.0), iris::Vector(1.0, 0.0, 0.0),
          iris::Vector(0.0, 0.0, 1.0), nullptr, iris::Bxdf::Hemisphere::BTDF))
      .WillOnce(testing::Return(static_cast<iris::visual_t>(-1.0)));

  iris::random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));

  iris::Bsdf bsdf(bxdf, iris::Vector(0.0, 0.0, 1.0),
                  iris::Vector(0.0, 0.0, 1.0));
  auto result = bsdf.Sample(iris::Vector(1.0, 0.0, 0.0), std::nullopt,
                            iris::Sampler(rng), allocator);
  EXPECT_FALSE(result);
}

TEST(BsdfTest, SampleNoReflector) {
  iris::internal::Arena arena;
  iris::SpectralAllocator allocator(arena);

  iris::bxdfs::MockBxdf bxdf;
  EXPECT_CALL(bxdf,
              Sample(iris::Vector(0.0, 1.0, 0.0), testing::Eq(std::nullopt),
                     iris::Vector(0.0, 0.0, 1.0), testing::_))
      .WillOnce(testing::Return(
          iris::Bxdf::SampleResult{iris::Vector(1.0, 0.0, 0.0)}));
  EXPECT_CALL(
      bxdf,
      Pdf(iris::Vector(0.0, 1.0, 0.0), iris::Vector(1.0, 0.0, 0.0),
          iris::Vector(0.0, 0.0, 1.0), nullptr, iris::Bxdf::Hemisphere::BTDF))
      .WillOnce(testing::Return(static_cast<iris::visual_t>(0.5)));
  EXPECT_CALL(bxdf, Reflectance(iris::Vector(0.0, 1.0, 0.0),
                                iris::Vector(1.0, 0.0, 0.0), nullptr,
                                iris::Bxdf::Hemisphere::BTDF, testing::_))
      .WillOnce(testing::Return(nullptr));

  iris::random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));

  iris::Bsdf bsdf(bxdf, iris::Vector(0.0, 0.0, 1.0),
                  iris::Vector(0.0, 0.0, 1.0));
  auto result = bsdf.Sample(iris::Vector(1.0, 0.0, 0.0), std::nullopt,
                            iris::Sampler(rng), allocator);
  EXPECT_FALSE(result);
}

TEST(BsdfTest, SampleBtdf) {
  iris::internal::Arena arena;
  iris::SpectralAllocator allocator(arena);
  iris::reflectors::MockReflector reflector;

  iris::bxdfs::MockBxdf bxdf;
  EXPECT_CALL(bxdf,
              Sample(iris::Vector(0.0, 1.0, 0.0), testing::Eq(std::nullopt),
                     iris::Vector(0.0, 0.0, 1.0), testing::_))
      .WillOnce(testing::Return(
          iris::Bxdf::SampleResult{iris::Vector(1.0, 0.0, 0.0)}));
  EXPECT_CALL(
      bxdf,
      Pdf(iris::Vector(0.0, 1.0, 0.0), iris::Vector(1.0, 0.0, 0.0),
          iris::Vector(0.0, 0.0, 1.0), nullptr, iris::Bxdf::Hemisphere::BTDF))
      .WillOnce(testing::Return(static_cast<iris::visual_t>(0.5)));
  EXPECT_CALL(bxdf, Reflectance(iris::Vector(0.0, 1.0, 0.0),
                                iris::Vector(1.0, 0.0, 0.0), nullptr,
                                iris::Bxdf::Hemisphere::BTDF, testing::_))
      .WillOnce(testing::Return(&reflector));

  iris::random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));

  iris::Bsdf bsdf(bxdf, iris::Vector(0.0, 0.0, 1.0),
                  iris::Vector(0.0, 0.0, 1.0), true);
  auto result = bsdf.Sample(iris::Vector(1.0, 0.0, 0.0), std::nullopt,
                            iris::Sampler(rng), allocator);
  EXPECT_TRUE(result);
  EXPECT_EQ(&reflector, &result->reflector);
  EXPECT_EQ(iris::Vector(0.0, 1.0, 0.0), result->direction);
  EXPECT_FALSE(result->differentials);
  EXPECT_EQ(0.5, result->pdf.value());
}

TEST(BsdfTest, SampleBrdf) {
  iris::internal::Arena arena;
  iris::SpectralAllocator allocator(arena);
  iris::reflectors::MockReflector reflector;

  iris::bxdfs::MockBxdf bxdf;
  EXPECT_CALL(bxdf,
              Sample(iris::Vector(0.0, 0.0, -1.0), testing::Eq(std::nullopt),
                     iris::Vector(0.0, 0.0, 1.0), testing::_))
      .WillOnce(testing::Return(
          iris::Bxdf::SampleResult{iris::Vector(0.0, 0.0, -1.0)}));
  EXPECT_CALL(
      bxdf,
      Pdf(iris::Vector(0.0, 0.0, -1.0), iris::Vector(0.0, 0.0, -1.0),
          iris::Vector(0.0, 0.0, 1.0), nullptr, iris::Bxdf::Hemisphere::BRDF))
      .WillOnce(testing::Return(static_cast<iris::visual_t>(0.5)));
  EXPECT_CALL(bxdf, Reflectance(iris::Vector(0.0, 0.0, -1.0),
                                iris::Vector(0.0, 0.0, -1.0), nullptr,
                                iris::Bxdf::Hemisphere::BRDF, testing::_))
      .WillOnce(testing::Return(&reflector));

  iris::random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));

  iris::Bsdf bsdf(bxdf, iris::Vector(0.0, 0.0, 1.0),
                  iris::Vector(0.0, 0.0, 1.0), true);
  auto result = bsdf.Sample(iris::Vector(0.0, 0.0, 1.0), std::nullopt,
                            iris::Sampler(rng), allocator);
  EXPECT_TRUE(result);
  EXPECT_EQ(&reflector, &result->reflector);
  EXPECT_EQ(iris::Vector(0.0, 0.0, -1.0), result->direction);
  EXPECT_FALSE(result->differentials);
  EXPECT_EQ(0.5, result->pdf.value());
}

TEST(BsdfTest, SampleNoPdf) {
  iris::internal::Arena arena;
  iris::SpectralAllocator allocator(arena);
  iris::reflectors::MockReflector reflector;

  iris::bxdfs::MockBxdf bxdf;
  EXPECT_CALL(bxdf,
              Sample(iris::Vector(0.0, 1.0, 0.0), testing::Eq(std::nullopt),
                     iris::Vector(0.0, 0.0, 1.0), testing::_))
      .WillOnce(testing::Return(
          iris::Bxdf::SampleResult{iris::Vector(1.0, 0.0, 0.0)}));
  EXPECT_CALL(
      bxdf,
      Pdf(iris::Vector(0.0, 1.0, 0.0), iris::Vector(1.0, 0.0, 0.0),
          iris::Vector(0.0, 0.0, 1.0), nullptr, iris::Bxdf::Hemisphere::BTDF))
      .WillOnce(testing::Return(std::nullopt));
  EXPECT_CALL(bxdf, Reflectance(iris::Vector(0.0, 1.0, 0.0),
                                iris::Vector(1.0, 0.0, 0.0), nullptr,
                                iris::Bxdf::Hemisphere::BTDF, testing::_))
      .WillOnce(testing::Return(&reflector));

  iris::random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));

  iris::Bsdf bsdf(bxdf, iris::Vector(0.0, 0.0, 1.0),
                  iris::Vector(0.0, 0.0, 1.0), true);
  auto result = bsdf.Sample(iris::Vector(1.0, 0.0, 0.0), std::nullopt,
                            iris::Sampler(rng), allocator);
  EXPECT_TRUE(result);
  EXPECT_EQ(&reflector, &result->reflector);
  EXPECT_EQ(iris::Vector(0.0, 1.0, 0.0), result->direction);
  EXPECT_FALSE(result->differentials);
  EXPECT_FALSE(result->pdf);
}

TEST(BsdfTest, SampleWithInputDerivatives) {
  iris::internal::Arena arena;
  iris::SpectralAllocator allocator(arena);
  iris::reflectors::MockReflector reflector;

  iris::bxdfs::MockBxdf bxdf;
  EXPECT_CALL(bxdf, Sample(iris::Vector(0.0, 1.0, 0.0),
                           testing::Not(testing::Eq(std::nullopt)),
                           iris::Vector(0.0, 0.0, 1.0), testing::_))
      .WillOnce(testing::Return(
          iris::Bxdf::SampleResult{iris::Vector(1.0, 0.0, 0.0)}));
  EXPECT_CALL(
      bxdf,
      Pdf(iris::Vector(0.0, 1.0, 0.0), iris::Vector(1.0, 0.0, 0.0),
          iris::Vector(0.0, 0.0, 1.0), nullptr, iris::Bxdf::Hemisphere::BTDF))
      .WillOnce(testing::Return(std::nullopt));
  EXPECT_CALL(bxdf, Reflectance(iris::Vector(0.0, 1.0, 0.0),
                                iris::Vector(1.0, 0.0, 0.0), nullptr,
                                iris::Bxdf::Hemisphere::BTDF, testing::_))
      .WillOnce(testing::Return(&reflector));

  iris::random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));

  iris::Bsdf bsdf(bxdf, iris::Vector(0.0, 0.0, 1.0),
                  iris::Vector(0.0, 0.0, 1.0), true);
  auto result =
      bsdf.Sample(iris::Vector(1.0, 0.0, 0.0),
                  {{iris::Vector(1.0, 0.0, 0.0), iris::Vector(1.0, 0.0, 0.0)}},
                  iris::Sampler(rng), allocator);
  EXPECT_TRUE(result);
  EXPECT_EQ(&reflector, &result->reflector);
  EXPECT_EQ(iris::Vector(0.0, 1.0, 0.0), result->direction);
  EXPECT_FALSE(result->differentials);
  EXPECT_FALSE(result->pdf);
}

TEST(BsdfTest, SampleWithOutputDerivatives) {
  iris::internal::Arena arena;
  iris::SpectralAllocator allocator(arena);
  iris::reflectors::MockReflector reflector;

  iris::bxdfs::MockBxdf bxdf;
  EXPECT_CALL(bxdf, Sample(iris::Vector(0.0, 1.0, 0.0),
                           testing::Not(testing::Eq(std::nullopt)),
                           iris::Vector(0.0, 0.0, 1.0), testing::_))
      .WillOnce(testing::Return(iris::Bxdf::SampleResult{
          iris::Vector(1.0, 0.0, 0.0),
          {{iris::Vector(1.0, 0.0, 0.0), iris::Vector(1.0, 0.0, 0.0)}}}));
  EXPECT_CALL(
      bxdf,
      Pdf(iris::Vector(0.0, 1.0, 0.0), iris::Vector(1.0, 0.0, 0.0),
          iris::Vector(0.0, 0.0, 1.0), nullptr, iris::Bxdf::Hemisphere::BTDF))
      .WillOnce(testing::Return(std::nullopt));
  EXPECT_CALL(bxdf, Reflectance(iris::Vector(0.0, 1.0, 0.0),
                                iris::Vector(1.0, 0.0, 0.0), nullptr,
                                iris::Bxdf::Hemisphere::BTDF, testing::_))
      .WillOnce(testing::Return(&reflector));

  iris::random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));

  iris::Bsdf bsdf(bxdf, iris::Vector(0.0, 0.0, 1.0),
                  iris::Vector(0.0, 0.0, 1.0), true);
  auto result =
      bsdf.Sample(iris::Vector(1.0, 0.0, 0.0),
                  {{iris::Vector(1.0, 0.0, 0.0), iris::Vector(1.0, 0.0, 0.0)}},
                  iris::Sampler(rng), allocator);
  EXPECT_TRUE(result);
  EXPECT_EQ(&reflector, &result->reflector);
  EXPECT_EQ(iris::Vector(0.0, 1.0, 0.0), result->direction);
  ASSERT_TRUE(result->differentials);
  EXPECT_EQ(iris::Vector(0.0, 1.0, 0.0), result->differentials->dx);
  EXPECT_EQ(iris::Vector(0.0, 1.0, 0.0), result->differentials->dy);
  EXPECT_FALSE(result->pdf);
}

TEST(BsdfTest, ReflectanceZeroPdf) {
  iris::internal::Arena arena;
  iris::SpectralAllocator allocator(arena);

  iris::bxdfs::MockBxdf bxdf;
  EXPECT_CALL(
      bxdf,
      Pdf(iris::Vector(0.0, 1.0, 0.0), iris::Vector(0.0, -1.0, 0.0),
          iris::Vector(0.0, 0.0, 1.0), nullptr, iris::Bxdf::Hemisphere::BTDF))
      .WillOnce(testing::Return(static_cast<iris::visual_t>(0.0)));

  iris::Bsdf bsdf(bxdf, iris::Vector(0.0, 0.0, 1.0),
                  iris::Vector(0.0, 0.0, 1.0), true);
  auto result = bsdf.Reflectance(iris::Vector(1.0, 0.0, 0.0),
                                 iris::Vector(1.0, 0.0, 0.0), allocator);
  EXPECT_FALSE(result);
}

TEST(BsdfTest, ReflectanceNegativePdf) {
  iris::internal::Arena arena;
  iris::SpectralAllocator allocator(arena);

  iris::bxdfs::MockBxdf bxdf;
  EXPECT_CALL(
      bxdf,
      Pdf(iris::Vector(0.0, 1.0, 0.0), iris::Vector(0.0, -1.0, 0.0),
          iris::Vector(0.0, 0.0, 1.0), nullptr, iris::Bxdf::Hemisphere::BTDF))
      .WillOnce(testing::Return(static_cast<iris::visual_t>(-1.0)));

  iris::Bsdf bsdf(bxdf, iris::Vector(0.0, 0.0, 1.0),
                  iris::Vector(0.0, 0.0, 1.0), true);
  auto result = bsdf.Reflectance(iris::Vector(1.0, 0.0, 0.0),
                                 iris::Vector(1.0, 0.0, 0.0), allocator);
  EXPECT_FALSE(result);
}

TEST(BsdfTest, ReflectanceNoPdf) {
  iris::internal::Arena arena;
  iris::SpectralAllocator allocator(arena);

  iris::bxdfs::MockBxdf bxdf;
  EXPECT_CALL(
      bxdf,
      Pdf(iris::Vector(0.0, 1.0, 0.0), iris::Vector(0.0, -1.0, 0.0),
          iris::Vector(0.0, 0.0, 1.0), nullptr, iris::Bxdf::Hemisphere::BTDF))
      .WillOnce(testing::Return(std::nullopt));

  iris::Bsdf bsdf(bxdf, iris::Vector(0.0, 0.0, 1.0),
                  iris::Vector(0.0, 0.0, 1.0), true);
  auto result = bsdf.Reflectance(iris::Vector(1.0, 0.0, 0.0),
                                 iris::Vector(1.0, 0.0, 0.0), allocator);
  EXPECT_FALSE(result);
}

TEST(BsdfTest, ReflectanceNoReflector) {
  iris::internal::Arena arena;
  iris::SpectralAllocator allocator(arena);

  iris::bxdfs::MockBxdf bxdf;
  EXPECT_CALL(
      bxdf,
      Pdf(iris::Vector(0.0, 1.0, 0.0), iris::Vector(0.0, -1.0, 0.0),
          iris::Vector(0.0, 0.0, 1.0), nullptr, iris::Bxdf::Hemisphere::BTDF))
      .WillOnce(testing::Return(static_cast<iris::visual>(0.5)));
  EXPECT_CALL(bxdf, Reflectance(iris::Vector(0.0, 1.0, 0.0),
                                iris::Vector(0.0, -1.0, 0.0), nullptr,
                                iris::Bxdf::Hemisphere::BTDF, testing::_))
      .WillOnce(testing::Return(nullptr));

  iris::Bsdf bsdf(bxdf, iris::Vector(0.0, 0.0, 1.0),
                  iris::Vector(0.0, 0.0, 1.0), true);
  auto result = bsdf.Reflectance(iris::Vector(1.0, 0.0, 0.0),
                                 iris::Vector(1.0, 0.0, 0.0), allocator);
  EXPECT_FALSE(result);
}

TEST(BsdfTest, ReflectanceBtdf) {
  iris::internal::Arena arena;
  iris::SpectralAllocator allocator(arena);
  iris::reflectors::MockReflector reflector;

  iris::bxdfs::MockBxdf bxdf;
  EXPECT_CALL(
      bxdf,
      Pdf(iris::Vector(0.0, 1.0, 0.0), iris::Vector(0.0, -1.0, 0.0),
          iris::Vector(0.0, 0.0, 1.0), nullptr, iris::Bxdf::Hemisphere::BTDF))
      .WillOnce(testing::Return(static_cast<iris::visual_t>(0.5)));
  EXPECT_CALL(bxdf, Reflectance(iris::Vector(0.0, 1.0, 0.0),
                                iris::Vector(0.0, -1.0, 0.0), nullptr,
                                iris::Bxdf::Hemisphere::BTDF, testing::_))
      .WillOnce(testing::Return(&reflector));

  iris::Bsdf bsdf(bxdf, iris::Vector(0.0, 0.0, 1.0),
                  iris::Vector(0.0, 0.0, 1.0), true);
  auto result = bsdf.Reflectance(iris::Vector(1.0, 0.0, 0.0),
                                 iris::Vector(1.0, 0.0, 0.0), allocator);
  EXPECT_TRUE(result);
  EXPECT_EQ(&reflector, &result->reflector);
  EXPECT_EQ(0.5, result->pdf);
}

TEST(BsdfTest, ReflectanceBrdf) {
  iris::internal::Arena arena;
  iris::SpectralAllocator allocator(arena);
  iris::reflectors::MockReflector reflector;

  iris::bxdfs::MockBxdf bxdf;
  EXPECT_CALL(
      bxdf,
      Pdf(iris::Vector(0.0, 0.0, -1.0), iris::Vector(0.0, 0.0, -1.0),
          iris::Vector(0.0, 0.0, 1.0), nullptr, iris::Bxdf::Hemisphere::BRDF))
      .WillOnce(testing::Return(static_cast<iris::visual_t>(0.5)));
  EXPECT_CALL(bxdf, Reflectance(iris::Vector(0.0, 0.0, -1.0),
                                iris::Vector(0.0, 0.0, -1.0), nullptr,
                                iris::Bxdf::Hemisphere::BRDF, testing::_))
      .WillOnce(testing::Return(&reflector));

  iris::Bsdf bsdf(bxdf, iris::Vector(0.0, 0.0, 1.0),
                  iris::Vector(0.0, 0.0, 1.0), true);
  auto result = bsdf.Reflectance(iris::Vector(0.0, 0.0, 1.0),
                                 iris::Vector(0.0, 0.0, -1.0), allocator);
  EXPECT_TRUE(result);
  EXPECT_EQ(&reflector, &result->reflector);
  EXPECT_EQ(0.5, result->pdf);
}

TEST(BsdfTest, Normalize) {
  iris::internal::Arena arena;
  iris::SpectralAllocator allocator(arena);
  iris::reflectors::MockReflector reflector;

  iris::bxdfs::MockBxdf bxdf;
  EXPECT_CALL(bxdf, Sample(iris::Vector(0.0, 1.0, 0.0), testing::_,
                           iris::Vector(0.0, 0.0, 1.0), testing::_))
      .WillOnce(testing::Return(
          iris::Bxdf::SampleResult{iris::Vector(1.0, 0.0, 0.0)}));
  EXPECT_CALL(
      bxdf,
      Pdf(iris::Vector(0.0, 1.0, 0.0), iris::Vector(1.0, 0.0, 0.0),
          iris::Vector(0.0, 0.0, 1.0), nullptr, iris::Bxdf::Hemisphere::BTDF))
      .WillOnce(testing::Return(static_cast<iris::visual_t>(0.5)));
  EXPECT_CALL(bxdf, Reflectance(iris::Vector(0.0, 1.0, 0.0),
                                iris::Vector(1.0, 0.0, 0.0), nullptr,
                                iris::Bxdf::Hemisphere::BTDF, testing::_))
      .WillOnce(testing::Return(&reflector));

  iris::random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));

  iris::Bsdf bsdf(bxdf, iris::Vector(0.0, 0.0, 2.0),
                  iris::Vector(0.0, 0.0, 2.0), true);
  auto result = bsdf.Sample(iris::Vector(1.0, 0.0, 0.0), std::nullopt,
                            iris::Sampler(rng), allocator);
  EXPECT_TRUE(result);
  EXPECT_EQ(&reflector, &result->reflector);
  EXPECT_EQ(iris::Vector(0.0, 1.0, 0.0), result->direction);
  EXPECT_EQ(0.5, result->pdf.value());
}