#include "iris/bxdfs/composite_bxdf.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/bxdfs/mock_bxdf.h"
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
using ::testing::DoAll;
using ::testing::Eq;
using ::testing::FieldsAre;
using ::testing::IsNull;
using ::testing::NotNull;
using ::testing::Ref;
using ::testing::Return;
using ::testing::SetArgPointee;

TEST(CompositeBxdfTest, IsNotDiffuse) {
  MockBxdf bxdf0;
  EXPECT_CALL(bxdf0, IsDiffuse(NotNull()))
      .WillRepeatedly(DoAll(SetArgPointee<0>(0.0), Return(false)));
  EXPECT_CALL(bxdf0, IsDiffuse(IsNull())).WillRepeatedly(Return(false));

  MockBxdf bxdf1;
  EXPECT_CALL(bxdf1, IsDiffuse(NotNull()))
      .WillRepeatedly(DoAll(SetArgPointee<0>(0.0), Return(false)));
  EXPECT_CALL(bxdf1, IsDiffuse(IsNull())).WillRepeatedly(Return(false));

  const Bxdf* composite = MakeCompositeBxdf(GetBxdfAllocator(), &bxdf0, &bxdf1);
  EXPECT_FALSE(composite->IsDiffuse(nullptr));

  visual_t diffuse_pdf;
  EXPECT_FALSE(composite->IsDiffuse(&diffuse_pdf));
  EXPECT_EQ(0.0, diffuse_pdf);
}

TEST(CompositeBxdfTest, IsDiffuse) {
  MockBxdf bxdf0;
  EXPECT_CALL(bxdf0, IsDiffuse(NotNull()))
      .WillRepeatedly(DoAll(SetArgPointee<0>(0.0), Return(false)));
  EXPECT_CALL(bxdf0, IsDiffuse(IsNull())).WillRepeatedly(Return(false));

  MockBxdf bxdf1;
  EXPECT_CALL(bxdf1, IsDiffuse(NotNull()))
      .WillRepeatedly(DoAll(SetArgPointee<0>(1.0), Return(true)));
  EXPECT_CALL(bxdf1, IsDiffuse(IsNull())).WillRepeatedly(Return(true));

  const Bxdf* composite = MakeCompositeBxdf(GetBxdfAllocator(), &bxdf0, &bxdf1);
  EXPECT_TRUE(composite->IsDiffuse(nullptr));

  visual_t diffuse_pdf;
  EXPECT_TRUE(composite->IsDiffuse(&diffuse_pdf));
  EXPECT_EQ(0.5, diffuse_pdf);
}

TEST(CompositeBxdfTest, SampleDiffuseNone) {
  MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));
  Sampler sampler(rng);

  MockBxdf bxdf0;
  EXPECT_CALL(bxdf0, IsDiffuse(NotNull()))
      .WillRepeatedly(DoAll(SetArgPointee<0>(0.0), Return(false)));
  EXPECT_CALL(bxdf0, IsDiffuse(IsNull())).WillRepeatedly(Return(false));

  MockBxdf bxdf1;
  EXPECT_CALL(bxdf1, IsDiffuse(NotNull()))
      .WillRepeatedly(DoAll(SetArgPointee<0>(0.0), Return(false)));
  EXPECT_CALL(bxdf1, IsDiffuse(IsNull())).WillRepeatedly(Return(false));

  const Bxdf* composite = MakeCompositeBxdf(GetBxdfAllocator(), &bxdf0, &bxdf1);
  std::optional<Vector> sample = composite->SampleDiffuse(
      Vector(1.0, 0.0, 0.0), Vector(0.0, 0.0, 1.0), sampler);
  EXPECT_FALSE(sample);
}

