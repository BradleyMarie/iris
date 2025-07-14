#include "iris/materials/glass_material.h"

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

TEST(GlassMaterialTest, NullMaterial) {
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> reflectance =
      MakeReferenceCounted<
          ConstantPointerTexture2D<Reflector, SpectralAllocator>>(
          ReferenceCounted<Reflector>());
  ReferenceCounted<ValueTexture2D<visual>> eta_front =
      MakeReferenceCounted<ConstantValueTexture2D<visual>>(1.0);
  ReferenceCounted<ValueTexture2D<visual>> eta_back =
      MakeReferenceCounted<ConstantValueTexture2D<visual>>(1.0);

  EXPECT_FALSE(MakeGlassMaterial(
      ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>(),
      ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>(),
      eta_front, eta_back, ReferenceCounted<ValueTexture2D<visual>>(),
      ReferenceCounted<ValueTexture2D<visual>>(), true));

  EXPECT_FALSE(MakeGlassMaterial(
      ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>(),
      reflectance, ReferenceCounted<ValueTexture2D<visual>>(), eta_back,
      ReferenceCounted<ValueTexture2D<visual>>(),
      ReferenceCounted<ValueTexture2D<visual>>(), true));

  EXPECT_FALSE(MakeGlassMaterial(
      ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>(),
      reflectance, eta_front, ReferenceCounted<ValueTexture2D<visual>>(),
      ReferenceCounted<ValueTexture2D<visual>>(),
      ReferenceCounted<ValueTexture2D<visual>>(), true));

  EXPECT_TRUE(MakeGlassMaterial(
      reflectance,
      ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>(),
      eta_front, eta_back, ReferenceCounted<ValueTexture2D<visual>>(),
      ReferenceCounted<ValueTexture2D<visual>>(), true));

  EXPECT_TRUE(MakeGlassMaterial(
      ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>(),
      reflectance, eta_front, eta_back,
      ReferenceCounted<ValueTexture2D<visual>>(),
      ReferenceCounted<ValueTexture2D<visual>>(), true));
}

TEST(GlassMaterialTest, EvaluateEmpty) {
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> reflectance =
      MakeReferenceCounted<
          ConstantPointerTexture2D<Reflector, SpectralAllocator>>(
          ReferenceCounted<Reflector>());
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
      transmittance = MakeReferenceCounted<
          ConstantPointerTexture2D<Reflector, SpectralAllocator>>(
          ReferenceCounted<Reflector>());
  ReferenceCounted<ValueTexture2D<visual>> eta_front =
      MakeReferenceCounted<ConstantValueTexture2D<visual>>(1.0);
  ReferenceCounted<ValueTexture2D<visual>> eta_back =
      MakeReferenceCounted<ConstantValueTexture2D<visual>>(1.0);

  ReferenceCounted<Material> material = MakeGlassMaterial(
      std::move(reflectance), std::move(transmittance), std::move(eta_front),
      std::move(eta_back), ReferenceCounted<ValueTexture2D<visual>>(),
      ReferenceCounted<ValueTexture2D<visual>>(), true);

  ASSERT_FALSE(material->Evaluate(TextureCoordinates{{0.0, 0.0}, std::nullopt},
                                  GetSpectralAllocator(), GetBxdfAllocator()));
}

TEST(GlassMaterialTest, EvaluateSpecular) {
  ReferenceCounted<Reflector> reflector = MakeReferenceCounted<MockReflector>();
  ReferenceCounted<Reflector> transmitter =
      MakeReferenceCounted<MockReflector>();

  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> reflectance =
      MakeReferenceCounted<
          ConstantPointerTexture2D<Reflector, SpectralAllocator>>(reflector);
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
      transmittance = MakeReferenceCounted<
          ConstantPointerTexture2D<Reflector, SpectralAllocator>>(transmitter);
  ReferenceCounted<ValueTexture2D<visual>> eta_front =
      MakeReferenceCounted<ConstantValueTexture2D<visual>>(1.0);
  ReferenceCounted<ValueTexture2D<visual>> eta_back =
      MakeReferenceCounted<ConstantValueTexture2D<visual>>(1.0);

  ReferenceCounted<Material> material = MakeGlassMaterial(
      std::move(reflectance), std::move(transmittance), std::move(eta_front),
      std::move(eta_back), ReferenceCounted<ValueTexture2D<visual>>(),
      ReferenceCounted<ValueTexture2D<visual>>(), true);

  const Bxdf* result =
      material->Evaluate(TextureCoordinates{{0.0, 0.0}, std::nullopt},
                         GetSpectralAllocator(), GetBxdfAllocator());
  ASSERT_TRUE(result);
}

TEST(GlassMaterialTest, EvaluateBrdf) {
  ReferenceCounted<Reflector> reflector = MakeReferenceCounted<MockReflector>();

  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> reflectance =
      MakeReferenceCounted<
          ConstantPointerTexture2D<Reflector, SpectralAllocator>>(reflector);
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
      transmittance = MakeReferenceCounted<
          ConstantPointerTexture2D<Reflector, SpectralAllocator>>(
          ReferenceCounted<Reflector>());
  ReferenceCounted<ValueTexture2D<visual>> eta_front =
      MakeReferenceCounted<ConstantValueTexture2D<visual>>(1.0);
  ReferenceCounted<ValueTexture2D<visual>> eta_back =
      MakeReferenceCounted<ConstantValueTexture2D<visual>>(1.0);
  ReferenceCounted<ValueTexture2D<visual>> roughness =
      MakeReferenceCounted<ConstantValueTexture2D<visual>>(1.0);

  ReferenceCounted<Material> material = MakeGlassMaterial(
      std::move(reflectance), std::move(transmittance), std::move(eta_front),
      std::move(eta_back), roughness, roughness, true);

  const Bxdf* result =
      material->Evaluate(TextureCoordinates{{0.0, 0.0}, std::nullopt},
                         GetSpectralAllocator(), GetBxdfAllocator());
  ASSERT_TRUE(result);
}

TEST(GlassMaterialTest, EvaluateBtdf) {
  ReferenceCounted<Reflector> transmitter =
      MakeReferenceCounted<MockReflector>();

  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> reflectance =
      MakeReferenceCounted<
          ConstantPointerTexture2D<Reflector, SpectralAllocator>>(
          ReferenceCounted<Reflector>());
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
      transmittance = MakeReferenceCounted<
          ConstantPointerTexture2D<Reflector, SpectralAllocator>>(transmitter);
  ReferenceCounted<ValueTexture2D<visual>> eta_front =
      MakeReferenceCounted<ConstantValueTexture2D<visual>>(1.0);
  ReferenceCounted<ValueTexture2D<visual>> eta_back =
      MakeReferenceCounted<ConstantValueTexture2D<visual>>(1.0);
  ReferenceCounted<ValueTexture2D<visual>> roughness =
      MakeReferenceCounted<ConstantValueTexture2D<visual>>(1.0);

  ReferenceCounted<Material> material = MakeGlassMaterial(
      std::move(reflectance), std::move(transmittance), std::move(eta_front),
      std::move(eta_back), roughness, roughness, true);

  const Bxdf* result =
      material->Evaluate(TextureCoordinates{{0.0, 0.0}, std::nullopt},
                         GetSpectralAllocator(), GetBxdfAllocator());
  ASSERT_TRUE(result);
}

}  // namespace
}  // namespace materials
}  // namespace iris
