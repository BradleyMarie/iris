#include "iris/bsdf.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/bxdfs/mock_bxdf.h"
#include "iris/internal/arena.h"
#include "iris/random/mock_random.h"
#include "iris/reflectors/mock_reflector.h"
#include "iris/testing/spectral_allocator.h"

namespace iris {
namespace {

using ::iris::bxdfs::MockBxdf;
using ::iris::internal::Arena;
using ::iris::random::MockRandom;
using ::iris::reflectors::MockReflector;
using ::iris::testing::GetSpectralAllocator;
using ::testing::_;
using ::testing::DoAll;
using ::testing::Eq;
using ::testing::IsNull;
using ::testing::Not;
using ::testing::NotNull;
using ::testing::Return;
using ::testing::SetArgPointee;

const Vector kTrueIncoming = Normalize(Vector(0.0, -1.0, -1.0));
const Vector kTrueBrdfOutgoing = Normalize(Vector(0.0, -1.0, 1.0));
const Vector kTrueBtdfOutgoing = Normalize(Vector(0.0, -1.0, -1.0));
const Vector kSurfaceNormal = Normalize(Vector(0.0, 0.0, 1.0));
const Vector kIncoming = Normalize(Vector(1.0, 0.0, 1.0));
const Vector kBrdfOutgoing = Normalize(Vector(-1.0, 0.0, 1.0));
const Vector kBtdfOutgoing = Normalize(Vector(-1.0, 0.0, -1.0));
const Vector kInvalid(1.0, 0.0, 0.0);

TEST(BsdfTest, IsDiffuse) {
  MockBxdf bxdf;
  EXPECT_CALL(bxdf, IsDiffuse(NotNull()))
      .WillOnce(DoAll(SetArgPointee<0>(1.0), Return(true)));

  Bsdf bsdf(bxdf, kSurfaceNormal, kSurfaceNormal);
  EXPECT_TRUE(bsdf.IsDiffuse());
}

TEST(BsdfTest, SampleIncomingZeroDP) {
  MockBxdf bxdf;
  EXPECT_CALL(bxdf, IsDiffuse(NotNull()))
      .WillOnce(DoAll(SetArgPointee<0>(1.0), Return(true)));

  MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));

  Bsdf bsdf(bxdf, kSurfaceNormal, kSurfaceNormal);
  std::optional<Bsdf::SampleResult> result =
      bsdf.Sample(kInvalid, std::nullopt, Sampler(rng), GetSpectralAllocator());
  EXPECT_FALSE(result);
}

TEST(BsdfTest, SampleFails) {
  MockBxdf bxdf;
  EXPECT_CALL(bxdf, IsDiffuse(NotNull()))
      .WillOnce(DoAll(SetArgPointee<0>(1.0), Return(true)));
  EXPECT_CALL(bxdf, Sample(kIncoming, Eq(std::nullopt), kSurfaceNormal, _, _))
      .WillOnce(Return(std::monostate()));

  MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));

  Bsdf bsdf(bxdf, kSurfaceNormal, kSurfaceNormal);
  std::optional<Bsdf::SampleResult> result = bsdf.Sample(
      kTrueIncoming, std::nullopt, Sampler(rng), GetSpectralAllocator());
  EXPECT_FALSE(result);
}

TEST(BsdfTest, SampleDiffuseFails) {
  MockBxdf bxdf;
  EXPECT_CALL(bxdf, IsDiffuse(NotNull()))
      .WillOnce(DoAll(SetArgPointee<0>(1.0), Return(true)));
  EXPECT_CALL(bxdf, SampleDiffuse(kIncoming, kSurfaceNormal, _))
      .WillOnce(Return(std::nullopt));

  MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));

  Bsdf bsdf(bxdf, kSurfaceNormal, kSurfaceNormal);
  std::optional<Bsdf::SampleResult> result =
      bsdf.Sample(kTrueIncoming, std::nullopt, Sampler(rng),
                  GetSpectralAllocator(), /*diffuse_only=*/true);
  EXPECT_FALSE(result);
}

