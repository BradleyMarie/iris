#include "iris/materials/translucent_material.h"

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
using ::testing::_;
using ::testing::Return;

TEST(TranslucentMaterialTest, NullMaterial) {
  ReferenceCounted<ReflectorTexture> reflectance = MakeBlackTexture();
  ReferenceCounted<ReflectorTexture> transmittance = MakeBlackTexture();
  ReferenceCounted<ReflectorTexture> diffuse = MakeBlackTexture();
  ReferenceCounted<ReflectorTexture> specular = MakeBlackTexture();
  ReferenceCounted<FloatTexture> eta_incident = MakeConstantTexture(1.0);
  ReferenceCounted<FloatTexture> eta_transmitted = MakeConstantTexture(1.5);
  ReferenceCounted<FloatTexture> sigma = MakeConstantTexture(1.0);

  EXPECT_FALSE(MakeTranslucentMaterial(ReferenceCounted<ReflectorTexture>(),
                                       ReferenceCounted<ReflectorTexture>(),
                                       ReferenceCounted<ReflectorTexture>(),
                                       ReferenceCounted<ReflectorTexture>(),
                                       eta_incident, eta_transmitted, sigma,
                                       false));
  EXPECT_FALSE(
      MakeTranslucentMaterial(diffuse, ReferenceCounted<ReflectorTexture>(),
                              ReferenceCounted<ReflectorTexture>(),
                              ReferenceCounted<ReflectorTexture>(),
                              eta_incident, eta_transmitted, sigma, false));
  EXPECT_FALSE(
      MakeTranslucentMaterial(ReferenceCounted<ReflectorTexture>(), diffuse,
                              ReferenceCounted<ReflectorTexture>(),
                              ReferenceCounted<ReflectorTexture>(),
                              eta_incident, eta_transmitted, sigma, false));
  EXPECT_FALSE(
      MakeTranslucentMaterial(ReferenceCounted<ReflectorTexture>(),
                              ReferenceCounted<ReflectorTexture>(), diffuse,
                              ReferenceCounted<ReflectorTexture>(),
                              eta_incident, eta_transmitted, sigma, false));
  EXPECT_FALSE(MakeTranslucentMaterial(ReferenceCounted<ReflectorTexture>(),
                                       ReferenceCounted<ReflectorTexture>(),
                                       ReferenceCounted<ReflectorTexture>(),
                                       diffuse, eta_incident, eta_transmitted,
                                       sigma, false));
  EXPECT_FALSE(MakeTranslucentMaterial(
      diffuse, diffuse, ReferenceCounted<ReflectorTexture>(),
      ReferenceCounted<ReflectorTexture>(), eta_incident, eta_transmitted,
      sigma, false));
  EXPECT_FALSE(MakeTranslucentMaterial(
      diffuse, diffuse, ReferenceCounted<ReflectorTexture>(), diffuse,
      ReferenceCounted<FloatTexture>(), eta_transmitted, sigma, false));
  EXPECT_FALSE(MakeTranslucentMaterial(
      diffuse, diffuse, ReferenceCounted<ReflectorTexture>(), diffuse,
      eta_incident, ReferenceCounted<FloatTexture>(), sigma, false));
  EXPECT_FALSE(MakeTranslucentMaterial(ReferenceCounted<ReflectorTexture>(),
                                       ReferenceCounted<ReflectorTexture>(),
                                       diffuse, diffuse, eta_incident,
                                       eta_transmitted, sigma, false));
  EXPECT_TRUE(MakeTranslucentMaterial(
      diffuse, diffuse, diffuse, ReferenceCounted<ReflectorTexture>(),
      eta_incident, eta_transmitted, sigma, false));
  EXPECT_TRUE(MakeTranslucentMaterial(diffuse, diffuse, diffuse, diffuse,
                                      ReferenceCounted<FloatTexture>(),
                                      eta_transmitted, sigma, false));
  EXPECT_TRUE(
      MakeTranslucentMaterial(diffuse, diffuse, diffuse, diffuse, eta_incident,
                              ReferenceCounted<FloatTexture>(), sigma, false));
  EXPECT_TRUE(MakeTranslucentMaterial(
      diffuse, diffuse, ReferenceCounted<ReflectorTexture>(), diffuse,
      eta_incident, eta_transmitted, sigma, false));
  EXPECT_TRUE(MakeTranslucentMaterial(
      diffuse, ReferenceCounted<ReflectorTexture>(), diffuse, diffuse,
      eta_incident, eta_transmitted, sigma, false));
  EXPECT_TRUE(MakeTranslucentMaterial(ReferenceCounted<ReflectorTexture>(),
                                      diffuse, diffuse, diffuse, eta_incident,
                                      eta_transmitted, sigma, false));
}