TEST(CompositeBxdfTest, SampleDiffuse) {
  MockRandom rng;
  EXPECT_CALL(rng, NextGeometric()).WillOnce(Return(0.55));
  EXPECT_CALL(rng, DiscardGeometric(1));
  Sampler sampler(rng);

  MockBxdf bxdf0;
  EXPECT_CALL(bxdf0, IsDiffuse(NotNull()))
      .WillRepeatedly(DoAll(SetArgPointee<0>(1.0), Return(true)));
  EXPECT_CALL(bxdf0, IsDiffuse(IsNull())).WillRepeatedly(Return(true));

  MockBxdf bxdf1;
  EXPECT_CALL(bxdf1, IsDiffuse(NotNull()))
      .WillRepeatedly(DoAll(SetArgPointee<0>(0.0), Return(false)));
  EXPECT_CALL(bxdf1, IsDiffuse(IsNull())).WillRepeatedly(Return(false));

  MockBxdf bxdf2;
  EXPECT_CALL(bxdf2, IsDiffuse(NotNull()))
      .WillRepeatedly(DoAll(SetArgPointee<0>(1.0), Return(true)));
  EXPECT_CALL(bxdf2, IsDiffuse(IsNull())).WillRepeatedly(Return(true));

  EXPECT_CALL(bxdf2,
              SampleDiffuse(Vector(1.0, 0.0, 0.0), Vector(0.0, 0.0, 1.0), _))
      .WillOnce(Return(Vector(1.0, 0.0, 0.0)));

  const Bxdf* composite =
      MakeCompositeBxdf(GetBxdfAllocator(), &bxdf0, &bxdf1, &bxdf2);
  std::optional<Vector> sample = composite->SampleDiffuse(
      Vector(1.0, 0.0, 0.0), Vector(0.0, 0.0, 1.0), sampler);
  ASSERT_TRUE(sample);
  EXPECT_EQ(Vector(1.0, 0.0, 0.0), *sample);
}

TEST(CompositeBxdfTest, SampleAllDifuse) {
  MockRandom rng;
  EXPECT_CALL(rng, NextGeometric()).WillOnce(Return(0.75));
  EXPECT_CALL(rng, DiscardGeometric(1));
  Sampler sampler(rng);

  MockBxdf bxdf0;
  EXPECT_CALL(bxdf0, IsDiffuse(NotNull()))
      .WillRepeatedly(DoAll(SetArgPointee<0>(1.0), Return(true)));
  EXPECT_CALL(bxdf0, IsDiffuse(IsNull())).WillRepeatedly(Return(true));

  MockBxdf bxdf1;
  EXPECT_CALL(bxdf1, IsDiffuse(NotNull()))
      .WillRepeatedly(DoAll(SetArgPointee<0>(1.0), Return(true)));
  EXPECT_CALL(bxdf1, IsDiffuse(IsNull())).WillRepeatedly(Return(true));

  EXPECT_CALL(bxdf1,
              Sample(Vector(1.0, 0.0, 0.0), _, Vector(0.0, 0.0, 1.0), _, _))
      .WillOnce(Return(Bxdf::DiffuseSample{Vector(1.0, 0.0, 0.0)}));

  const Bxdf* composite = MakeCompositeBxdf(GetBxdfAllocator(), &bxdf0, &bxdf1);
  auto result =
      composite->Sample(Vector(1.0, 0.0, 0.0), std::nullopt,
                        Vector(0.0, 0.0, 1.0), sampler, GetSpectralAllocator());
  EXPECT_THAT(std::get<Bxdf::DiffuseSample>(result),
              FieldsAre(Vector(1.0, 0.0, 0.0)));
}

TEST(CompositeBxdfTest, SampleAllSpecular) {
  MockRandom rng;
  EXPECT_CALL(rng, NextGeometric()).WillOnce(Return(0.75));
  EXPECT_CALL(rng, DiscardGeometric(1));
  Sampler sampler(rng);

  MockReflector reflector;

  MockBxdf bxdf0;
  EXPECT_CALL(bxdf0, IsDiffuse(NotNull()))
      .WillRepeatedly(DoAll(SetArgPointee<0>(0.0), Return(false)));
  EXPECT_CALL(bxdf0, IsDiffuse(IsNull())).WillRepeatedly(Return(false));

  MockBxdf bxdf1;
  EXPECT_CALL(bxdf1, IsDiffuse(NotNull()))
      .WillRepeatedly(DoAll(SetArgPointee<0>(0.0), Return(false)));
  EXPECT_CALL(bxdf1, IsDiffuse(IsNull())).WillRepeatedly(Return(false));

  EXPECT_CALL(bxdf1,
              Sample(Vector(1.0, 0.0, 0.0), _, Vector(0.0, 0.0, 1.0), _, _))
      .WillOnce(Return(Bxdf::SpecularSample{Bxdf::Hemisphere::BRDF,
                                            Vector(1.0, 0.0, 0.0), &reflector,
                                            std::nullopt, 1.0}));

  const Bxdf* composite = MakeCompositeBxdf(GetBxdfAllocator(), &bxdf0, &bxdf1);
  auto result =
      composite->Sample(Vector(1.0, 0.0, 0.0), std::nullopt,
                        Vector(0.0, 0.0, 1.0), sampler, GetSpectralAllocator());
  EXPECT_THAT(std::get<Bxdf::SpecularSample>(result),
              FieldsAre(Bxdf::Hemisphere::BRDF, Vector(1.0, 0.0, 0.0),
                        Eq(&reflector), Eq(std::nullopt), Eq(0.5)));
}