TEST(BsdfTest, SampleOutgoingZeroDP) {
  MockBxdf bxdf;
  EXPECT_CALL(bxdf, IsDiffuse(NotNull()))
      .WillOnce(DoAll(SetArgPointee<0>(1.0), Return(true)));
  EXPECT_CALL(bxdf, Sample(kIncoming, Eq(std::nullopt), kSurfaceNormal, _, _))
      .WillOnce(Return(Bxdf::DiffuseSample{kInvalid}));

  MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));

  Bsdf bsdf(bxdf, kSurfaceNormal, kSurfaceNormal);
  std::optional<Bsdf::SampleResult> result = bsdf.Sample(
      kTrueIncoming, std::nullopt, Sampler(rng), GetSpectralAllocator());
  EXPECT_FALSE(result);
}

TEST(BsdfTest, SampleZeroPdf) {
  MockBxdf bxdf;
  EXPECT_CALL(bxdf, IsDiffuse(NotNull()))
      .WillOnce(DoAll(SetArgPointee<0>(1.0), Return(true)));
  EXPECT_CALL(bxdf, Sample(kIncoming, Eq(std::nullopt), kSurfaceNormal, _, _))
      .WillOnce(Return(Bxdf::DiffuseSample{kBtdfOutgoing}));
  EXPECT_CALL(bxdf, PdfDiffuse(kIncoming, kBtdfOutgoing, kSurfaceNormal,
                               Bxdf::Hemisphere::BTDF))
      .WillOnce(Return(static_cast<visual_t>(0.0)));

  MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));

  Bsdf bsdf(bxdf, kSurfaceNormal, kSurfaceNormal);
  std::optional<Bsdf::SampleResult> result = bsdf.Sample(
      kTrueIncoming, std::nullopt, Sampler(rng), GetSpectralAllocator());
  EXPECT_FALSE(result);
}

TEST(BsdfTest, SampleNegativePdf) {
  MockBxdf bxdf;
  EXPECT_CALL(bxdf, IsDiffuse(NotNull()))
      .WillOnce(DoAll(SetArgPointee<0>(1.0), Return(true)));
  EXPECT_CALL(bxdf, Sample(kIncoming, Eq(std::nullopt), kSurfaceNormal, _, _))
      .WillOnce(Return(Bxdf::DiffuseSample{kBtdfOutgoing}));
  EXPECT_CALL(bxdf, PdfDiffuse(kIncoming, kBtdfOutgoing, kSurfaceNormal,
                               Bxdf::Hemisphere::BTDF))
      .WillOnce(Return(static_cast<visual_t>(-1.0)));

  MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));

  Bsdf bsdf(bxdf, kSurfaceNormal, kSurfaceNormal);
  std::optional<Bsdf::SampleResult> result = bsdf.Sample(
      kTrueIncoming, std::nullopt, Sampler(rng), GetSpectralAllocator());
  EXPECT_FALSE(result);
}

TEST(BsdfTest, SampleNoReflector) {
  MockBxdf bxdf;
  EXPECT_CALL(bxdf, IsDiffuse(NotNull()))
      .WillOnce(DoAll(SetArgPointee<0>(1.0), Return(true)));
  EXPECT_CALL(bxdf, Sample(kIncoming, Eq(std::nullopt), kSurfaceNormal, _, _))
      .WillOnce(Return(Bxdf::DiffuseSample{kBtdfOutgoing}));
  EXPECT_CALL(bxdf, PdfDiffuse(kIncoming, kBtdfOutgoing, kSurfaceNormal,
                               Bxdf::Hemisphere::BTDF))
      .WillOnce(Return(static_cast<visual_t>(0.5)));
  EXPECT_CALL(bxdf, ReflectanceDiffuse(kIncoming, kBtdfOutgoing,
                                       Bxdf::Hemisphere::BTDF, _))
      .WillOnce(Return(nullptr));

  MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));

  Bsdf bsdf(bxdf, kSurfaceNormal, kSurfaceNormal);
  std::optional<Bsdf::SampleResult> result = bsdf.Sample(
      kTrueIncoming, std::nullopt, Sampler(rng), GetSpectralAllocator());
  EXPECT_FALSE(result);
}