TEST(TranslucentMaterialTest, NoBxdfs) {
  ReferenceCounted<ReflectorTexture> reflectance = MakeBlackTexture();
  ReferenceCounted<ReflectorTexture> transmittance = MakeBlackTexture();
  ReferenceCounted<ReflectorTexture> diffuse = MakeBlackTexture();
  ReferenceCounted<ReflectorTexture> specular = MakeBlackTexture();
  ReferenceCounted<FloatTexture> eta_incident = MakeConstantTexture(1.0);
  ReferenceCounted<FloatTexture> eta_transmitted = MakeConstantTexture(1.5);
  ReferenceCounted<FloatTexture> sigma = MakeConstantTexture(1.0);

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

  ReferenceCounted<ReflectorTexture> reflectance =
      MakeConstantTexture(reflector);
  ReferenceCounted<ReflectorTexture> transmittance = MakeBlackTexture();
  ReferenceCounted<ReflectorTexture> diffuse = MakeConstantTexture(reflector);
  ReferenceCounted<ReflectorTexture> specular = MakeBlackTexture();
  ReferenceCounted<FloatTexture> eta_incident = MakeConstantTexture(1.0);
  ReferenceCounted<FloatTexture> eta_transmitted = MakeConstantTexture(1.5);
  ReferenceCounted<FloatTexture> sigma = MakeConstantTexture(1.0);

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

  ReferenceCounted<ReflectorTexture> reflectance = MakeBlackTexture();
  ReferenceCounted<ReflectorTexture> transmittance =
      MakeConstantTexture(reflector);
  ReferenceCounted<ReflectorTexture> diffuse = MakeConstantTexture(reflector);
  ReferenceCounted<ReflectorTexture> specular = MakeBlackTexture();
  ReferenceCounted<FloatTexture> eta_incident = MakeConstantTexture(1.0);
  ReferenceCounted<FloatTexture> eta_transmitted = MakeConstantTexture(1.5);
  ReferenceCounted<FloatTexture> sigma = MakeConstantTexture(1.0);

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

  ReferenceCounted<ReflectorTexture> reflectance =
      MakeConstantTexture(reflector);
  ReferenceCounted<ReflectorTexture> transmittance = MakeBlackTexture();
  ReferenceCounted<ReflectorTexture> diffuse = MakeBlackTexture();
  ReferenceCounted<ReflectorTexture> specular = MakeConstantTexture(reflector);
  ReferenceCounted<FloatTexture> eta_incident = MakeConstantTexture(1.0);
  ReferenceCounted<FloatTexture> eta_transmitted = MakeConstantTexture(1.5);
  ReferenceCounted<FloatTexture> sigma = MakeConstantTexture(1.0);

  ReferenceCounted<Material> material =
      MakeTranslucentMaterial(reflectance, transmittance, diffuse, specular,
                              eta_incident, eta_transmitted, sigma, false);

  const Bxdf* result =
      material->Evaluate(TextureCoordinates{{0.0, 0.0}, std::nullopt},
                         GetSpectralAllocator(), GetBxdfAllocator());
  ASSERT_TRUE(result);
}

TEST(TranslucentMaterialTest, SpecularTransmission) {
  ReferenceCounted<Reflector> reflector = MakeReferenceCounted<MockReflector>();

  ReferenceCounted<ReflectorTexture> reflectance = MakeBlackTexture();
  ReferenceCounted<ReflectorTexture> transmittance =
      MakeConstantTexture(reflector);
  ReferenceCounted<ReflectorTexture> diffuse = MakeBlackTexture();
  ReferenceCounted<ReflectorTexture> specular = MakeConstantTexture(reflector);
  ReferenceCounted<FloatTexture> eta_incident = MakeConstantTexture(1.0);
  ReferenceCounted<FloatTexture> eta_transmitted = MakeConstantTexture(1.5);
  ReferenceCounted<FloatTexture> sigma = MakeConstantTexture(1.0);

  ReferenceCounted<Material> material =
      MakeTranslucentMaterial(reflectance, transmittance, diffuse, specular,
                              eta_incident, eta_transmitted, sigma, false);

  const Bxdf* result =
      material->Evaluate(TextureCoordinates{{0.0, 0.0}, std::nullopt},
                         GetSpectralAllocator(), GetBxdfAllocator());
  ASSERT_TRUE(result);
}

TEST(TranslucentMaterialTest, All) {
  ReferenceCounted<Reflector> reflector = MakeReferenceCounted<MockReflector>();

  ReferenceCounted<ReflectorTexture> reflectance =
      MakeConstantTexture(reflector);
  ReferenceCounted<ReflectorTexture> transmittance =
      MakeConstantTexture(reflector);
  ReferenceCounted<ReflectorTexture> diffuse = MakeConstantTexture(reflector);
  ReferenceCounted<ReflectorTexture> specular = MakeConstantTexture(reflector);
  ReferenceCounted<FloatTexture> eta_incident = MakeConstantTexture(1.0);
  ReferenceCounted<FloatTexture> eta_transmitted = MakeConstantTexture(1.5);
  ReferenceCounted<FloatTexture> sigma = MakeConstantTexture(1.0);

  ReferenceCounted<Material> material =
      MakeTranslucentMaterial(reflectance, transmittance, diffuse, specular,
                              eta_incident, eta_transmitted, sigma, true);

  const Bxdf* result =
      material->Evaluate(TextureCoordinates{{0.0, 0.0}, std::nullopt},
                         GetSpectralAllocator(), GetBxdfAllocator());
  ASSERT_TRUE(result);
}

}  // namespace
}  // namespace materials
}  // namespace iris
