#include "iris/bxdfs/ashikhmin_shirley_brdf.h"

#include <cmath>

#include "googletest/include/gtest/gtest.h"
#include "iris/reflectors/mock_reflector.h"
#include "iris/testing/bxdf_allocator.h"
#include "iris/testing/sampler.h"
#include "iris/testing/spectral_allocator.h"

namespace iris {
namespace bxdfs {
namespace {

using ::iris::reflectors::MockReflector;
using ::iris::testing::GetBxdfAllocator;
using ::iris::testing::GetSpectralAllocator;
using ::iris::testing::MakeSampler;
using ::testing::_;
using ::testing::Return;

TEST(AshikhminShirleyBrdf, SampleDiffuseMicrofacet) {
  Sampler sampler = MakeSampler({0.25}, {{0.25, 0.25}});

  MockReflector reflector;
  const Bxdf* bxdf = MakeAshikhminShirleyBrdf(GetBxdfAllocator(), &reflector,
                                              &reflector, 0.0, 0.0, true);

  std::optional<Vector> sample = bxdf->SampleDiffuse(
      Normalize(Vector(1.0, 1.0, 1.0)), Vector(0.0, 0.0, 1.0), sampler);
  ASSERT_TRUE(sample);
  EXPECT_NEAR(sample->x, -0.59565, 0.001);
  EXPECT_NEAR(sample->y, -0.59572, 0.001);
  EXPECT_NEAR(sample->z, 0.53880, 0.001);
}

TEST(AshikhminShirleyBrdf, SampleDiffuseLambertian) {
  Sampler sampler = MakeSampler({0.75}, {{0.51, 0.51}});

  MockReflector reflector;
  const Bxdf* bxdf = MakeAshikhminShirleyBrdf(GetBxdfAllocator(), &reflector,
                                              &reflector, 0.5, 0.5, true);

  std::optional<Vector> sample = bxdf->SampleDiffuse(
      Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0), sampler);
  ASSERT_TRUE(sample);
  EXPECT_NEAR(sample->x, 0.01414, 0.001);
  EXPECT_NEAR(sample->y, 0.01414, 0.001);
  EXPECT_NEAR(sample->z, 0.99979, 0.001);
}

TEST(AshikhminShirleyBrdf, PdfBTDF) {
  MockReflector reflector;
  const Bxdf* bxdf = MakeAshikhminShirleyBrdf(GetBxdfAllocator(), &reflector,
                                              &reflector, 0.5, 0.5, true);

  EXPECT_EQ(0.0,
            bxdf->PdfDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, -1.0),
                             Vector(0.0, 0.0, -1.0), Bxdf::Hemisphere::BTDF));
}

TEST(AshikhminShirleyBrdf, PdfNoIncomingZ) {
  MockReflector reflector;
  const Bxdf* bxdf = MakeAshikhminShirleyBrdf(GetBxdfAllocator(), &reflector,
                                              &reflector, 0.5, 0.5, true);

  EXPECT_NEAR(0.159154,
              bxdf->PdfDiffuse(Vector(0.0, 1.0, 0.0), Vector(0.0, 0.0, 1.0),
                               Vector(0.0, 0.0, 1.0), Bxdf::Hemisphere::BRDF),
              0.001);
}

TEST(AshikhminShirleyBrdf, PdfNoOutgoingZ) {
  MockReflector reflector;
  const Bxdf* bxdf = MakeAshikhminShirleyBrdf(GetBxdfAllocator(), &reflector,
                                              &reflector, 0.5, 0.5, true);

  EXPECT_EQ(0.0,
            bxdf->PdfDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 1.0, 0.0),
                             Vector(0.0, 0.0, 1.0), Bxdf::Hemisphere::BRDF));
}

TEST(AshikhminShirleyBrdf, PdfOppositeHemispheres) {
  MockReflector reflector;
  const Bxdf* bxdf = MakeAshikhminShirleyBrdf(GetBxdfAllocator(), &reflector,
                                              &reflector, 0.5, 0.5, true);

  EXPECT_NEAR(0.159154,
              bxdf->PdfDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, -1.0),
                               Vector(0.0, 0.0, 1.0), Bxdf::Hemisphere::BRDF),
              0.001);
}

