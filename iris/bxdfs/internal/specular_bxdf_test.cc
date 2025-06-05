#include "iris/bxdfs/internal/specular_bxdf.h"

#include "googlemock/include/gmock/gmock.h"
#include "googletest/include/gtest/gtest.h"
#include "iris/random/mock_random.h"
#include "iris/reflectors/mock_reflector.h"
#include "iris/testing/spectral_allocator.h"

namespace iris {
namespace bxdfs {
namespace internal {
namespace {

using ::iris::random::MockRandom;
using ::iris::reflectors::MockReflector;
using ::iris::testing::GetSpectralAllocator;
using ::testing::_;
using ::testing::Eq;
using ::testing::FieldsAre;
using ::testing::Ref;
using ::testing::Return;

class MockSpecularBxdf final : public SpecularBxdf {
 public:
  MOCK_METHOD(std::optional<Bxdf::SpecularSample>, SampleSpecular,
              (const Vector&, const std::optional<Bxdf::Differentials>&,
               const Vector&, Sampler&, SpectralAllocator&),
              (const override));
};

TEST(SpecularBxdfTest, IsDiffuse) {
  MockSpecularBxdf bxdf;
  EXPECT_FALSE(bxdf.IsDiffuse(nullptr));

  visual_t diffuse_pdf;
  EXPECT_FALSE(bxdf.IsDiffuse(&diffuse_pdf));
  EXPECT_EQ(0.0, diffuse_pdf);
}

TEST(SpecularBxdfTest, Sample) {
  MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2)).Times(1);
  Sampler sampler(rng);
  MockReflector reflector;

  MockSpecularBxdf bxdf;
  EXPECT_CALL(bxdf, SampleSpecular(Vector(0.0, 1.0, 0.0), Eq(std::nullopt),
                                   Vector(0.0, 0.0, 1.0), Ref(sampler), _))
      .WillOnce(Return(Bxdf::SpecularSample{Bxdf::Hemisphere::BRDF,
                                            Vector(1.0, 2.0, 3.0), &reflector,
                                            std::nullopt, 1.0}));
  auto sample =
      bxdf.Sample(Vector(0.0, 1.0, 0.0), std::nullopt, Vector(0.0, 0.0, 1.0),
                  sampler, GetSpectralAllocator());
  EXPECT_THAT(std::get<Bxdf::SpecularSample>(sample),
              FieldsAre(Bxdf::Hemisphere::BRDF, Vector(1.0, 2.0, 3.0),
                        Eq(&reflector), Eq(std::nullopt), Eq(1.0)));
}

TEST(SpecularBxdfTest, SampleFails) {
  MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2)).Times(1);
  Sampler sampler(rng);

  MockSpecularBxdf bxdf;
  EXPECT_CALL(bxdf, SampleSpecular(Vector(0.0, 1.0, 0.0), Eq(std::nullopt),
                                   Vector(0.0, 0.0, 1.0), Ref(sampler), _))
      .WillOnce(Return(std::nullopt));
  auto sample =
      bxdf.Sample(Vector(0.0, 1.0, 0.0), std::nullopt, Vector(0.0, 0.0, 1.0),
                  sampler, GetSpectralAllocator());
  EXPECT_TRUE(std::holds_alternative<std::monostate>(sample));
}

TEST(SpecularBxdfTest, SampleDiffuse) {
  MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2)).Times(1);
  Sampler sampler(rng);

  MockSpecularBxdf bxdf;
  EXPECT_THAT(
      bxdf.SampleDiffuse(Vector(0.0, 1.0, 0.0), Vector(0.0, 0.0, 1.0), sampler),
      Eq(std::nullopt));
}

TEST(SpecularBxdfTest, PdfDiffuse) {
  MockSpecularBxdf bxdf;
  EXPECT_EQ(bxdf.PdfDiffuse(Vector(0.0, 1.0, 0.0), Vector(0.0, 1.0, 0.0),
                            Vector(0.0, 0.0, 1.0), Bxdf::Hemisphere::BRDF),
            0.0);
}

TEST(SpecularBxdfTest, ReflectanceDiffuse) {
  MockSpecularBxdf bxdf;
  EXPECT_EQ(
      bxdf.ReflectanceDiffuse(Vector(0.0, 1.0, 0.0), Vector(0.0, 0.0, 1.0),
                              Bxdf::Hemisphere::BRDF, GetSpectralAllocator()),
      nullptr);
}

}  // namespace
}  // namespace internal
}  // namespace bxdfs
}  // namespace iris
