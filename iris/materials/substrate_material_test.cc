#include "iris/materials/substrate_material.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/reflectors/mock_reflector.h"
#include "iris/testing/bxdf_allocator.h"
#include "iris/testing/spectral_allocator.h"
#include "iris/textures/constant_texture.h"
#include "iris/textures/float_texture.h"
#include "iris/textures/reflector_texture.h"
#include "iris/textures/test_util.h"

namespace iris {
namespace materials {
namespace {

using ::iris::reflectors::MockReflector;
using ::iris::testing::GetBxdfAllocator;
using ::iris::testing::GetSpectralAllocator;
using ::iris::textures::FloatTexture;
using ::iris::textures::MakeBlackTexture;
using ::iris::textures::MakeConstantTexture;
using ::iris::textures::ReflectorTexture;

TEST(SubstrateMaterialTest, NullMaterial) {
  ReferenceCounted<ReflectorTexture> reflectance = MakeBlackTexture();
  ReferenceCounted<FloatTexture> roughness = MakeConstantTexture(1.0);

  EXPECT_FALSE(MakeSubstrateMaterial(ReferenceCounted<ReflectorTexture>(),
                                     ReferenceCounted<ReflectorTexture>(),
                                     roughness, roughness, true));
  EXPECT_TRUE(MakeSubstrateMaterial(reflectance,
                                    ReferenceCounted<ReflectorTexture>(),
                                    roughness, roughness, true));
  EXPECT_TRUE(MakeSubstrateMaterial(ReferenceCounted<ReflectorTexture>(),
                                    reflectance, roughness, roughness, true));
}

TEST(SubstrateMaterialTest, EvaluateEmpty) {
  ReferenceCounted<ReflectorTexture> reflectance = MakeBlackTexture();
  ReferenceCounted<FloatTexture> roughness = MakeConstantTexture(1.0);

  ReferenceCounted<Material> material = MakeSubstrateMaterial(
      reflectance, reflectance, roughness, roughness, true);

  ASSERT_FALSE(material->Evaluate(TextureCoordinates{{0.0, 0.0}, std::nullopt},
                                  GetSpectralAllocator(), GetBxdfAllocator()));
}

TEST(SubstrateMaterialTest, Evaluate) {
  ReferenceCounted<Reflector> reflector = MakeReferenceCounted<MockReflector>();

  ReferenceCounted<ReflectorTexture> reflectance =
      MakeConstantTexture(reflector);
  ReferenceCounted<FloatTexture> roughness = MakeConstantTexture(0.0);

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
