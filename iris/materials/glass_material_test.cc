#include "iris/materials/glass_material.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/point.h"
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

TEST(GlassMaterialTest, NullMaterial) {
  ReferenceCounted<ReflectorTexture> reflectance =
      MakeConstantTexture(MakeReferenceCounted<MockReflector>());
  ReferenceCounted<FloatTexture> eta_front = MakeConstantTexture(1.0);
  ReferenceCounted<FloatTexture> eta_back = MakeConstantTexture(1.0);

  EXPECT_FALSE(MakeGlassMaterial(ReferenceCounted<ReflectorTexture>(),
                                 ReferenceCounted<ReflectorTexture>(),
                                 eta_front, eta_back,
                                 ReferenceCounted<FloatTexture>(),
                                 ReferenceCounted<FloatTexture>(), true));

  EXPECT_FALSE(MakeGlassMaterial(ReferenceCounted<ReflectorTexture>(),
                                 reflectance, ReferenceCounted<FloatTexture>(),
                                 eta_back, ReferenceCounted<FloatTexture>(),
                                 ReferenceCounted<FloatTexture>(), true));

  EXPECT_FALSE(MakeGlassMaterial(
      ReferenceCounted<ReflectorTexture>(), reflectance, eta_front,
      ReferenceCounted<FloatTexture>(), ReferenceCounted<FloatTexture>(),
      ReferenceCounted<FloatTexture>(), true));

  EXPECT_TRUE(MakeGlassMaterial(reflectance,
                                ReferenceCounted<ReflectorTexture>(), eta_front,
                                eta_back, ReferenceCounted<FloatTexture>(),
                                ReferenceCounted<FloatTexture>(), true));

  EXPECT_TRUE(MakeGlassMaterial(ReferenceCounted<ReflectorTexture>(),
                                reflectance, eta_front, eta_back,
                                ReferenceCounted<FloatTexture>(),
                                ReferenceCounted<FloatTexture>(), true));
}

TEST(GlassMaterialTest, EvaluateEmpty) {
  ReferenceCounted<ReflectorTexture> reflectance = MakeBlackTexture();
  ReferenceCounted<ReflectorTexture> transmittance = MakeBlackTexture();
  ReferenceCounted<FloatTexture> eta_front = MakeConstantTexture(1.0);
  ReferenceCounted<FloatTexture> eta_back = MakeConstantTexture(1.0);

  ReferenceCounted<Material> material = MakeGlassMaterial(
      std::move(reflectance), std::move(transmittance), std::move(eta_front),
      std::move(eta_back), ReferenceCounted<FloatTexture>(),
      ReferenceCounted<FloatTexture>(), true);

  ASSERT_FALSE(material->Evaluate(
      TextureCoordinates{Point(0.0, 0.0, 0.0), {}, {0.0, 0.0}, std::nullopt},
      GetSpectralAllocator(), GetBxdfAllocator()));
}

TEST(GlassMaterialTest, EvaluateSpecular) {
  ReferenceCounted<Reflector> reflector = MakeReferenceCounted<MockReflector>();
  ReferenceCounted<Reflector> transmitter =
      MakeReferenceCounted<MockReflector>();

  ReferenceCounted<ReflectorTexture> reflectance =
      MakeConstantTexture(reflector);
  ReferenceCounted<ReflectorTexture> transmittance =
      MakeConstantTexture(transmitter);
  ReferenceCounted<FloatTexture> eta_front = MakeConstantTexture(1.0);
  ReferenceCounted<FloatTexture> eta_back = MakeConstantTexture(1.0);

  ReferenceCounted<Material> material = MakeGlassMaterial(
      std::move(reflectance), std::move(transmittance), std::move(eta_front),
      std::move(eta_back), ReferenceCounted<FloatTexture>(),
      ReferenceCounted<FloatTexture>(), true);

  const Bxdf* result = material->Evaluate(
      TextureCoordinates{Point(0.0, 0.0, 0.0), {}, {0.0, 0.0}, std::nullopt},
      GetSpectralAllocator(), GetBxdfAllocator());
  ASSERT_TRUE(result);
}

TEST(GlassMaterialTest, EvaluateBrdf) {
  ReferenceCounted<Reflector> reflector = MakeReferenceCounted<MockReflector>();

  ReferenceCounted<ReflectorTexture> reflectance =
      MakeConstantTexture(reflector);
  ReferenceCounted<ReflectorTexture> transmittance =
      MakeConstantTexture(MakeReferenceCounted<MockReflector>());
  ReferenceCounted<FloatTexture> eta_front = MakeConstantTexture(1.0);
  ReferenceCounted<FloatTexture> eta_back = MakeConstantTexture(1.0);
  ReferenceCounted<FloatTexture> roughness = MakeConstantTexture(1.0);

  ReferenceCounted<Material> material = MakeGlassMaterial(
      std::move(reflectance), std::move(transmittance), std::move(eta_front),
      std::move(eta_back), roughness, roughness, true);

  const Bxdf* result = material->Evaluate(
      TextureCoordinates{Point(0.0, 0.0, 0.0), {}, {0.0, 0.0}, std::nullopt},
      GetSpectralAllocator(), GetBxdfAllocator());
  ASSERT_TRUE(result);
}

TEST(GlassMaterialTest, EvaluateBtdf) {
  ReferenceCounted<Reflector> transmitter =
      MakeReferenceCounted<MockReflector>();

  ReferenceCounted<ReflectorTexture> reflectance =
      MakeConstantTexture(MakeReferenceCounted<MockReflector>());
  ReferenceCounted<ReflectorTexture> transmittance =
      MakeConstantTexture(transmitter);
  ReferenceCounted<FloatTexture> eta_front = MakeConstantTexture(1.0);
  ReferenceCounted<FloatTexture> eta_back = MakeConstantTexture(1.0);
  ReferenceCounted<FloatTexture> roughness = MakeConstantTexture(1.0);

  ReferenceCounted<Material> material = MakeGlassMaterial(
      std::move(reflectance), std::move(transmittance), std::move(eta_front),
      std::move(eta_back), roughness, roughness, true);

  const Bxdf* result = material->Evaluate(
      TextureCoordinates{Point(0.0, 0.0, 0.0), {}, {0.0, 0.0}, std::nullopt},
      GetSpectralAllocator(), GetBxdfAllocator());
  ASSERT_TRUE(result);
}

}  // namespace
}  // namespace materials
}  // namespace iris
