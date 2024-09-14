#include "iris/materials/mirror_material.h"

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
using ::iris::textures::PointerTexture2D;
using ::iris::textures::ValueTexture2D;

TEST(MirrorMaterialTest, EvaluateEmpty) {
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> reflectance =
      MakeReferenceCounted<
          ConstantPointerTexture2D<Reflector, SpectralAllocator>>(
          ReferenceCounted<Reflector>());
  MirrorMaterial material(std::move(reflectance));

  ASSERT_FALSE(material.Evaluate(TextureCoordinates{{0.0, 0.0}, std::nullopt},
                                 GetSpectralAllocator(), GetBxdfAllocator()));
}

TEST(MirrorMaterialTest, Evaluate) {
  ReferenceCounted<Reflector> reflector = MakeReferenceCounted<MockReflector>();
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> reflectance =
      MakeReferenceCounted<
          ConstantPointerTexture2D<Reflector, SpectralAllocator>>(reflector);
  MirrorMaterial material(std::move(reflectance));

  const Bxdf* result =
      material.Evaluate(TextureCoordinates{{0.0, 0.0}, std::nullopt},
                        GetSpectralAllocator(), GetBxdfAllocator());
  ASSERT_TRUE(result);

  const Reflector* returned_reflector =
      result->Reflectance(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0),
                          Bxdf::Hemisphere::BRDF, GetSpectralAllocator());
  EXPECT_EQ(reflector.Get(), returned_reflector);
}

}  // namespace
}  // namespace materials
}  // namespace iris