#include "iris/bxdfs/attenuated_bxdf.h"

#include <cmath>

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

TEST(AttenuatedBrdfTest, MakeAttenuatedBxdfNone) {
  MockReflector mock_reflector;
  MockBxdf mock_bxdf;
  EXPECT_EQ(nullptr, MakeAttenuatedBxdf(testing::GetBxdfAllocator(), &mock_bxdf,
                                        nullptr));
  EXPECT_EQ(nullptr, MakeAttenuatedBxdf(testing::GetBxdfAllocator(), nullptr,
                                        &mock_reflector));
}

TEST(AttenuatedBrdfTest, IsDiffuse) {
  MockReflector mock_reflector;
  MockBxdf mock_bxdf;
  EXPECT_CALL(mock_bxdf, IsDiffuse(NotNull()))
      .WillOnce(DoAll(SetArgPointee<0>(0.5), Return(true)));
  EXPECT_CALL(mock_bxdf, IsDiffuse(IsNull())).WillOnce(Return(false));

  const Bxdf* bxdf = MakeAttenuatedBxdf(testing::GetBxdfAllocator(), &mock_bxdf,
                                        &mock_reflector);

  visual_t pdf;
  EXPECT_TRUE(bxdf->IsDiffuse(&pdf));
  EXPECT_EQ(0.5, pdf);

  EXPECT_FALSE(bxdf->IsDiffuse());
}

TEST(AttenuatedBrdfTest, SampleDiffuse) {
  MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2)).Times(1);
  Sampler sampler(rng);

  MockBxdf mock_bxdf;
  EXPECT_CALL(mock_bxdf, SampleDiffuse(Vector(0.0, 0.0, 1.0),
                                       Vector(0.0, 0.0, 1.0), Ref(sampler)))
      .WillOnce(Return(Vector(1.0, 2.0, 3.0)));

  MockReflector mock_reflector;
  const Bxdf* bxdf = MakeAttenuatedBxdf(testing::GetBxdfAllocator(), &mock_bxdf,
                                        &mock_reflector);
  std::optional<Vector> result = bxdf->SampleDiffuse(
      Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0), sampler);
  ASSERT_TRUE(result);
  EXPECT_EQ(Vector(1.0, 2.0, 3.0), *result);
}

TEST(AttenuatedBrdfTest, Sample) {
  MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2)).Times(1);
  Sampler sampler(rng);

  MockBxdf mock_bxdf;
  EXPECT_CALL(mock_bxdf, Sample(Vector(0.0, 0.0, 1.0), Eq(std::nullopt),
                                Vector(0.0, 0.0, 1.0), Ref(sampler), _))
      .WillOnce(Return(Bxdf::DiffuseSample{Vector(1.0, 2.0, 3.0)}));

  MockReflector mock_reflector;
  const Bxdf* bxdf = MakeAttenuatedBxdf(testing::GetBxdfAllocator(), &mock_bxdf,
                                        &mock_reflector);
  auto result =
      bxdf->Sample(Vector(0.0, 0.0, 1.0), std::nullopt, Vector(0.0, 0.0, 1.0),
                   sampler, GetSpectralAllocator());
  EXPECT_THAT(std::get<Bxdf::DiffuseSample>(result),
              FieldsAre(Vector(1.0, 2.0, 3.0)));
}

TEST(AttenuatedBrdfTest, SampleSpecular) {
  MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(2.0)).WillRepeatedly(Return(0.5));

  MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2)).Times(1);
  Sampler sampler(rng);

  MockBxdf mock_bxdf;
  EXPECT_CALL(mock_bxdf, Sample(Vector(0.0, 0.0, 1.0), Eq(std::nullopt),
                                Vector(0.0, 0.0, 1.0), Ref(sampler), _))
      .WillOnce(Return(Bxdf::SpecularSample{Bxdf::Hemisphere::BTDF,
                                            Vector(1.0, 2.0, 3.0), &reflector,
                                            std::nullopt, 1.0}));

  MockReflector attenuation;
  EXPECT_CALL(attenuation, Reflectance(_)).WillOnce(Return(0.5));

  const Bxdf* bxdf =
      MakeAttenuatedBxdf(testing::GetBxdfAllocator(), &mock_bxdf, &attenuation);
  auto result =
      bxdf->Sample(Vector(0.0, 0.0, 1.0), std::nullopt, Vector(0.0, 0.0, 1.0),
                   sampler, GetSpectralAllocator());
  const Bxdf::SpecularSample& specular_sample =
      std::get<Bxdf::SpecularSample>(result);

  EXPECT_EQ(specular_sample.hemisphere, Bxdf::Hemisphere::BTDF);
  EXPECT_EQ(specular_sample.direction, Vector(1.0, 2.0, 3.0));
  EXPECT_NE(specular_sample.reflectance, &reflector);
  EXPECT_FALSE(specular_sample.differentials);
  EXPECT_EQ(specular_sample.pdf, 1.0);

  EXPECT_EQ(0.25, specular_sample.reflectance->Reflectance(2.0));
}

TEST(AttenuatedBrdfTest, PdfDiffuse) {
  MockBxdf mock_bxdf;
  EXPECT_CALL(mock_bxdf,
              PdfDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, -1.0),
                         Vector(0.0, 0.0, -1.0), Bxdf::Hemisphere::BTDF))
      .WillOnce(Return(0.5));

  MockReflector mock_reflector;
  const Bxdf* bxdf = MakeAttenuatedBxdf(testing::GetBxdfAllocator(), &mock_bxdf,
                                        &mock_reflector);
  EXPECT_EQ(0.5,
            bxdf->PdfDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, -1.0),
                             Vector(0.0, 0.0, -1.0), Bxdf::Hemisphere::BTDF));
}

TEST(AttenuatedBrdfTest, ReflectanceDiffuse) {
  MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(2.0)).WillRepeatedly(Return(0.5));

  MockBxdf mock_bxdf;
  EXPECT_CALL(mock_bxdf,
              ReflectanceDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0),
                                 Bxdf::Hemisphere::BTDF, _))
      .WillOnce(Return(&reflector));

  MockReflector attenuation;
  EXPECT_CALL(attenuation, Reflectance(_)).WillOnce(Return(0.5));

  const Bxdf* bxdf =
      MakeAttenuatedBxdf(testing::GetBxdfAllocator(), &mock_bxdf, &attenuation);
  auto* result = bxdf->ReflectanceDiffuse(
      Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0), Bxdf::Hemisphere::BTDF,
      testing::GetSpectralAllocator());
  ASSERT_TRUE(result);
  EXPECT_EQ(0.25, result->Reflectance(2.0));
}

}  // namespace
}  // namespace bxdfs
}  // namespace iris