TEST(CompositeBxdfTest, PdfAllSpecular) {
  MockBxdf bxdf;
  EXPECT_CALL(bxdf, IsDiffuse(NotNull()))
      .WillRepeatedly(DoAll(SetArgPointee<0>(0.0), Return(false)));
  EXPECT_CALL(bxdf, IsDiffuse(IsNull())).WillRepeatedly(Return(false));

  const Bxdf* composite = MakeCompositeBxdf(GetBxdfAllocator(), &bxdf, &bxdf);
  EXPECT_EQ(0.0, composite->PdfDiffuse(
                     Vector(1.0, 0.0, 0.0), Vector(-1.0, 0.0, 0.0),
                     Vector(0.0, 0.0, 1.0), Bxdf::Hemisphere::BRDF));
}

TEST(CompositeBxdfTest, PdfOneBxdf) {
  MockBxdf bxdf;
  EXPECT_CALL(bxdf, IsDiffuse(NotNull()))
      .WillRepeatedly(DoAll(SetArgPointee<0>(1.0), Return(true)));
  EXPECT_CALL(bxdf, IsDiffuse(IsNull())).WillRepeatedly(Return(true));

  EXPECT_CALL(bxdf, PdfDiffuse(Vector(1.0, 0.0, 0.0), Vector(-1.0, 0.0, 0.0),
                               Vector(0.0, 0.0, 1.0), Bxdf::Hemisphere::BRDF))
      .WillOnce(Return(1.0));

  const Bxdf* composite = MakeCompositeBxdf(GetBxdfAllocator(), &bxdf);
  EXPECT_EQ(1.0, composite->PdfDiffuse(
                     Vector(1.0, 0.0, 0.0), Vector(-1.0, 0.0, 0.0),
                     Vector(0.0, 0.0, 1.0), Bxdf::Hemisphere::BRDF));
}

TEST(CompositeBxdfTest, PdfTwoBxdfs) {
  MockBxdf bxdf0;
  EXPECT_CALL(bxdf0, IsDiffuse(NotNull()))
      .WillRepeatedly(DoAll(SetArgPointee<0>(1.0), Return(true)));
  EXPECT_CALL(bxdf0, IsDiffuse(IsNull())).WillRepeatedly(Return(true));

  EXPECT_CALL(bxdf0, PdfDiffuse(Vector(1.0, 0.0, 0.0), Vector(-1.0, 0.0, 0.0),
                                Vector(0.0, 0.0, 1.0), Bxdf::Hemisphere::BRDF))
      .WillOnce(Return(1.0));

  MockBxdf bxdf1;
  EXPECT_CALL(bxdf1, IsDiffuse(NotNull()))
      .WillRepeatedly(DoAll(SetArgPointee<0>(1.0), Return(true)));
  EXPECT_CALL(bxdf1, IsDiffuse(IsNull())).WillRepeatedly(Return(true));

  EXPECT_CALL(bxdf1, PdfDiffuse(Vector(1.0, 0.0, 0.0), Vector(-1.0, 0.0, 0.0),
                                Vector(0.0, 0.0, 1.0), Bxdf::Hemisphere::BRDF))
      .WillOnce(Return(static_cast<visual_t>(2.0)));

  const Bxdf* composite = MakeCompositeBxdf(GetBxdfAllocator(), &bxdf0, &bxdf1);
  EXPECT_EQ(1.5, composite->PdfDiffuse(
                     Vector(1.0, 0.0, 0.0), Vector(-1.0, 0.0, 0.0),
                     Vector(0.0, 0.0, 1.0), Bxdf::Hemisphere::BRDF));
}

TEST(CompositeBxdfTest, PdfTwoBxdfsOneSpecular) {
  MockBxdf bxdf0;
  EXPECT_CALL(bxdf0, IsDiffuse(NotNull()))
      .WillRepeatedly(DoAll(SetArgPointee<0>(0.0), Return(false)));
  EXPECT_CALL(bxdf0, IsDiffuse(IsNull())).WillRepeatedly(Return(false));

  MockBxdf bxdf1;
  EXPECT_CALL(bxdf1, IsDiffuse(NotNull()))
      .WillRepeatedly(DoAll(SetArgPointee<0>(1.0), Return(true)));
  EXPECT_CALL(bxdf1, IsDiffuse(IsNull())).WillRepeatedly(Return(true));

  EXPECT_CALL(bxdf1, PdfDiffuse(Vector(1.0, 0.0, 0.0), Vector(-1.0, 0.0, 0.0),
                                Vector(0.0, 0.0, 1.0), Bxdf::Hemisphere::BRDF))
      .WillOnce(Return(static_cast<visual_t>(3.0)));

  const Bxdf* composite = MakeCompositeBxdf(GetBxdfAllocator(), &bxdf0, &bxdf1);
  EXPECT_EQ(3.0, composite->PdfDiffuse(
                     Vector(1.0, 0.0, 0.0), Vector(-1.0, 0.0, 0.0),
                     Vector(0.0, 0.0, 1.0), Bxdf::Hemisphere::BRDF));
}

