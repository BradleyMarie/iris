#include "iris/materials/translucent_material.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/bxdfs/composite_bxdf.h"
#include "iris/bxdfs/microfacet_bxdf.h"
#include "iris/bxdfs/microfacet_distributions/trowbridge_reitz_distribution.h"
#include "iris/reflectors/mock_reflector.h"
#include "iris/testing/bxdf_allocator.h"
#include "iris/testing/spectral_allocator.h"
#include "iris/textures/constant_texture.h"

namespace iris {
namespace materials {
namespace {

using ::iris::bxdfs::FresnelDielectric;
using ::iris::bxdfs::MicrofacetBrdf;
using ::iris::bxdfs::MicrofacetBtdf;
using ::iris::bxdfs::internal::CompositeBxdf;
using ::iris::bxdfs::microfacet_distributions::TrowbridgeReitzDistribution;
using ::iris::reflectors::MockReflector;
using ::iris::testing::GetBxdfAllocator;
using ::iris::testing::GetSpectralAllocator;
using ::iris::textures::ConstantPointerTexture2D;
using ::iris::textures::ConstantValueTexture2D;
using ::iris::textures::PointerTexture2D;
using ::iris::textures::ValueTexture2D;
using ::testing::_;
using ::testing::Return;

TEST(TranslucentMaterialTest, NullMaterial) {
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> reflectance =
      MakeReferenceCounted<
          ConstantPointerTexture2D<Reflector, SpectralAllocator>>(
          ReferenceCounted<Reflector>());
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
      transmittance = MakeReferenceCounted<
          ConstantPointerTexture2D<Reflector, SpectralAllocator>>(
          ReferenceCounted<Reflector>());
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> diffuse =
      MakeReferenceCounted<
          ConstantPointerTexture2D<Reflector, SpectralAllocator>>(
          ReferenceCounted<Reflector>());
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> specular =
      MakeReferenceCounted<
          ConstantPointerTexture2D<Reflector, SpectralAllocator>>(
          ReferenceCounted<Reflector>());
  ReferenceCounted<ValueTexture2D<visual>> eta_incident =
      MakeReferenceCounted<ConstantValueTexture2D<visual>>(1.0);
  ReferenceCounted<ValueTexture2D<visual>> eta_transmitted =
      MakeReferenceCounted<ConstantValueTexture2D<visual>>(1.5);
  ReferenceCounted<ValueTexture2D<visual>> sigma =
      MakeReferenceCounted<ConstantValueTexture2D<visual>>(1.0);

  EXPECT_FALSE(MakeTranslucentMaterial(
      ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>(),
      ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>(),
      ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>(),
      ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>(),
      eta_incident, eta_transmitted, sigma, false));
  EXPECT_FALSE(MakeTranslucentMaterial(
      diffuse,
      ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>(),
      ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>(),
      ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>(),
      eta_incident, eta_transmitted, sigma, false));
  EXPECT_FALSE(MakeTranslucentMaterial(
      ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>(),
      diffuse,
      ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>(),
      ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>(),
      eta_incident, eta_transmitted, sigma, false));
  EXPECT_FALSE(MakeTranslucentMaterial(
      ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>(),
      ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>(),
      diffuse,
      ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>(),
      eta_incident, eta_transmitted, sigma, false));
  EXPECT_FALSE(MakeTranslucentMaterial(
      ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>(),
      ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>(),
      ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>(),
      diffuse, eta_incident, eta_transmitted, sigma, false));
  EXPECT_FALSE(MakeTranslucentMaterial(
      diffuse, diffuse,
      ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>(),
      ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>(),
      eta_incident, eta_transmitted, sigma, false));
  EXPECT_FALSE(MakeTranslucentMaterial(
      ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>(),
      ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>(),
      diffuse, diffuse, eta_incident, eta_transmitted, sigma, false));
  EXPECT_TRUE(MakeTranslucentMaterial(
      diffuse, diffuse, diffuse,
      ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>(),
      eta_incident, eta_transmitted, sigma, false));
  EXPECT_TRUE(MakeTranslucentMaterial(
      diffuse, diffuse,
      ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>(),
      diffuse, eta_incident, eta_transmitted, sigma, false));
  EXPECT_TRUE(MakeTranslucentMaterial(
      diffuse,
      ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>(),
      diffuse, diffuse, eta_incident, eta_transmitted, sigma, false));
  EXPECT_TRUE(MakeTranslucentMaterial(
      ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>(),
      diffuse, diffuse, diffuse, eta_incident, eta_transmitted, sigma, false));
}

TEST(TranslucentMaterialTest, NoBxdfs) {
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> reflectance =
      MakeReferenceCounted<
          ConstantPointerTexture2D<Reflector, SpectralAllocator>>(
          ReferenceCounted<Reflector>());
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
      transmittance = MakeReferenceCounted<
          ConstantPointerTexture2D<Reflector, SpectralAllocator>>(
          ReferenceCounted<Reflector>());
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> diffuse =
      MakeReferenceCounted<
          ConstantPointerTexture2D<Reflector, SpectralAllocator>>(
          ReferenceCounted<Reflector>());
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> specular =
      MakeReferenceCounted<
          ConstantPointerTexture2D<Reflector, SpectralAllocator>>(
          ReferenceCounted<Reflector>());
  ReferenceCounted<ValueTexture2D<visual>> eta_incident =
      MakeReferenceCounted<ConstantValueTexture2D<visual>>(1.0);
  ReferenceCounted<ValueTexture2D<visual>> eta_transmitted =
      MakeReferenceCounted<ConstantValueTexture2D<visual>>(1.5);
  ReferenceCounted<ValueTexture2D<visual>> sigma =
      MakeReferenceCounted<ConstantValueTexture2D<visual>>(1.0);

  ReferenceCounted<Material> material =
      MakeTranslucentMaterial(reflectance, transmittance, diffuse, specular,
                              eta_incident, eta_transmitted, sigma, false);

  const Bxdf* result =
      material->Evaluate(TextureCoordinates{{0.0, 0.0}, std::nullopt},
                         GetSpectralAllocator(), GetBxdfAllocator());
  ASSERT_FALSE(result);
}

TEST(TranslucentMaterialTest, DiffuseReflection) {
  ReferenceCounted<Reflector> reflector = MakeReferenceCounted<MockReflector>();

  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> reflectance =
      MakeReferenceCounted<
          ConstantPointerTexture2D<Reflector, SpectralAllocator>>(reflector);
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
      transmittance = MakeReferenceCounted<
          ConstantPointerTexture2D<Reflector, SpectralAllocator>>(
          ReferenceCounted<Reflector>());
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> diffuse =
      MakeReferenceCounted<
          ConstantPointerTexture2D<Reflector, SpectralAllocator>>(reflector);
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> specular =
      MakeReferenceCounted<
          ConstantPointerTexture2D<Reflector, SpectralAllocator>>(
          ReferenceCounted<Reflector>());
  ReferenceCounted<ValueTexture2D<visual>> eta_incident =
      MakeReferenceCounted<ConstantValueTexture2D<visual>>(1.0);
  ReferenceCounted<ValueTexture2D<visual>> eta_transmitted =
      MakeReferenceCounted<ConstantValueTexture2D<visual>>(1.5);
  ReferenceCounted<ValueTexture2D<visual>> sigma =
      MakeReferenceCounted<ConstantValueTexture2D<visual>>(1.0);

  ReferenceCounted<Material> material =
      MakeTranslucentMaterial(reflectance, transmittance, diffuse, specular,
                              eta_incident, eta_transmitted, sigma, false);

  const Bxdf* result =
      material->Evaluate(TextureCoordinates{{0.0, 0.0}, std::nullopt},
                         GetSpectralAllocator(), GetBxdfAllocator());
  ASSERT_TRUE(result);
}

TEST(TranslucentMaterialTest, DiffuseTransmission) {
  ReferenceCounted<Reflector> reflector = MakeReferenceCounted<MockReflector>();

  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> reflectance =
      MakeReferenceCounted<
          ConstantPointerTexture2D<Reflector, SpectralAllocator>>(
          ReferenceCounted<Reflector>());
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
      transmittance = MakeReferenceCounted<
          ConstantPointerTexture2D<Reflector, SpectralAllocator>>(reflector);
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> diffuse =
      MakeReferenceCounted<
          ConstantPointerTexture2D<Reflector, SpectralAllocator>>(reflector);
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> specular =
      MakeReferenceCounted<
          ConstantPointerTexture2D<Reflector, SpectralAllocator>>(
          ReferenceCounted<Reflector>());
  ReferenceCounted<ValueTexture2D<visual>> eta_incident =
      MakeReferenceCounted<ConstantValueTexture2D<visual>>(1.0);
  ReferenceCounted<ValueTexture2D<visual>> eta_transmitted =
      MakeReferenceCounted<ConstantValueTexture2D<visual>>(1.5);
  ReferenceCounted<ValueTexture2D<visual>> sigma =
      MakeReferenceCounted<ConstantValueTexture2D<visual>>(1.0);

  ReferenceCounted<Material> material =
      MakeTranslucentMaterial(reflectance, transmittance, diffuse, specular,
                              eta_incident, eta_transmitted, sigma, false);

  const Bxdf* result =
      material->Evaluate(TextureCoordinates{{0.0, 0.0}, std::nullopt},
                         GetSpectralAllocator(), GetBxdfAllocator());
  ASSERT_TRUE(result);
}

TEST(TranslucentMaterialTest, SpecularReflection) {
  ReferenceCounted<Reflector> reflector =
      MakeReferenceCounted<reflectors::MockReflector>();

  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> reflectance =
      MakeReferenceCounted<
          ConstantPointerTexture2D<Reflector, SpectralAllocator>>(reflector);
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
      transmittance = MakeReferenceCounted<
          ConstantPointerTexture2D<Reflector, SpectralAllocator>>(
          ReferenceCounted<Reflector>());
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> diffuse =
      MakeReferenceCounted<
          ConstantPointerTexture2D<Reflector, SpectralAllocator>>(
          ReferenceCounted<Reflector>());
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> specular =
      MakeReferenceCounted<
          ConstantPointerTexture2D<Reflector, SpectralAllocator>>(reflector);
  ReferenceCounted<ValueTexture2D<visual>> eta_incident =
      MakeReferenceCounted<ConstantValueTexture2D<visual>>(1.0);
  ReferenceCounted<ValueTexture2D<visual>> eta_transmitted =
      MakeReferenceCounted<ConstantValueTexture2D<visual>>(1.5);
  ReferenceCounted<ValueTexture2D<visual>> sigma =
      MakeReferenceCounted<ConstantValueTexture2D<visual>>(1.0);

  ReferenceCounted<Material> material =
      MakeTranslucentMaterial(reflectance, transmittance, diffuse, specular,
                              eta_incident, eta_transmitted, sigma, false);

  const Bxdf* result =
      material->Evaluate(TextureCoordinates{{0.0, 0.0}, std::nullopt},
                         GetSpectralAllocator(), GetBxdfAllocator());
  ASSERT_TRUE(result);
  EXPECT_TRUE((dynamic_cast<const MicrofacetBrdf<TrowbridgeReitzDistribution,
                                                 FresnelDielectric>*>(result)));
}

TEST(TranslucentMaterialTest, SpecularTransmission) {
  ReferenceCounted<Reflector> reflector = MakeReferenceCounted<MockReflector>();

  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> reflectance =
      MakeReferenceCounted<
          ConstantPointerTexture2D<Reflector, SpectralAllocator>>(
          ReferenceCounted<Reflector>());
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
      transmittance = MakeReferenceCounted<
          ConstantPointerTexture2D<Reflector, SpectralAllocator>>(reflector);
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> diffuse =
      MakeReferenceCounted<
          ConstantPointerTexture2D<Reflector, SpectralAllocator>>(
          ReferenceCounted<Reflector>());
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> specular =
      MakeReferenceCounted<
          ConstantPointerTexture2D<Reflector, SpectralAllocator>>(reflector);
  ReferenceCounted<ValueTexture2D<visual>> eta_incident =
      MakeReferenceCounted<ConstantValueTexture2D<visual>>(1.0);
  ReferenceCounted<ValueTexture2D<visual>> eta_transmitted =
      MakeReferenceCounted<ConstantValueTexture2D<visual>>(1.5);
  ReferenceCounted<ValueTexture2D<visual>> sigma =
      MakeReferenceCounted<ConstantValueTexture2D<visual>>(1.0);

  ReferenceCounted<Material> material =
      MakeTranslucentMaterial(reflectance, transmittance, diffuse, specular,
                              eta_incident, eta_transmitted, sigma, false);

  const Bxdf* result =
      material->Evaluate(TextureCoordinates{{0.0, 0.0}, std::nullopt},
                         GetSpectralAllocator(), GetBxdfAllocator());
  ASSERT_TRUE(result);
  EXPECT_TRUE((dynamic_cast<const MicrofacetBtdf<TrowbridgeReitzDistribution,
                                                 FresnelDielectric>*>(result)));
}

TEST(TranslucentMaterialTest, All) {
  ReferenceCounted<Reflector> reflector = MakeReferenceCounted<MockReflector>();

  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> reflectance =
      MakeReferenceCounted<
          ConstantPointerTexture2D<Reflector, SpectralAllocator>>(reflector);
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
      transmittance = MakeReferenceCounted<
          ConstantPointerTexture2D<Reflector, SpectralAllocator>>(reflector);
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> diffuse =
      MakeReferenceCounted<
          ConstantPointerTexture2D<Reflector, SpectralAllocator>>(reflector);
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> specular =
      MakeReferenceCounted<
          ConstantPointerTexture2D<Reflector, SpectralAllocator>>(reflector);
  ReferenceCounted<ValueTexture2D<visual>> eta_incident =
      MakeReferenceCounted<ConstantValueTexture2D<visual>>(1.0);
  ReferenceCounted<ValueTexture2D<visual>> eta_transmitted =
      MakeReferenceCounted<ConstantValueTexture2D<visual>>(1.5);
  ReferenceCounted<ValueTexture2D<visual>> sigma =
      MakeReferenceCounted<ConstantValueTexture2D<visual>>(1.0);

  ReferenceCounted<Material> material =
      MakeTranslucentMaterial(reflectance, transmittance, diffuse, specular,
                              eta_incident, eta_transmitted, sigma, true);

  const Bxdf* result =
      material->Evaluate(TextureCoordinates{{0.0, 0.0}, std::nullopt},
                         GetSpectralAllocator(), GetBxdfAllocator());
  ASSERT_TRUE(result);
  EXPECT_TRUE(dynamic_cast<const CompositeBxdf<4>*>(result));
}

}  // namespace
}  // namespace materials
}  // namespace iris