TEST(BsdfTest, SampleAdjustByDiffusePdf) {
  MockReflector reflector;

  MockBxdf bxdf;
  EXPECT_CALL(bxdf, IsDiffuse(NotNull()))
      .WillOnce(DoAll(SetArgPointee<0>(0.5), Return(true)));
  EXPECT_CALL(bxdf, Sample(kIncoming, Eq(std::nullopt), kSurfaceNormal, _, _))
      .WillOnce(Return(Bxdf::DiffuseSample{kBtdfOutgoing}));
  EXPECT_CALL(bxdf, PdfDiffuse(kIncoming, kBtdfOutgoing, kSurfaceNormal,
                               Bxdf::Hemisphere::BTDF))
      .WillOnce(Return(static_cast<visual_t>(0.5)));
  EXPECT_CALL(bxdf, ReflectanceDiffuse(kIncoming, kBtdfOutgoing,
                                       Bxdf::Hemisphere::BTDF, _))
      .WillOnce(Return(&reflector));

  MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));

  Bsdf bsdf(bxdf, kSurfaceNormal, kSurfaceNormal, true);
  std::optional<iris::Bsdf::SampleResult> result = bsdf.Sample(
      kTrueIncoming, std::nullopt, Sampler(rng), GetSpectralAllocator());
  EXPECT_TRUE(result);
  EXPECT_EQ(&reflector, &result->reflector);
  EXPECT_EQ(kTrueBtdfOutgoing, result->direction);
  EXPECT_FALSE(result->differentials);
  EXPECT_EQ(0.25, result->pdf);
  EXPECT_TRUE(result->diffuse);
}

TEST(BsdfTest, SampleBtdf) {
  MockReflector reflector;

  MockBxdf bxdf;
  EXPECT_CALL(bxdf, IsDiffuse(NotNull()))
      .WillOnce(DoAll(SetArgPointee<0>(1.0), Return(true)));
  EXPECT_CALL(bxdf, Sample(kIncoming, Eq(std::nullopt), kSurfaceNormal, _, _))
      .WillOnce(Return(Bxdf::DiffuseSample{kBtdfOutgoing}));
  EXPECT_CALL(bxdf, PdfDiffuse(kIncoming, kBtdfOutgoing, kSurfaceNormal,
                               Bxdf::Hemisphere::BTDF))
      .WillOnce(Return(static_cast<visual_t>(0.5)));
  EXPECT_CALL(bxdf, ReflectanceDiffuse(kIncoming, kBtdfOutgoing,
                                       Bxdf::Hemisphere::BTDF, _))
      .WillOnce(Return(&reflector));

  MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));

  Bsdf bsdf(bxdf, kSurfaceNormal, kSurfaceNormal, true);
  std::optional<Bsdf::SampleResult> result = bsdf.Sample(
      kTrueIncoming, std::nullopt, Sampler(rng), GetSpectralAllocator());
  EXPECT_TRUE(result);
  EXPECT_EQ(&reflector, &result->reflector);
  EXPECT_EQ(kTrueBtdfOutgoing, result->direction);
  EXPECT_FALSE(result->differentials);
  EXPECT_EQ(0.5, result->pdf);
  EXPECT_TRUE(result->diffuse);
}

TEST(BsdfTest, SampleBrdf) {
  MockReflector reflector;

  MockBxdf bxdf;
  EXPECT_CALL(bxdf, IsDiffuse(NotNull()))
      .WillOnce(DoAll(SetArgPointee<0>(1.0), Return(true)));
  EXPECT_CALL(bxdf, Sample(kIncoming, Eq(std::nullopt), kSurfaceNormal, _, _))
      .WillOnce(Return(Bxdf::DiffuseSample{kBrdfOutgoing}));
  EXPECT_CALL(bxdf, PdfDiffuse(kIncoming, kBrdfOutgoing, kSurfaceNormal,
                               Bxdf::Hemisphere::BRDF))
      .WillOnce(Return(static_cast<visual_t>(0.5)));
  EXPECT_CALL(bxdf, ReflectanceDiffuse(kIncoming, kBrdfOutgoing,
                                       Bxdf::Hemisphere::BRDF, _))
      .WillOnce(Return(&reflector));

  MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));

  Bsdf bsdf(bxdf, kSurfaceNormal, kSurfaceNormal, true);
  std::optional<Bsdf::SampleResult> result = bsdf.Sample(
      kTrueIncoming, std::nullopt, Sampler(rng), GetSpectralAllocator());
  EXPECT_TRUE(result);
  EXPECT_EQ(&reflector, &result->reflector);
  EXPECT_EQ(kTrueBrdfOutgoing, result->direction);
  EXPECT_EQ(0.5, result->pdf);
  EXPECT_TRUE(result->diffuse);
}

