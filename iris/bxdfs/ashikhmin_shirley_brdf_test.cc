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

TEST(AshikhminShirleyBrdf, PdfBTDF) {
  MockReflector reflector;
  TestMicrofacetDistribution distribution;
  AshikhminShirleyBrdf brdf(reflector, reflector, distribution);

  EXPECT_EQ(0.0,
            brdf.PdfDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, -1.0),
                            Vector(0.0, 0.0, -1.0), Bxdf::Hemisphere::BTDF));
}

TEST(AshikhminShirleyBrdf, PdfNoIncomingZ) {
  MockReflector reflector;
  TestMicrofacetDistribution distribution;
  AshikhminShirleyBrdf brdf(reflector, reflector, distribution);

  EXPECT_NEAR(0.159154,
              brdf.PdfDiffuse(Vector(0.0, 1.0, 0.0), Vector(0.0, 0.0, 1.0),
                              Vector(0.0, 0.0, 1.0), Bxdf::Hemisphere::BRDF),
              0.001);
}

TEST(AshikhminShirleyBrdf, PdfNoOutgoingZ) {
  MockReflector reflector;
  TestMicrofacetDistribution distribution;
  AshikhminShirleyBrdf brdf(reflector, reflector, distribution);

  EXPECT_EQ(0.0,
            brdf.PdfDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 1.0, 0.0),
                            Vector(0.0, 0.0, 1.0), Bxdf::Hemisphere::BRDF));
}

TEST(AshikhminShirleyBrdf, PdfOppositeHemispheres) {
  MockReflector reflector;
  TestMicrofacetDistribution distribution;
  AshikhminShirleyBrdf brdf(reflector, reflector, distribution);

  EXPECT_NEAR(0.159154,
              brdf.PdfDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, -1.0),
                              Vector(0.0, 0.0, 1.0), Bxdf::Hemisphere::BRDF),
              0.001);
}

TEST(AshikhminShirleyBrdf, Pdf) {
  MockReflector reflector;
  TestMicrofacetDistribution distribution;
  AshikhminShirleyBrdf brdf(reflector, reflector, distribution);

  EXPECT_NEAR(0.22165,
              brdf.PdfDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0),
                              Vector(0.0, 0.0, 1.0), Bxdf::Hemisphere::BRDF),
              0.001);
}

TEST(AshikhminShirleyBrdf, ReflectanceWrongHemishphere) {
  MockReflector reflector;
  TestMicrofacetDistribution distribution;
  AshikhminShirleyBrdf brdf(reflector, reflector, distribution);

  EXPECT_EQ(nullptr, brdf.ReflectanceDiffuse(
                         Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0),
                         Bxdf::Hemisphere::BTDF, GetSpectralAllocator()));
}

TEST(AshikhminShirleyBrdf, ReflectanceNoIncomingZ) {
  MockReflector reflector;
  TestMicrofacetDistribution distribution;
  AshikhminShirleyBrdf brdf(reflector, reflector, distribution);

  EXPECT_EQ(nullptr, brdf.ReflectanceDiffuse(
                         Vector(0.0, 1.0, 0.0), Vector(0.0, 0.0, 1.0),
                         Bxdf::Hemisphere::BRDF, GetSpectralAllocator()));
}

TEST(AshikhminShirleyBrdf, ReflectanceNoOutgoingZ) {
  MockReflector reflector;
  TestMicrofacetDistribution distribution;
  AshikhminShirleyBrdf brdf(reflector, reflector, distribution);

  EXPECT_EQ(nullptr, brdf.ReflectanceDiffuse(
                         Vector(0.0, 0.0, 1.0), Vector(0.0, 1.0, 0.0),
                         Bxdf::Hemisphere::BRDF, GetSpectralAllocator()));
}

TEST(AshikhminShirleyBrdf, ReflectanceOppositeHemispheres) {
  MockReflector diffuse;
  EXPECT_CALL(diffuse, Reflectance(_)).WillRepeatedly(Return(0.1));
  MockReflector specular;
  EXPECT_CALL(specular, Reflectance(_)).WillRepeatedly(Return(0.2));
  TestMicrofacetDistribution distribution;
  AshikhminShirleyBrdf brdf(diffuse, specular, distribution);

  const Reflector* reflectance =
      brdf.ReflectanceDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, -1.0),
                              Bxdf::Hemisphere::BRDF, GetSpectralAllocator());
  ASSERT_TRUE(reflectance);
  EXPECT_NEAR(0.0290, reflectance->Reflectance(0.5), 0.001);
}

TEST(AshikhminShirleyBrdf, Reflectance) {
  MockReflector diffuse;
  EXPECT_CALL(diffuse, Reflectance(_)).WillRepeatedly(Return(0.1));
  MockReflector specular;
  EXPECT_CALL(specular, Reflectance(_)).WillRepeatedly(Return(0.2));
  TestMicrofacetDistribution distribution;
  AshikhminShirleyBrdf brdf(diffuse, specular, distribution);

  const Reflector* reflectance =
      brdf.ReflectanceDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0),
                              Bxdf::Hemisphere::BRDF, GetSpectralAllocator());
  ASSERT_TRUE(reflectance);
  EXPECT_NEAR(0.0790, reflectance->Reflectance(0.5), 0.001);
}

}  // namespace
}  // namespace bxdfs
}  // namespace iris