TEST(CompositeBxdfTest, Reflectance) {
  MockReflector reflector0;
  EXPECT_CALL(reflector0, Reflectance(1.0)).WillOnce(Return(0.25));
  MockBxdf bxdf0;
  EXPECT_CALL(bxdf0, IsDiffuse(NotNull()))
      .WillRepeatedly(DoAll(SetArgPointee<0>(1.0), Return(true)));
  EXPECT_CALL(bxdf0, IsDiffuse(IsNull())).WillRepeatedly(Return(true));

  EXPECT_CALL(bxdf0,
              ReflectanceDiffuse(Vector(1.0, 0.0, 0.0), Vector(-1.0, 0.0, 0.0),
                                 Bxdf::Hemisphere::BRDF, _))
      .WillOnce(Return(&reflector0));
  MockReflector reflector1;
  EXPECT_CALL(reflector1, Reflectance(1.0)).WillOnce(Return(0.5));

  MockBxdf bxdf1;
  EXPECT_CALL(bxdf1, IsDiffuse(NotNull()))
      .WillRepeatedly(DoAll(SetArgPointee<0>(1.0), Return(true)));
  EXPECT_CALL(bxdf1, IsDiffuse(IsNull())).WillRepeatedly(Return(true));

  EXPECT_CALL(bxdf1,
              ReflectanceDiffuse(Vector(1.0, 0.0, 0.0), Vector(-1.0, 0.0, 0.0),
                                 Bxdf::Hemisphere::BRDF, _))
      .WillOnce(Return(&reflector1));

  const Bxdf* composite = MakeCompositeBxdf(GetBxdfAllocator(), &bxdf0, &bxdf1);
  const Reflector* result = composite->ReflectanceDiffuse(
      Vector(1.0, 0.0, 0.0), Vector(-1.0, 0.0, 0.0), Bxdf::Hemisphere::BRDF,
      testing::GetSpectralAllocator());
  EXPECT_EQ(0.75, result->Reflectance(1.0));
}

TEST(CompositeBxdfTest, MakeCompositeWithNullptr) {
  MockBxdf bxdf0;
  EXPECT_CALL(bxdf0, IsDiffuse(NotNull()))
      .WillRepeatedly(DoAll(SetArgPointee<0>(1.0), Return(true)));
  EXPECT_CALL(bxdf0, IsDiffuse(IsNull())).WillRepeatedly(Return(true));

  EXPECT_CALL(bxdf0, PdfDiffuse(Vector(1.0, 0.0, 0.0), Vector(-1.0, 0.0, 0.0),
                                Vector(0.0, 0.0, 1.0), Bxdf::Hemisphere::BRDF))
      .WillOnce(Return(1.0));

  MockBxdf bxdf1;
  EXPECT_CALL(bxdf1, IsDiffuse(NotNull()))
      .WillRepeatedly(DoAll(SetArgPointee<0>(1.0), Return(true)));
  EXPECT_CALL(bxdf1, IsDiffuse(IsNull())).WillRepeatedly(Return(true));

  EXPECT_CALL(bxdf1, PdfDiffuse(Vector(1.0, 0.0, 0.0), Vector(-1.0, 0.0, 0.0),
                                Vector(0.0, 0.0, 1.0), Bxdf::Hemisphere::BRDF))
      .WillOnce(Return(static_cast<visual_t>(2.0)));

  const Bxdf* composite = MakeCompositeBxdf(GetBxdfAllocator(), &bxdf0, &bxdf1,
                                            nullptr, nullptr, nullptr);
  ASSERT_TRUE(composite);

  EXPECT_EQ(1.5, composite->PdfDiffuse(
                     Vector(1.0, 0.0, 0.0), Vector(-1.0, 0.0, 0.0),
                     Vector(0.0, 0.0, 1.0), Bxdf::Hemisphere::BRDF));
}

}  // namespace
}  // namespace bxdfs
}  // namespace iris