TEST(BsdfTest, SampleDiffuse) {
  MockReflector reflector;

  MockBxdf bxdf;
  EXPECT_CALL(bxdf, IsDiffuse(NotNull()))
      .WillOnce(DoAll(SetArgPointee<0>(1.0), Return(true)));
  EXPECT_CALL(bxdf, SampleDiffuse(kIncoming, kSurfaceNormal, _))
      .WillOnce(Return(kBrdfOutgoing));
  EXPECT_CALL(bxdf, PdfDiffuse(kIncoming, kBrdfOutgoing, kSurfaceNormal,
                               Bxdf::Hemisphere::BRDF))
      .WillOnce(Return(static_cast<visual_t>(0.5)));
  EXPECT_CALL(bxdf, ReflectanceDiffuse(kIncoming, kBrdfOutgoing,
                                       Bxdf::Hemisphere::BRDF, _))
      .WillOnce(Return(&reflector));

  MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));

  Bsdf bsdf(bxdf, kSurfaceNormal, kSurfaceNormal, true);
  std::optional<Bsdf::SampleResult> result =
      bsdf.Sample(kTrueIncoming, std::nullopt, Sampler(rng),
                  GetSpectralAllocator(), /*diffuse_only=*/true);
  EXPECT_TRUE(result);
  EXPECT_EQ(&reflector, &result->reflector);
  EXPECT_EQ(kTrueBrdfOutgoing, result->direction);
  EXPECT_EQ(0.5, result->pdf);
  EXPECT_TRUE(result->diffuse);
}

TEST(BsdfTest, SamplePartiallyDiffuse) {
  MockReflector reflector;

  MockBxdf bxdf;
  EXPECT_CALL(bxdf, IsDiffuse(NotNull()))
      .WillOnce(DoAll(SetArgPointee<0>(0.5), Return(true)));
  EXPECT_CALL(bxdf, SampleDiffuse(kIncoming, kSurfaceNormal, _))
      .WillOnce(Return(kBrdfOutgoing));
  EXPECT_CALL(bxdf, PdfDiffuse(kIncoming, kBrdfOutgoing, kSurfaceNormal,
                               Bxdf::Hemisphere::BRDF))
      .WillOnce(Return(static_cast<visual_t>(0.5)));
  EXPECT_CALL(bxdf, ReflectanceDiffuse(kIncoming, kBrdfOutgoing,
                                       Bxdf::Hemisphere::BRDF, _))
      .WillOnce(Return(&reflector));

  MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));

  Bsdf bsdf(bxdf, kSurfaceNormal, kSurfaceNormal, true);
  std::optional<Bsdf::SampleResult> result =
      bsdf.Sample(kTrueIncoming, std::nullopt, Sampler(rng),
                  GetSpectralAllocator(), /*diffuse_only=*/true);
  EXPECT_TRUE(result);
  EXPECT_EQ(&reflector, &result->reflector);
  EXPECT_EQ(kTrueBrdfOutgoing, result->direction);
  EXPECT_EQ(0.5, result->pdf);
  EXPECT_TRUE(result->diffuse);
}

TEST(BsdfTest, SampleWithInputDerivatives) {
  MockReflector reflector;

  MockBxdf bxdf;
  EXPECT_CALL(bxdf, IsDiffuse(NotNull()))
      .WillOnce(DoAll(SetArgPointee<0>(1.0), Return(true)));
  EXPECT_CALL(bxdf,
              Sample(kIncoming, Not(Eq(std::nullopt)), kSurfaceNormal, _, _))
      .WillOnce(Return(Bxdf::DiffuseSample{kBtdfOutgoing}));
  EXPECT_CALL(bxdf, PdfDiffuse(kIncoming, kBtdfOutgoing, kSurfaceNormal,
                               Bxdf::Hemisphere::BTDF))
      .WillOnce(Return(1.0));
  EXPECT_CALL(bxdf, ReflectanceDiffuse(kIncoming, kBtdfOutgoing,
                                       Bxdf::Hemisphere::BTDF, _))
      .WillOnce(Return(&reflector));

  MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));

  Bsdf bsdf(bxdf, kSurfaceNormal, kSurfaceNormal, true);
  std::optional<Bsdf::SampleResult> result =
      bsdf.Sample(kTrueIncoming, {{kIncoming, kIncoming}}, Sampler(rng),
                  GetSpectralAllocator());
  EXPECT_TRUE(result);
  EXPECT_EQ(&reflector, &result->reflector);
  EXPECT_EQ(kTrueBtdfOutgoing, result->direction);
  EXPECT_FALSE(result->differentials);
  EXPECT_EQ(1.0, result->pdf);
  EXPECT_TRUE(result->diffuse);
}

