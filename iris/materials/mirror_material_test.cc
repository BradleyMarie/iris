#include "iris/materials/mirror_material.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/reflectors/mock_reflector.h"
#include "iris/testing/bxdf_allocator.h"
#include "iris/testing/spectral_allocator.h"
#include "iris/textures/constant_texture.h"
#include "iris/textures/reflector_texture.h"
#include "iris/textures/test_util.h"

namespace iris {
namespace materials {
namespace {

using ::iris::reflectors::MockReflector;
using ::iris::testing::GetBxdfAllocator;
using ::iris::testing::GetSpectralAllocator;
using ::iris::textures::MakeBlackTexture;
using ::iris::textures::MakeConstantTexture;
using ::iris::textures::ReflectorTexture;

TEST(MirrorMaterialTest, NullMaterial) {
  EXPECT_FALSE(MakeMirrorMaterial(ReferenceCounted<ReflectorTexture>()));
}

TEST(MirrorMaterialTest, EvaluateEmpty) {
  ReferenceCounted<ReflectorTexture> reflectance = MakeBlackTexture();
  ReferenceCounted<Material> material =
      MakeMirrorMaterial(std::move(reflectance));

  ASSERT_FALSE(material->Evaluate(TextureCoordinates{{0.0, 0.0}, std::nullopt},
                                  GetSpectralAllocator(), GetBxdfAllocator()));
}

TEST(MirrorMaterialTest, Evaluate) {
  ReferenceCounted<Reflector> reflector = MakeReferenceCounted<MockReflector>();
  ReferenceCounted<ReflectorTexture> reflectance =
      MakeConstantTexture(reflector);
  ReferenceCounted<Material> material =
      MakeMirrorMaterial(std::move(reflectance));

  const Bxdf* result =
      material->Evaluate(TextureCoordinates{{0.0, 0.0}, std::nullopt},
                         GetSpectralAllocator(), GetBxdfAllocator());
  ASSERT_TRUE(result);
}

}  // namespace
}  // namespace materials
}  // namespace iris
