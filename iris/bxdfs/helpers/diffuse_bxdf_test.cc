#include "iris/bxdfs/helpers/diffuse_bxdf.h"

#include "googlemock/include/gmock/gmock.h"
#include "googletest/include/gtest/gtest.h"
#include "iris/random/mock_random.h"
#include "iris/testing/spectral_allocator.h"

namespace iris {
namespace bxdfs {
namespace helpers {
namespace {

using ::iris::random::MockRandom;
using ::iris::testing::GetSpectralAllocator;
using ::testing::FieldsAre;
using ::testing::Ref;
using ::testing::Return;

class MockDiffuseBxdf final : public DiffuseBxdf {
 public:
  MOCK_METHOD(std::optional<Vector>, SampleDiffuse,
              (const Vector&, const Vector&, Sampler&), (const override));
  MOCK_METHOD(visual_t, PdfDiffuse,
              (const Vector&, const Vector&, const Vector&, Bxdf::Hemisphere),
              (const override));
  MOCK_METHOD(const Reflector*, ReflectanceDiffuse,
              (const Vector&, const Vector&, Bxdf::Hemisphere,
               SpectralAllocator&),
              (const override));
};

TEST(DiffuseBxdfTest, IsDiffuse) {
  MockDiffuseBxdf bxdf;
  EXPECT_TRUE(bxdf.IsDiffuse(nullptr));

  visual_t diffuse_pdf;
  EXPECT_TRUE(bxdf.IsDiffuse(&diffuse_pdf));
  EXPECT_EQ(1.0, diffuse_pdf);
}

TEST(DiffuseBxdfTest, Sample) {
  MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2)).Times(1);
  Sampler sampler(rng);

  MockDiffuseBxdf bxdf;
  EXPECT_CALL(bxdf, SampleDiffuse(Vector(0.0, 1.0, 0.0), Vector(0.0, 0.0, 1.0),
                                  Ref(sampler)))
      .WillOnce(Return(Vector(1.0, 2.0, 3.0)));
  auto sample =
      bxdf.Sample(Vector(0.0, 1.0, 0.0), std::nullopt, Vector(0.0, 0.0, 1.0),
                  sampler, GetSpectralAllocator());
  EXPECT_THAT(std::get<Bxdf::DiffuseSample>(sample),
              FieldsAre(Vector(1.0, 2.0, 3.0)));
}

TEST(DiffuseBxdfTest, SampleFails) {
  MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2)).Times(1);
  Sampler sampler(rng);

  MockDiffuseBxdf bxdf;
  EXPECT_CALL(bxdf, SampleDiffuse(Vector(0.0, 1.0, 0.0), Vector(0.0, 0.0, 1.0),
                                  Ref(sampler)))
      .WillOnce(Return(std::nullopt));
  auto sample =
      bxdf.Sample(Vector(0.0, 1.0, 0.0), std::nullopt, Vector(0.0, 0.0, 1.0),
                  sampler, GetSpectralAllocator());
  EXPECT_TRUE(std::holds_alternative<std::monostate>(sample));
}

}  // namespace
}  // namespace helpers
}  // namespace bxdfs
}  // namespace iris
