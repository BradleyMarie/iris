#include "iris/materials/substrate_material.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/reflectors/mock_reflector.h"
#include "iris/testing/bxdf_allocator.h"
#include "iris/testing/spectral_allocator.h"
#include "iris/textures/constant_texture.h"

namespace iris {
namespace materials {
namespace {

using ::iris::reflectors::MockReflector;
using ::iris::testing::GetBxdfAllocator;
using ::iris::testing::GetSpectralAllocator;
using ::iris::textures::ConstantPointerTexture2D;
using ::iris::textures::ConstantValueTexture2D;
using ::iris::textures::PointerTexture2D;
using ::iris::textures::ValueTexture2D;

TEST(SubstrateMaterialTest, NullMaterial) {
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> reflectance =
      MakeReferenceCounted<
          ConstantPointerTexture2D<Reflector, SpectralAllocator>>(
          ReferenceCounted<Reflector>());
  ReferenceCounted<ValueTexture2D<visual>> roughness =
      MakeReferenceCounted<ConstantValueTexture2D<visual>>(1.0);

  EXPECT_FALSE(MakeSubstrateMaterial(
      ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>(),
      ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>(),
      roughness, roughness, true));
  EXPECT_TRUE(MakeSubstrateMaterial(
      reflectance,
      ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>(),
      roughness, roughness, true));
  EXPECT_TRUE(MakeSubstrateMaterial(
      ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>(),
      reflectance, roughness, roughness, true));
}

TEST(SubstrateMaterialTest, EvaluateEmpty) {
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> reflectance =
      MakeReferenceCounted<
          ConstantPointerTexture2D<Reflector, SpectralAllocator>>(
          ReferenceCounted<Reflector>());
  ReferenceCounted<ValueTexture2D<visual>> roughness =
      MakeReferenceCounted<ConstantValueTexture2D<visual>>(1.0);

  ReferenceCounted<Material> material = MakeSubstrateMaterial(
      reflectance, reflectance, roughness, roughness, true);

  ASSERT_FALSE(material->Evaluate(TextureCoordinates{{0.0, 0.0}, std::nullopt},
                                  GetSpectralAllocator(), GetBxdfAllocator()));
}

TEST(SubstrateMaterialTest, Evaluate) {
  ReferenceCounted<Reflector> reflector = MakeReferenceCounted<MockReflector>();

  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> reflectance =
      MakeReferenceCounted<
          ConstantPointerTexture2D<Reflector, SpectralAllocator>>(reflector);
  ReferenceCounted<ValueTexture2D<visual>> roughness =
      MakeReferenceCounted<ConstantValueTexture2D<visual>>(0.0);

  ReferenceCounted<Material> material = MakeSubstrateMaterial(
      reflectance, reflectance, roughness, roughness, true);

  const Bxdf* result =
      material->Evaluate(TextureCoordinates{{0.0, 0.0}, std::nullopt},
                         GetSpectralAllocator(), GetBxdfAllocator());
  ASSERT_TRUE(result);
}

}  // namespace
}  // namespace materials
}  // namespace iris
