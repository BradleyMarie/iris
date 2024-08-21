#include "iris/materials/uber_material.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/reflectors/uniform_reflector.h"
#include "iris/testing/bxdf_allocator.h"
#include "iris/testing/spectral_allocator.h"
#include "iris/textures/constant_texture.h"

namespace iris {
namespace materials {
namespace {

static const ReferenceCounted<
    textures::ConstantPointerTexture2D<Reflector, SpectralAllocator>>
    kBlack = MakeReferenceCounted<
        textures::ConstantPointerTexture2D<Reflector, SpectralAllocator>>(
        ReferenceCounted<Reflector>());
static const ReferenceCounted<
    textures::ConstantPointerTexture2D<Reflector, SpectralAllocator>>
    kWhite = MakeReferenceCounted<
        textures::ConstantPointerTexture2D<Reflector, SpectralAllocator>>(
        MakeReferenceCounted<reflectors::UniformReflector>(1.0));
static const ReferenceCounted<textures::ConstantValueTexture2D<visual>>
    kEtaIncident =
        MakeReferenceCounted<textures::ConstantValueTexture2D<visual>>(
            static_cast<visual>(1.0));
static const ReferenceCounted<textures::ConstantValueTexture2D<visual>>
    kEtaTransmitted =
        MakeReferenceCounted<textures::ConstantValueTexture2D<visual>>(
            static_cast<visual>(1.5));
static const ReferenceCounted<textures::ConstantValueTexture2D<visual>>
    kRoughness = MakeReferenceCounted<textures::ConstantValueTexture2D<visual>>(
        static_cast<visual>(1.0));

TEST(UberMaterialTest, NoOpacityOrTransparency) {
  materials::UberMaterial material(kWhite, kWhite, kWhite, kWhite, kBlack,
                                   kBlack, kEtaIncident, kEtaTransmitted,
                                   kRoughness, kRoughness, false);

  auto* result = material.Evaluate(TextureCoordinates{{0.0, 0.0}, std::nullopt},
                                   testing::GetSpectralAllocator(),
                                   testing::GetBxdfAllocator());
  EXPECT_FALSE(result);
}

TEST(UberMaterialTest, TransparencyOnly) {
  materials::UberMaterial material(kWhite, kWhite, kWhite, kWhite, kBlack,
                                   kWhite, kEtaIncident, kEtaTransmitted,
                                   kRoughness, kRoughness, false);

  auto* result = material.Evaluate(TextureCoordinates{{0.0, 0.0}, std::nullopt},
                                   testing::GetSpectralAllocator(),
                                   testing::GetBxdfAllocator());
  ASSERT_TRUE(result);
  EXPECT_FALSE(result->IsDiffuse());
}

TEST(UberMaterialTest, OpaqueOnlyAllTermsEmpty) {
  materials::UberMaterial material(kBlack, kBlack, kBlack, kBlack, kWhite,
                                   kBlack, kEtaIncident, kEtaTransmitted,
                                   kRoughness, kRoughness, false);

  auto* result = material.Evaluate(TextureCoordinates{{0.0, 0.0}, std::nullopt},
                                   testing::GetSpectralAllocator(),
                                   testing::GetBxdfAllocator());
  EXPECT_FALSE(result);
}

TEST(UberMaterialTest, LambertianOnly) {
  materials::UberMaterial material(kBlack, kBlack, kWhite, kBlack, kWhite,
                                   kBlack, kEtaIncident, kEtaTransmitted,
                                   kRoughness, kRoughness, false);

  auto* result = material.Evaluate(TextureCoordinates{{0.0, 0.0}, std::nullopt},
                                   testing::GetSpectralAllocator(),
                                   testing::GetBxdfAllocator());
  ASSERT_TRUE(result);
  EXPECT_TRUE(result->IsDiffuse());
}

TEST(UberMaterialTest, SpecularReflectionOnly) {
  materials::UberMaterial material(kWhite, kBlack, kBlack, kBlack, kWhite,
                                   kBlack, kEtaIncident, kEtaTransmitted,
                                   kRoughness, kRoughness, false);

  auto* result = material.Evaluate(TextureCoordinates{{0.0, 0.0}, std::nullopt},
                                   testing::GetSpectralAllocator(),
                                   testing::GetBxdfAllocator());
  ASSERT_TRUE(result);
  EXPECT_FALSE(result->IsDiffuse());
}

TEST(UberMaterialTest, SpecularTransmissionOnly) {
  materials::UberMaterial material(kBlack, kWhite, kBlack, kBlack, kWhite,
                                   kBlack, kEtaIncident, kEtaTransmitted,
                                   kRoughness, kRoughness, false);

  auto* result = material.Evaluate(TextureCoordinates{{0.0, 0.0}, std::nullopt},
                                   testing::GetSpectralAllocator(),
                                   testing::GetBxdfAllocator());
  ASSERT_TRUE(result);
  EXPECT_FALSE(result->IsDiffuse());
}

TEST(UberMaterialTest, MicrofacetOnly) {
  materials::UberMaterial material(kBlack, kBlack, kBlack, kWhite, kWhite,
                                   kBlack, kEtaIncident, kEtaTransmitted,
                                   kRoughness, kRoughness, false);

  auto* result = material.Evaluate(TextureCoordinates{{0.0, 0.0}, std::nullopt},
                                   testing::GetSpectralAllocator(),
                                   testing::GetBxdfAllocator());
  ASSERT_TRUE(result);
  EXPECT_TRUE(result->IsDiffuse());
}

TEST(UberMaterialTest, DiffuseAndMicrofacetOnly) {
  materials::UberMaterial material(kBlack, kBlack, kWhite, kWhite, kWhite,
                                   kBlack, kEtaIncident, kEtaTransmitted,
                                   kRoughness, kRoughness, false);

  auto* result = material.Evaluate(TextureCoordinates{{0.0, 0.0}, std::nullopt},
                                   testing::GetSpectralAllocator(),
                                   testing::GetBxdfAllocator());
  ASSERT_TRUE(result);
  EXPECT_TRUE(result->IsDiffuse());
}

TEST(UberMaterialTest, SpecularOnly) {
  materials::UberMaterial material(kWhite, kWhite, kBlack, kBlack, kWhite,
                                   kWhite, kEtaIncident, kEtaTransmitted,
                                   kRoughness, kRoughness, false);

  auto* result = material.Evaluate(TextureCoordinates{{0.0, 0.0}, std::nullopt},
                                   testing::GetSpectralAllocator(),
                                   testing::GetBxdfAllocator());
  ASSERT_TRUE(result);
  EXPECT_FALSE(result->IsDiffuse());
}

TEST(UberMaterialTest, All) {
  materials::UberMaterial material(kWhite, kWhite, kWhite, kWhite, kWhite,
                                   kWhite, kEtaIncident, kEtaTransmitted,
                                   kRoughness, kRoughness, true);

  auto* result = material.Evaluate(TextureCoordinates{{0.0, 0.0}, std::nullopt},
                                   testing::GetSpectralAllocator(),
                                   testing::GetBxdfAllocator());
  ASSERT_TRUE(result);
  EXPECT_TRUE(result->IsDiffuse());
}

}  // namespace
}  // namespace materials
}  // namespace iris