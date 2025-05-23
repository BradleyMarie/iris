#include "iris/materials/matte_material.h"

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

TEST(MatteMaterialTest, NullMaterial) {
  EXPECT_FALSE(MakeMatteMaterial(
      ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>(),
      MakeReferenceCounted<ConstantValueTexture2D<visual>>(1.0)));
}

TEST(MatteMaterialTest, EvaluateEmpty) {
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> reflectance =
      MakeReferenceCounted<
          ConstantPointerTexture2D<Reflector, SpectralAllocator>>(
          ReferenceCounted<Reflector>());
  ReferenceCounted<ValueTexture2D<visual>> sigma =
      MakeReferenceCounted<ConstantValueTexture2D<visual>>(1.0);
  ReferenceCounted<Material> material =
      MakeMatteMaterial(std::move(reflectance), std::move(sigma));

  ASSERT_FALSE(material->Evaluate(TextureCoordinates{{0.0, 0.0}, std::nullopt},
                                  GetSpectralAllocator(), GetBxdfAllocator()));
}

TEST(MatteMaterialTest, EvaluateLambertian) {
  ReferenceCounted<Reflector> reflector = MakeReferenceCounted<MockReflector>();

  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> reflectance =
      MakeReferenceCounted<
          ConstantPointerTexture2D<Reflector, SpectralAllocator>>(reflector);
  ReferenceCounted<ValueTexture2D<visual>> sigma =
      MakeReferenceCounted<ConstantValueTexture2D<visual>>(0.0);
  ReferenceCounted<Material> material =
      MakeMatteMaterial(std::move(reflectance), std::move(sigma));

  const Bxdf* result =
      material->Evaluate(TextureCoordinates{{0.0, 0.0}, std::nullopt},
                         GetSpectralAllocator(), GetBxdfAllocator());
  ASSERT_TRUE(result);
}

TEST(MatteMaterialTest, EvaluateOrenNayar) {
  ReferenceCounted<Reflector> reflector = MakeReferenceCounted<MockReflector>();

  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> reflectance =
      MakeReferenceCounted<
          ConstantPointerTexture2D<Reflector, SpectralAllocator>>(reflector);
  ReferenceCounted<ValueTexture2D<visual>> sigma =
      MakeReferenceCounted<ConstantValueTexture2D<visual>>(1.0);
  ReferenceCounted<Material> material =
      MakeMatteMaterial(std::move(reflectance), std::move(sigma));

  const Bxdf* result =
      material->Evaluate(TextureCoordinates{{0.0, 0.0}, std::nullopt},
                         GetSpectralAllocator(), GetBxdfAllocator());
  ASSERT_TRUE(result);
}

}  // namespace
}  // namespace materials
}  // namespace iris
