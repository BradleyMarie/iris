#include "iris/materials/matte_material.h"

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

TEST(MatteMaterialTest, NullMaterial) {
  EXPECT_FALSE(MakeMatteMaterial(ReferenceCounted<ReflectorTexture>(),
                                 MakeConstantTexture(1.0)));
}

TEST(MatteMaterialTest, EvaluateEmpty) {
  ReferenceCounted<ReflectorTexture> reflectance = MakeBlackTexture();
  ReferenceCounted<FloatTexture> sigma = MakeConstantTexture(1.0);
  ReferenceCounted<Material> material =
      MakeMatteMaterial(std::move(reflectance), std::move(sigma));

  ASSERT_FALSE(material->Evaluate(TextureCoordinates{{0.0, 0.0}, std::nullopt},
                                  GetSpectralAllocator(), GetBxdfAllocator()));
}

TEST(MatteMaterialTest, EvaluateLambertian) {
  ReferenceCounted<Reflector> reflector = MakeReferenceCounted<MockReflector>();

  ReferenceCounted<ReflectorTexture> reflectance =
      MakeConstantTexture(reflector);
  ReferenceCounted<FloatTexture> sigma = MakeConstantTexture(0.0);
  ReferenceCounted<Material> material =
      MakeMatteMaterial(std::move(reflectance), std::move(sigma));

  const Bxdf* result =
      material->Evaluate(TextureCoordinates{{0.0, 0.0}, std::nullopt},
                         GetSpectralAllocator(), GetBxdfAllocator());
  ASSERT_TRUE(result);
}

TEST(MatteMaterialTest, EvaluateOrenNayar) {
  ReferenceCounted<Reflector> reflector = MakeReferenceCounted<MockReflector>();

  ReferenceCounted<ReflectorTexture> reflectance =
      MakeConstantTexture(reflector);
  ReferenceCounted<FloatTexture> sigma = MakeConstantTexture(1.0);
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
