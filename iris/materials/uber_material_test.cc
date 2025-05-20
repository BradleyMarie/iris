#include "iris/materials/uber_material.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/reflectors/uniform_reflector.h"
#include "iris/testing/bxdf_allocator.h"
#include "iris/testing/spectral_allocator.h"
#include "iris/textures/constant_texture.h"

namespace iris {
namespace materials {
namespace {

using ::iris::reflectors::CreateUniformReflector;
using ::iris::testing::GetBxdfAllocator;
using ::iris::testing::GetSpectralAllocator;
using ::iris::textures::ConstantPointerTexture2D;
using ::iris::textures::ConstantValueTexture2D;
using ::iris::textures::PointerTexture2D;
using ::iris::textures::ValueTexture2D;

static const ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
    kBlack = MakeReferenceCounted<
        ConstantPointerTexture2D<Reflector, SpectralAllocator>>(
        ReferenceCounted<Reflector>());
static const ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
    kWhite = MakeReferenceCounted<
        ConstantPointerTexture2D<Reflector, SpectralAllocator>>(
        CreateUniformReflector(1.0));
static const ReferenceCounted<ValueTexture2D<visual>> kEtaIncident =
    MakeReferenceCounted<ConstantValueTexture2D<visual>>(1.0);
static const ReferenceCounted<ValueTexture2D<visual>> kEtaTransmitted =
    MakeReferenceCounted<ConstantValueTexture2D<visual>>(1.5);
static const ReferenceCounted<ValueTexture2D<visual>> kRoughness =
    MakeReferenceCounted<ConstantValueTexture2D<visual>>(1.0);
static const ReferenceCounted<ValueTexture2D<visual>> kTransparent =
    MakeReferenceCounted<ConstantValueTexture2D<visual>>(0.0);
static const ReferenceCounted<ValueTexture2D<visual>> kTranslucent =
    MakeReferenceCounted<ConstantValueTexture2D<visual>>(0.5);
static const ReferenceCounted<ValueTexture2D<visual>> kOpaque =
    MakeReferenceCounted<ConstantValueTexture2D<visual>>(1.0);

TEST(UberMaterialTest, NullMaterial) {
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> empty;
  EXPECT_FALSE(MakeUberMaterial(empty, empty, empty, empty, kTransparent,
                                kEtaIncident, kEtaTransmitted, kRoughness,
                                kRoughness, false));
  EXPECT_FALSE(MakeUberMaterial(kWhite, empty, empty, empty, kTransparent,
                                kEtaIncident, kEtaTransmitted, kRoughness,
                                kRoughness, false));
  EXPECT_FALSE(MakeUberMaterial(empty, kWhite, empty, empty, kTransparent,
                                kEtaIncident, kEtaTransmitted, kRoughness,
                                kRoughness, false));
  EXPECT_FALSE(MakeUberMaterial(empty, empty, kWhite, empty, kTransparent,
                                kEtaIncident, kEtaTransmitted, kRoughness,
                                kRoughness, false));
  EXPECT_FALSE(MakeUberMaterial(empty, empty, empty, kWhite, kTransparent,
                                kEtaIncident, kEtaTransmitted, kRoughness,
                                kRoughness, false));
  EXPECT_FALSE(MakeUberMaterial(kWhite, kWhite, empty, empty, kTransparent,
                                kEtaIncident, kEtaTransmitted, kRoughness,
                                kRoughness, false));
  EXPECT_FALSE(MakeUberMaterial(empty, empty, kWhite, kWhite, kTransparent,
                                kEtaIncident, kEtaTransmitted, kRoughness,
                                kRoughness, false));
  EXPECT_TRUE(MakeUberMaterial(kWhite, empty, kWhite, empty, kTransparent,
                               kEtaIncident, kEtaTransmitted, kRoughness,
                               kRoughness, false));
  EXPECT_TRUE(MakeUberMaterial(empty, kWhite, empty, kWhite, kTransparent,
                               kEtaIncident, kEtaTransmitted, kRoughness,
                               kRoughness, false));
}

TEST(UberMaterialTest, TransparencyOnly) {
  ReferenceCounted<Material> material = MakeUberMaterial(
      kWhite, kWhite, kWhite, kWhite, kTransparent, kEtaIncident,
      kEtaTransmitted, kRoughness, kRoughness, false);

  const Bxdf* result =
      material->Evaluate(TextureCoordinates{{0.0, 0.0}, std::nullopt},
                         GetSpectralAllocator(), GetBxdfAllocator());
  ASSERT_TRUE(result);
}

TEST(UberMaterialTest, OpaqueOnlyAllTermsEmpty) {
  ReferenceCounted<Material> material =
      MakeUberMaterial(kBlack, kBlack, kBlack, kBlack, kOpaque, kEtaIncident,
                       kEtaTransmitted, kRoughness, kRoughness, false);

  const Bxdf* result =
      material->Evaluate(TextureCoordinates{{0.0, 0.0}, std::nullopt},
                         GetSpectralAllocator(), GetBxdfAllocator());
  EXPECT_FALSE(result);
}

TEST(UberMaterialTest, LambertianOnly) {
  ReferenceCounted<Material> material =
      MakeUberMaterial(kBlack, kBlack, kWhite, kBlack, kOpaque, kEtaIncident,
                       kEtaTransmitted, kRoughness, kRoughness, false);

  const Bxdf* result =
      material->Evaluate(TextureCoordinates{{0.0, 0.0}, std::nullopt},
                         GetSpectralAllocator(), GetBxdfAllocator());
  ASSERT_TRUE(result);
}

TEST(UberMaterialTest, SpecularReflectionOnly) {
  ReferenceCounted<Material> material =
      MakeUberMaterial(kWhite, kBlack, kBlack, kBlack, kOpaque, kEtaIncident,
                       kEtaTransmitted, kRoughness, kRoughness, false);

  const Bxdf* result =
      material->Evaluate(TextureCoordinates{{0.0, 0.0}, std::nullopt},
                         GetSpectralAllocator(), GetBxdfAllocator());
  ASSERT_TRUE(result);
}

TEST(UberMaterialTest, SpecularTransmissionOnly) {
  ReferenceCounted<Material> material =
      MakeUberMaterial(kBlack, kWhite, kBlack, kBlack, kOpaque, kEtaIncident,
                       kEtaTransmitted, kRoughness, kRoughness, false);

  const Bxdf* result =
      material->Evaluate(TextureCoordinates{{0.0, 0.0}, std::nullopt},
                         GetSpectralAllocator(), GetBxdfAllocator());
  ASSERT_TRUE(result);
}

TEST(UberMaterialTest, MicrofacetOnly) {
  ReferenceCounted<Material> material =
      MakeUberMaterial(kBlack, kBlack, kBlack, kWhite, kOpaque, kEtaIncident,
                       kEtaTransmitted, kRoughness, kRoughness, false);

  const Bxdf* result =
      material->Evaluate(TextureCoordinates{{0.0, 0.0}, std::nullopt},
                         GetSpectralAllocator(), GetBxdfAllocator());
  ASSERT_TRUE(result);
}

TEST(UberMaterialTest, DiffuseAndMicrofacetOnly) {
  ReferenceCounted<Material> material =
      MakeUberMaterial(kBlack, kBlack, kWhite, kWhite, kOpaque, kEtaIncident,
                       kEtaTransmitted, kRoughness, kRoughness, false);

  const Bxdf* result =
      material->Evaluate(TextureCoordinates{{0.0, 0.0}, std::nullopt},
                         GetSpectralAllocator(), GetBxdfAllocator());
  ASSERT_TRUE(result);
  EXPECT_TRUE(result->IsDiffuse());
}

TEST(UberMaterialTest, SpecularOnly) {
  ReferenceCounted<Material> material = MakeUberMaterial(
      kWhite, kWhite, kBlack, kBlack, kTranslucent, kEtaIncident,
      kEtaTransmitted, kRoughness, kRoughness, false);

  const Bxdf* result =
      material->Evaluate(TextureCoordinates{{0.0, 0.0}, std::nullopt},
                         GetSpectralAllocator(), GetBxdfAllocator());
  ASSERT_TRUE(result);
  EXPECT_FALSE(result->IsDiffuse());
}

TEST(UberMaterialTest, All) {
  ReferenceCounted<Material> material = MakeUberMaterial(
      kWhite, kWhite, kWhite, kWhite, kTranslucent, kEtaIncident,
      kEtaTransmitted, kRoughness, kRoughness, true);

  const Bxdf* result =
      material->Evaluate(TextureCoordinates{{0.0, 0.0}, std::nullopt},
                         GetSpectralAllocator(), GetBxdfAllocator());
  ASSERT_TRUE(result);
  EXPECT_TRUE(result->IsDiffuse());
}

}  // namespace
}  // namespace materials
}  // namespace iris