TEST(BsdfTest, SampleSpecularInvalidPdf) {
  MockReflector reflector;

  MockBxdf bxdf;
  EXPECT_CALL(bxdf, IsDiffuse(NotNull()))
      .WillOnce(DoAll(SetArgPointee<0>(0.0), Return(false)));
  EXPECT_CALL(bxdf,
              Sample(kIncoming, Not(Eq(std::nullopt)), kSurfaceNormal, _, _))
      .WillOnce(Return(Bxdf::SpecularSample{Bxdf::Hemisphere::BTDF,
                                            kBtdfOutgoing,
                                            &reflector,
                                            {{kBtdfOutgoing, kBtdfOutgoing}},
                                            0.0}));

  MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));

  Bsdf bsdf(bxdf, kSurfaceNormal, kSurfaceNormal, true);
  std::optional<Bsdf::SampleResult> result =
      bsdf.Sample(kTrueIncoming, {{kIncoming, kIncoming}}, Sampler(rng),
                  GetSpectralAllocator());
  EXPECT_FALSE(result);
}

TEST(BsdfTest, SampleSpecularInvalidReflector) {
  MockReflector reflector;

  MockBxdf bxdf;
  EXPECT_CALL(bxdf, IsDiffuse(NotNull()))
      .WillOnce(DoAll(SetArgPointee<0>(0.0), Return(false)));
  EXPECT_CALL(bxdf,
              Sample(kIncoming, Not(Eq(std::nullopt)), kSurfaceNormal, _, _))
      .WillOnce(Return(Bxdf::SpecularSample{Bxdf::Hemisphere::BTDF,
                                            kBtdfOutgoing,
                                            nullptr,
                                            {{kBtdfOutgoing, kBtdfOutgoing}},
                                            1.0}));

  MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));

  Bsdf bsdf(bxdf, kSurfaceNormal, kSurfaceNormal, true);
  std::optional<Bsdf::SampleResult> result =
      bsdf.Sample(kTrueIncoming, {{kIncoming, kIncoming}}, Sampler(rng),
                  GetSpectralAllocator());
  EXPECT_FALSE(result);
}

TEST(BsdfTest, SampleSpecularWrongHemisphere) {
  MockReflector reflector;

  MockBxdf bxdf;
  EXPECT_CALL(bxdf, IsDiffuse(NotNull()))
      .WillOnce(DoAll(SetArgPointee<0>(0.0), Return(false)));
  EXPECT_CALL(bxdf,
              Sample(kIncoming, Not(Eq(std::nullopt)), kSurfaceNormal, _, _))
      .WillOnce(Return(Bxdf::SpecularSample{Bxdf::Hemisphere::BRDF,
                                            kBtdfOutgoing,
                                            &reflector,
                                            {{kBtdfOutgoing, kBtdfOutgoing}},
                                            1.0}));

  MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));

  Bsdf bsdf(bxdf, kSurfaceNormal, kSurfaceNormal, true);
  std::optional<Bsdf::SampleResult> result =
      bsdf.Sample(kTrueIncoming, {{kIncoming, kIncoming}}, Sampler(rng),
                  GetSpectralAllocator());
  EXPECT_FALSE(result);
}