TEST(AshikhminShirleyBrdf, Pdf) {
  MockReflector reflector;
  const Bxdf* bxdf = MakeAshikhminShirleyBrdf(GetBxdfAllocator(), &reflector,
                                              &reflector, 0.5, 0.5, true);

  EXPECT_NEAR(0.190269,
              bxdf->PdfDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0),
                               Vector(0.0, 0.0, 1.0), Bxdf::Hemisphere::BRDF),
              0.001);
}

TEST(AshikhminShirleyBrdf, ReflectanceWrongHemishphere) {
  MockReflector reflector;
  const Bxdf* bxdf = MakeAshikhminShirleyBrdf(GetBxdfAllocator(), &reflector,
                                              &reflector, 0.5, 0.5, true);

  EXPECT_EQ(nullptr, bxdf->ReflectanceDiffuse(
                         Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0),
                         Bxdf::Hemisphere::BTDF, GetSpectralAllocator()));
}

TEST(AshikhminShirleyBrdf, ReflectanceNoIncomingZ) {
  MockReflector reflector;
  const Bxdf* bxdf = MakeAshikhminShirleyBrdf(GetBxdfAllocator(), &reflector,
                                              &reflector, 0.5, 0.5, true);

  EXPECT_EQ(nullptr, bxdf->ReflectanceDiffuse(
                         Vector(0.0, 1.0, 0.0), Vector(0.0, 0.0, 1.0),
                         Bxdf::Hemisphere::BRDF, GetSpectralAllocator()));
}

TEST(AshikhminShirleyBrdf, ReflectanceNoOutgoingZ) {
  MockReflector reflector;
  const Bxdf* bxdf = MakeAshikhminShirleyBrdf(GetBxdfAllocator(), &reflector,
                                              &reflector, 0.5, 0.5, true);

  EXPECT_EQ(nullptr, bxdf->ReflectanceDiffuse(
                         Vector(0.0, 0.0, 1.0), Vector(0.0, 1.0, 0.0),
                         Bxdf::Hemisphere::BRDF, GetSpectralAllocator()));
}

TEST(AshikhminShirleyBrdf, ReflectanceOppositeHemispheres) {
  MockReflector diffuse;
  EXPECT_CALL(diffuse, Reflectance(_)).WillRepeatedly(Return(0.1));
  MockReflector specular;
  EXPECT_CALL(specular, Reflectance(_)).WillRepeatedly(Return(0.2));
  const Bxdf* bxdf = MakeAshikhminShirleyBrdf(GetBxdfAllocator(), &diffuse,
                                              &specular, 0.5, 0.5, true);

  const Reflector* reflectance =
      bxdf->ReflectanceDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, -1.0),
                               Bxdf::Hemisphere::BRDF, GetSpectralAllocator());
  ASSERT_TRUE(reflectance);
  EXPECT_NEAR(0.0290, reflectance->Reflectance(0.5), 0.001);
}

TEST(AshikhminShirleyBrdf, Reflectance) {
  MockReflector diffuse;
  EXPECT_CALL(diffuse, Reflectance(_)).WillRepeatedly(Return(0.1));
  MockReflector specular;
  EXPECT_CALL(specular, Reflectance(_)).WillRepeatedly(Return(0.2));
  const Bxdf* bxdf = MakeAshikhminShirleyBrdf(GetBxdfAllocator(), &diffuse,
                                              &specular, 0.5, 0.5, true);

  const Reflector* reflectance =
      bxdf->ReflectanceDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0),
                               Bxdf::Hemisphere::BRDF, GetSpectralAllocator());
  ASSERT_TRUE(reflectance);
  EXPECT_NEAR(0.041539, reflectance->Reflectance(0.5), 0.001);
}

}  // namespace
}  // namespace bxdfs
}  // namespace iris
