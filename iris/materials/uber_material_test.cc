#include "iris/materials/uber_material.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/point.h"
#include "iris/reflectors/uniform_reflector.h"
#include "iris/testing/bxdf_allocator.h"
#include "iris/testing/spectral_allocator.h"
#include "iris/textures/constant_texture.h"
#include "iris/textures/float_texture.h"
#include "iris/textures/reflector_texture.h"
#include "iris/textures/test_util.h"

namespace iris {
namespace materials {
namespace {

using ::iris::reflectors::CreateUniformReflector;
using ::iris::testing::GetBxdfAllocator;
using ::iris::testing::GetSpectralAllocator;
using ::iris::textures::FloatTexture;
using ::iris::textures::MakeBlackTexture;
using ::iris::textures::MakeConstantTexture;
using ::iris::textures::ReflectorTexture;

static const ReferenceCounted<ReflectorTexture> kBlack = MakeBlackTexture();
static const ReferenceCounted<ReflectorTexture> kWhite =
    MakeConstantTexture(CreateUniformReflector(1.0));
static const ReferenceCounted<FloatTexture> kEtaIncident =
    MakeConstantTexture(1.0);
static const ReferenceCounted<FloatTexture> kEtaTransmitted =
    MakeConstantTexture(1.5);
static const ReferenceCounted<FloatTexture> kRoughness =
    MakeConstantTexture(1.0);
static const ReferenceCounted<FloatTexture> kTransparent =
    MakeConstantTexture(0.0);
static const ReferenceCounted<FloatTexture> kTranslucent =
    MakeConstantTexture(0.5);
static const ReferenceCounted<FloatTexture> kOpaque = MakeConstantTexture(1.0);

TEST(UberMaterialTest, NoEtaIncident) {
  ReferenceCounted<Material> material = MakeUberMaterial(
      kWhite, kWhite, kWhite, kWhite, kOpaque, ReferenceCounted<FloatTexture>(),
      kEtaTransmitted, kRoughness, kRoughness, true);

  const Bxdf* result = material->Evaluate(
      TextureCoordinates{Point(0.0, 0.0, 0.0), {}, {0.0, 0.0}, std::nullopt},
      GetSpectralAllocator(), GetBxdfAllocator());
  ASSERT_TRUE(result);
  EXPECT_TRUE(result->IsDiffuse());
}

TEST(UberMaterialTest, NoEtaTransmitted) {
  ReferenceCounted<Material> material = MakeUberMaterial(
      kWhite, kWhite, kWhite, kWhite, kOpaque, kEtaIncident,
      ReferenceCounted<FloatTexture>(), kRoughness, kRoughness, true);

  const Bxdf* result = material->Evaluate(
      TextureCoordinates{Point(0.0, 0.0, 0.0), {}, {0.0, 0.0}, std::nullopt},
      GetSpectralAllocator(), GetBxdfAllocator());
  ASSERT_TRUE(result);
  EXPECT_TRUE(result->IsDiffuse());
}

TEST(UberMaterialTest, TransparencyOnly) {
  ReferenceCounted<Material> material = MakeUberMaterial(
      kWhite, kWhite, kWhite, kWhite, kTransparent, kEtaIncident,
      kEtaTransmitted, kRoughness, kRoughness, false);

  const Bxdf* result = material->Evaluate(
      TextureCoordinates{Point(0.0, 0.0, 0.0), {}, {0.0, 0.0}, std::nullopt},
      GetSpectralAllocator(), GetBxdfAllocator());
  ASSERT_TRUE(result);
}

TEST(UberMaterialTest, OpaqueOnlyAllTermsEmpty) {
  ReferenceCounted<Material> material =
      MakeUberMaterial(kBlack, kBlack, kBlack, kBlack, kOpaque, kEtaIncident,
                       kEtaTransmitted, kRoughness, kRoughness, false);

  const Bxdf* result = material->Evaluate(
      TextureCoordinates{Point(0.0, 0.0, 0.0), {}, {0.0, 0.0}, std::nullopt},
      GetSpectralAllocator(), GetBxdfAllocator());
  EXPECT_FALSE(result);
}

TEST(UberMaterialTest, LambertianOnly) {
  ReferenceCounted<Material> material =
      MakeUberMaterial(kBlack, kBlack, kWhite, kBlack, kOpaque, kEtaIncident,
                       kEtaTransmitted, kRoughness, kRoughness, false);

  const Bxdf* result = material->Evaluate(
      TextureCoordinates{Point(0.0, 0.0, 0.0), {}, {0.0, 0.0}, std::nullopt},
      GetSpectralAllocator(), GetBxdfAllocator());
  ASSERT_TRUE(result);
}

TEST(UberMaterialTest, SpecularReflectionOnly) {
  ReferenceCounted<Material> material =
      MakeUberMaterial(kWhite, kBlack, kBlack, kBlack, kOpaque, kEtaIncident,
                       kEtaTransmitted, kRoughness, kRoughness, false);

  const Bxdf* result = material->Evaluate(
      TextureCoordinates{Point(0.0, 0.0, 0.0), {}, {0.0, 0.0}, std::nullopt},
      GetSpectralAllocator(), GetBxdfAllocator());
  ASSERT_TRUE(result);
}

TEST(UberMaterialTest, SpecularTransmissionOnly) {
  ReferenceCounted<Material> material =
      MakeUberMaterial(kBlack, kWhite, kBlack, kBlack, kOpaque, kEtaIncident,
                       kEtaTransmitted, kRoughness, kRoughness, false);

  const Bxdf* result = material->Evaluate(
      TextureCoordinates{Point(0.0, 0.0, 0.0), {}, {0.0, 0.0}, std::nullopt},
      GetSpectralAllocator(), GetBxdfAllocator());
  ASSERT_TRUE(result);
}

TEST(UberMaterialTest, MicrofacetOnly) {
  ReferenceCounted<Material> material =
      MakeUberMaterial(kBlack, kBlack, kBlack, kWhite, kOpaque, kEtaIncident,
                       kEtaTransmitted, kRoughness, kRoughness, false);

  const Bxdf* result = material->Evaluate(
      TextureCoordinates{Point(0.0, 0.0, 0.0), {}, {0.0, 0.0}, std::nullopt},
      GetSpectralAllocator(), GetBxdfAllocator());
  ASSERT_TRUE(result);
}

TEST(UberMaterialTest, DiffuseAndMicrofacetOnly) {
  ReferenceCounted<Material> material =
      MakeUberMaterial(kBlack, kBlack, kWhite, kWhite, kOpaque, kEtaIncident,
                       kEtaTransmitted, kRoughness, kRoughness, false);

  const Bxdf* result = material->Evaluate(
      TextureCoordinates{Point(0.0, 0.0, 0.0), {}, {0.0, 0.0}, std::nullopt},
      GetSpectralAllocator(), GetBxdfAllocator());
  ASSERT_TRUE(result);
  EXPECT_TRUE(result->IsDiffuse());
}

TEST(UberMaterialTest, SpecularOnly) {
  ReferenceCounted<Material> material = MakeUberMaterial(
      kWhite, kWhite, kBlack, kBlack, kTranslucent, kEtaIncident,
      kEtaTransmitted, kRoughness, kRoughness, false);

  const Bxdf* result = material->Evaluate(
      TextureCoordinates{Point(0.0, 0.0, 0.0), {}, {0.0, 0.0}, std::nullopt},
      GetSpectralAllocator(), GetBxdfAllocator());
  ASSERT_TRUE(result);
  EXPECT_FALSE(result->IsDiffuse());
}

TEST(UberMaterialTest, All) {
  ReferenceCounted<Material> material = MakeUberMaterial(
      kWhite, kWhite, kWhite, kWhite, kTranslucent, kEtaIncident,
      kEtaTransmitted, kRoughness, kRoughness, true);

  const Bxdf* result = material->Evaluate(
      TextureCoordinates{Point(0.0, 0.0, 0.0), {}, {0.0, 0.0}, std::nullopt},
      GetSpectralAllocator(), GetBxdfAllocator());
  ASSERT_TRUE(result);
  EXPECT_TRUE(result->IsDiffuse());
}

}  // namespace
}  // namespace materials
}  // namespace iris