TEST(BsdfTest, SampleSpecularWithNoDifferentials) {
  MockReflector reflector;

  MockBxdf bxdf;
  EXPECT_CALL(bxdf, IsDiffuse(NotNull()))
      .WillOnce(DoAll(SetArgPointee<0>(0.0), Return(false)));
  EXPECT_CALL(bxdf, Sample(kIncoming, Eq(std::nullopt), kSurfaceNormal, _, _))
      .WillOnce(
          Return(Bxdf::SpecularSample{Bxdf::Hemisphere::BTDF, kBtdfOutgoing,
                                      &reflector, std::nullopt, 1.0}));

  MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));

  Bsdf bsdf(bxdf, kSurfaceNormal, kSurfaceNormal, true);
  std::optional<Bsdf::SampleResult> result = bsdf.Sample(
      kTrueIncoming, std::nullopt, Sampler(rng), GetSpectralAllocator());
  EXPECT_TRUE(result);
  EXPECT_EQ(&reflector, &result->reflector);
  EXPECT_EQ(kTrueBtdfOutgoing, result->direction);
  EXPECT_FALSE(result->differentials);
  EXPECT_EQ(1.0, result->pdf);
  EXPECT_FALSE(result->diffuse);
}

TEST(BsdfTest, SampleSpecularWithOnlyIncomingDifferentials) {
  MockReflector reflector;

  MockBxdf bxdf;
  EXPECT_CALL(bxdf, IsDiffuse(NotNull()))
      .WillOnce(DoAll(SetArgPointee<0>(0.0), Return(false)));
  EXPECT_CALL(bxdf,
              Sample(kIncoming, Not(Eq(std::nullopt)), kSurfaceNormal, _, _))
      .WillOnce(
          Return(Bxdf::SpecularSample{Bxdf::Hemisphere::BTDF, kBtdfOutgoing,
                                      &reflector, std::nullopt, 1.0}));

  MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));

  Bsdf bsdf(bxdf, kSurfaceNormal, kSurfaceNormal, true);
  std::optional<Bsdf::SampleResult> result =
      bsdf.Sample(kTrueIncoming, {{kIncoming, kIncoming}}, Sampler(rng),
                  GetSpectralAllocator());
  EXPECT_TRUE(result);
  EXPECT_EQ(&reflector, &result->reflector);
  EXPECT_EQ(kTrueBtdfOutgoing, result->direction);
  EXPECT_FALSE(result->differentials);
  EXPECT_EQ(1.0, result->pdf);
  EXPECT_FALSE(result->diffuse);
}

TEST(BsdfTest, SampleSpecularWithDifferentials) {
  MockReflector reflector;

  MockBxdf bxdf;
  EXPECT_CALL(bxdf, IsDiffuse(NotNull()))
      .WillOnce(DoAll(SetArgPointee<0>(0.0), Return(false)));
  EXPECT_CALL(bxdf,
              Sample(kIncoming, Not(Eq(std::nullopt)), kSurfaceNormal, _, _))
      .WillOnce(Return(Bxdf::SpecularSample{Bxdf::Hemisphere::BTDF,
                                            kBtdfOutgoing,
                                            &reflector,
                                            {{kBtdfOutgoing, kBtdfOutgoing}},
                                            1.0}));

  MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));

  Bsdf bsdf(bxdf, kSurfaceNormal, kSurfaceNormal, true);
  std::optional<Bsdf::SampleResult> result =
      bsdf.Sample(kTrueIncoming, {{kIncoming, kIncoming}}, Sampler(rng),
                  GetSpectralAllocator());
  EXPECT_TRUE(result);
  EXPECT_EQ(&reflector, &result->reflector);
  EXPECT_EQ(kTrueBtdfOutgoing, result->direction);
  ASSERT_TRUE(result->differentials);
  EXPECT_EQ(kTrueBtdfOutgoing, result->differentials->dx);
  EXPECT_EQ(kTrueBtdfOutgoing, result->differentials->dy);
  EXPECT_EQ(1.0, result->pdf);
  EXPECT_FALSE(result->diffuse);
}

TEST(BsdfTest, ReflectanceNotDiffuse) {
  MockBxdf bxdf;
  EXPECT_CALL(bxdf, IsDiffuse(NotNull()))
      .WillOnce(DoAll(SetArgPointee<0>(0.0), Return(false)));

  Bsdf bsdf(bxdf, kSurfaceNormal, kSurfaceNormal, true);
  std::optional<Bsdf::ReflectanceResult> result = bsdf.Reflectance(
      kTrueIncoming, kTrueBtdfOutgoing, GetSpectralAllocator());
  EXPECT_FALSE(result);
}

