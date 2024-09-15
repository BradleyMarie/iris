#include "iris/bxdfs/ashikhmin_shirley_brdf.h"

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

class TestMicrofacetDistribution final : public MicrofacetDistribution {
 public:
  TestMicrofacetDistribution(Vector vector = Vector(0.0, 0.0, 1.0))
      : vector_(vector) {}

  visual_t D(const Vector& vector) const override { return 1.0; }

  visual_t Lambda(const Vector& vector) const override { return 1.0; }

  Vector Sample(const Vector& incoming, geometric_t u,
                geometric_t v) const override {
    return vector_;
  }

 private:
  Vector vector_;
};

TEST(AshikhminShirleyBrdf, IsDiffuse) {
  MockReflector reflector;
  TestMicrofacetDistribution distribution;
  AshikhminShirleyBrdf brdf(reflector, reflector, distribution);
  EXPECT_TRUE(brdf.IsDiffuse(nullptr));

  visual_t diffuse_pdf;
  EXPECT_TRUE(brdf.IsDiffuse(&diffuse_pdf));
  EXPECT_EQ(1.0, diffuse_pdf);
}

TEST(AshikhminShirleyBrdf, SampleDiffuseMicrofacet) {
  MockRandom rng;
  EXPECT_CALL(rng, NextGeometric()).Times(2).WillRepeatedly(Return(0.49));
  Sampler sampler(rng);

  MockReflector reflector;
  TestMicrofacetDistribution distribution;
  AshikhminShirleyBrdf brdf(reflector, reflector, distribution);

  std::optional<Vector> sample =
      brdf.SampleDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0), sampler);
  ASSERT_TRUE(sample);
  EXPECT_NEAR(sample->x, 0.0, 0.001);
  EXPECT_NEAR(sample->y, 0.0, 0.001);
  EXPECT_NEAR(sample->z, 1.0, 0.001);
}

TEST(AshikhminShirleyBrdf, SampleDiffuseLambertian) {
  MockRandom rng;
  EXPECT_CALL(rng, NextGeometric()).Times(2).WillRepeatedly(Return(0.51));
  Sampler sampler(rng);

  MockReflector reflector;
  TestMicrofacetDistribution distribution;
  AshikhminShirleyBrdf brdf(reflector, reflector, distribution);

  std::optional<Vector> sample =
      brdf.SampleDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0), sampler);
  ASSERT_TRUE(sample);
  EXPECT_NEAR(sample->x, -0.9598, 0.001);
  EXPECT_NEAR(sample->y, 0.01570, 0.001);
  EXPECT_NEAR(sample->z, 0.27999, 0.001);
}

TEST(AshikhminShirleyBrdf, SampleMicrofacet) {
  MockRandom rng;
  EXPECT_CALL(rng, NextGeometric()).Times(2).WillRepeatedly(Return(0.49));
  Sampler sampler(rng);

  MockReflector reflector;
  TestMicrofacetDistribution distribution;
  AshikhminShirleyBrdf brdf(reflector, reflector, distribution);

  std::optional<Bxdf::SampleResult> sample = brdf.Sample(
      Vector(0.0, 0.0, 1.0), std::nullopt, Vector(0.0, 0.0, 1.0), sampler);
  ASSERT_TRUE(sample);
  EXPECT_FALSE(sample->differentials);
  EXPECT_NEAR(sample->direction.x, 0.0, 0.001);
  EXPECT_NEAR(sample->direction.y, 0.0, 0.001);
  EXPECT_NEAR(sample->direction.z, 1.0, 0.001);
  EXPECT_EQ(sample->pdf_weight, 1.0);
}

TEST(AshikhminShirleyBrdf, SampleLambertian) {
  MockRandom rng;
  EXPECT_CALL(rng, NextGeometric()).Times(2).WillRepeatedly(Return(0.51));
  Sampler sampler(rng);

  MockReflector reflector;
  TestMicrofacetDistribution distribution;
  AshikhminShirleyBrdf brdf(reflector, reflector, distribution);

  std::optional<Bxdf::SampleResult> sample = brdf.Sample(
      Vector(0.0, 0.0, 1.0), std::nullopt, Vector(0.0, 0.0, 1.0), sampler);
  ASSERT_TRUE(sample);
  EXPECT_FALSE(sample->differentials);
  EXPECT_NEAR(sample->direction.x, -0.9598, 0.001);
  EXPECT_NEAR(sample->direction.y, 0.01570, 0.001);
  EXPECT_NEAR(sample->direction.z, 0.27999, 0.001);
  EXPECT_EQ(sample->pdf_weight, 1.0);
}

TEST(AshikhminShirleyBrdf, PdfBTDF) {
  MockReflector reflector;
  TestMicrofacetDistribution distribution;
  AshikhminShirleyBrdf brdf(reflector, reflector, distribution);

  EXPECT_EQ(0.0, brdf.Pdf(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, -1.0),
                          Vector(0.0, 0.0, -1.0), Bxdf::Hemisphere::BTDF));
}

TEST(AshikhminShirleyBrdf, PdfNoHalfAngle) {
  MockReflector reflector;
  TestMicrofacetDistribution distribution;
  AshikhminShirleyBrdf brdf(reflector, reflector, distribution);

  EXPECT_EQ(0.0, brdf.Pdf(Vector(0.0, 1.0, 0.0), Vector(0.0, -1.0, 0.0),
                          Vector(0.0, 0.0, -1.0), Bxdf::Hemisphere::BRDF));
}

TEST(AshikhminShirleyBrdf, Pdf) {
  MockReflector reflector;
  TestMicrofacetDistribution distribution;
  AshikhminShirleyBrdf brdf(reflector, reflector, distribution);

  EXPECT_NEAR(0.28415,
              brdf.Pdf(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0),
                       Vector(0.0, 0.0, 1.0), Bxdf::Hemisphere::BRDF),
              0.001);
}

TEST(AshikhminShirleyBrdf, ReflectanceWrongHemishphere) {
  MockReflector reflector;
  TestMicrofacetDistribution distribution;
  AshikhminShirleyBrdf brdf(reflector, reflector, distribution);

  EXPECT_EQ(nullptr,
            brdf.Reflectance(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0),
                             Bxdf::Hemisphere::BTDF, GetSpectralAllocator()));
}

TEST(AshikhminShirleyBrdf, ReflectanceNoHalfAngle) {
  MockReflector reflector;
  TestMicrofacetDistribution distribution;
  AshikhminShirleyBrdf brdf(reflector, reflector, distribution);

  EXPECT_EQ(nullptr,
            brdf.Reflectance(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, -1.0),
                             Bxdf::Hemisphere::BRDF, GetSpectralAllocator()));
}

TEST(AshikhminShirleyBrdf, Reflectance) {
  MockReflector diffuse;
  EXPECT_CALL(diffuse, Reflectance(_)).WillRepeatedly(Return(0.1));
  MockReflector specular;
  EXPECT_CALL(specular, Reflectance(_)).WillRepeatedly(Return(0.2));
  TestMicrofacetDistribution distribution;
  AshikhminShirleyBrdf brdf(diffuse, specular, distribution);

  const Reflector* reflector =
      brdf.Reflectance(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0),
                       Bxdf::Hemisphere::BRDF, GetSpectralAllocator());
  ASSERT_TRUE(reflector);
  EXPECT_NEAR(0.0790, reflector->Reflectance(0.5), 0.001);
}

}  // namespace
}  // namespace bxdfs
}  // namespace iris