TEST(BsdfTest, ReflectanceIncomingZeroDp) {
  MockBxdf bxdf;
  EXPECT_CALL(bxdf, IsDiffuse(NotNull()))
      .WillOnce(DoAll(SetArgPointee<0>(1.0), Return(true)));

  Bsdf bsdf(bxdf, kSurfaceNormal, kSurfaceNormal, true);
  std::optional<Bsdf::ReflectanceResult> result =
      bsdf.Reflectance(kInvalid, kTrueBtdfOutgoing, GetSpectralAllocator());
  EXPECT_FALSE(result);
}

TEST(BsdfTest, ReflectanceOutgoingZeroDp) {
  MockBxdf bxdf;
  EXPECT_CALL(bxdf, IsDiffuse(NotNull()))
      .WillOnce(DoAll(SetArgPointee<0>(1.0), Return(true)));

  Bsdf bsdf(bxdf, kSurfaceNormal, kSurfaceNormal, true);
  std::optional<Bsdf::ReflectanceResult> result =
      bsdf.Reflectance(kTrueIncoming, kInvalid, GetSpectralAllocator());
  EXPECT_FALSE(result);
}

TEST(BsdfTest, ReflectanceZeroPdf) {
  MockBxdf bxdf;
  EXPECT_CALL(bxdf, IsDiffuse(NotNull()))
      .WillOnce(DoAll(SetArgPointee<0>(1.0), Return(true)));
  EXPECT_CALL(bxdf, PdfDiffuse(kIncoming, kBtdfOutgoing, kSurfaceNormal,
                               Bxdf::Hemisphere::BTDF))
      .WillOnce(Return(static_cast<visual_t>(0.0)));

  Bsdf bsdf(bxdf, kSurfaceNormal, kSurfaceNormal, true);
  std::optional<Bsdf::ReflectanceResult> result = bsdf.Reflectance(
      kTrueIncoming, kTrueBtdfOutgoing, GetSpectralAllocator());
  EXPECT_FALSE(result);
}

TEST(BsdfTest, ReflectanceNegativePdf) {
  MockBxdf bxdf;
  EXPECT_CALL(bxdf, IsDiffuse(NotNull()))
      .WillOnce(DoAll(SetArgPointee<0>(1.0), Return(true)));
  EXPECT_CALL(bxdf, PdfDiffuse(kIncoming, kBtdfOutgoing, kSurfaceNormal,
                               Bxdf::Hemisphere::BTDF))
      .WillOnce(Return(static_cast<visual_t>(-1.0)));

  Bsdf bsdf(bxdf, kSurfaceNormal, kSurfaceNormal, true);
  std::optional<Bsdf::ReflectanceResult> result = bsdf.Reflectance(
      kTrueIncoming, kTrueBtdfOutgoing, GetSpectralAllocator());
  EXPECT_FALSE(result);
}

TEST(BsdfTest, ReflectanceNoPdf) {
  MockBxdf bxdf;
  EXPECT_CALL(bxdf, IsDiffuse(NotNull()))
      .WillOnce(DoAll(SetArgPointee<0>(1.0), Return(true)));
  EXPECT_CALL(bxdf, PdfDiffuse(kIncoming, kBtdfOutgoing, kSurfaceNormal,
                               Bxdf::Hemisphere::BTDF))
      .WillOnce(Return(0.0));

  Bsdf bsdf(bxdf, kSurfaceNormal, kSurfaceNormal, true);
  std::optional<Bsdf::ReflectanceResult> result = bsdf.Reflectance(
      kTrueIncoming, kTrueBtdfOutgoing, GetSpectralAllocator());
  EXPECT_FALSE(result);
}

TEST(BsdfTest, ReflectanceNoReflector) {
  MockBxdf bxdf;
  EXPECT_CALL(bxdf, IsDiffuse(NotNull()))
      .WillOnce(DoAll(SetArgPointee<0>(1.0), Return(true)));
  EXPECT_CALL(bxdf, PdfDiffuse(kIncoming, kBtdfOutgoing, kSurfaceNormal,
                               Bxdf::Hemisphere::BTDF))
      .WillOnce(Return(static_cast<visual>(0.5)));
  EXPECT_CALL(bxdf, ReflectanceDiffuse(kIncoming, kBtdfOutgoing,
                                       Bxdf::Hemisphere::BTDF, _))
      .WillOnce(Return(nullptr));

  Bsdf bsdf(bxdf, kSurfaceNormal, kSurfaceNormal, true);
  std::optional<Bsdf::ReflectanceResult> result = bsdf.Reflectance(
      kTrueIncoming, kTrueBtdfOutgoing, GetSpectralAllocator());
  EXPECT_FALSE(result);
}

TEST(BsdfTest, ReflectanceBtdf) {
  MockReflector reflector;

  MockBxdf bxdf;
  EXPECT_CALL(bxdf, IsDiffuse(NotNull()))
      .WillOnce(DoAll(SetArgPointee<0>(1.0), Return(true)));
  EXPECT_CALL(bxdf, PdfDiffuse(kIncoming, kBtdfOutgoing, kSurfaceNormal,
                               Bxdf::Hemisphere::BTDF))
      .WillOnce(Return(static_cast<visual_t>(0.5)));
  EXPECT_CALL(bxdf, ReflectanceDiffuse(kIncoming, kBtdfOutgoing,
                                       Bxdf::Hemisphere::BTDF, _))
      .WillOnce(Return(&reflector));

  Bsdf bsdf(bxdf, kSurfaceNormal, kSurfaceNormal, true);
  std::optional<Bsdf::ReflectanceResult> result = bsdf.Reflectance(
      kTrueIncoming, kTrueBtdfOutgoing, GetSpectralAllocator());
  EXPECT_TRUE(result);
  EXPECT_EQ(&reflector, &result->reflector);
  EXPECT_EQ(0.5, result->pdf);
}

TEST(BsdfTest, ReflectanceBrdf) {
  MockReflector reflector;

  MockBxdf bxdf;
  EXPECT_CALL(bxdf, IsDiffuse(NotNull()))
      .WillOnce(DoAll(SetArgPointee<0>(1.0), Return(true)));
  EXPECT_CALL(bxdf, PdfDiffuse(kIncoming, kBrdfOutgoing, kSurfaceNormal,
                               Bxdf::Hemisphere::BRDF))
      .WillOnce(Return(static_cast<visual_t>(0.5)));
  EXPECT_CALL(bxdf, ReflectanceDiffuse(kIncoming, kBrdfOutgoing,
                                       Bxdf::Hemisphere::BRDF, _))
      .WillOnce(Return(&reflector));

  Bsdf bsdf(bxdf, kSurfaceNormal, kSurfaceNormal, true);
  std::optional<Bsdf::ReflectanceResult> result = bsdf.Reflectance(
      kTrueIncoming, kTrueBrdfOutgoing, GetSpectralAllocator());
  EXPECT_TRUE(result);
  EXPECT_EQ(&reflector, &result->reflector);
  EXPECT_EQ(0.5, result->pdf);
}

TEST(BsdfTest, Normalize) {
  MockReflector reflector;

  MockBxdf bxdf;
  EXPECT_CALL(bxdf, Sample(kIncoming, _, kSurfaceNormal, _, _))
      .WillOnce(Return(Bxdf::DiffuseSample{kBtdfOutgoing}));
  EXPECT_CALL(bxdf, PdfDiffuse(kIncoming, kBtdfOutgoing, kSurfaceNormal,
                               Bxdf::Hemisphere::BTDF))
      .WillOnce(Return(static_cast<visual_t>(0.5)));
  EXPECT_CALL(bxdf, ReflectanceDiffuse(kIncoming, kBtdfOutgoing,
                                       Bxdf::Hemisphere::BTDF, _))
      .WillOnce(Return(&reflector));
  EXPECT_CALL(bxdf, IsDiffuse(NotNull()))
      .WillOnce(DoAll(SetArgPointee<0>(1.0), Return(true)));

  MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));

  Bsdf bsdf(bxdf, Vector(0.0, 0.0, 2.0), Vector(0.0, 0.0, 2.0), true);
  std::optional<Bsdf::SampleResult> result = bsdf.Sample(
      kTrueIncoming, std::nullopt, Sampler(rng), GetSpectralAllocator());
  EXPECT_TRUE(result);
  EXPECT_EQ(&reflector, &result->reflector);
  EXPECT_EQ(kTrueBtdfOutgoing, result->direction);
  EXPECT_EQ(0.5, result->pdf);
  EXPECT_TRUE(result->diffuse);
}

}  // namespace
}  // namespace iris
