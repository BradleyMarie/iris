#include "iris/materials/plastic_material.h"

#include <numbers>

#include "googlemock/include/gmock/gmock.h"
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
using ::testing::_;
using ::testing::Return;

TEST(PlasticMaterialTest, NullMaterial) {
  ReferenceCounted<ReflectorTexture> reflectance = MakeBlackTexture();
  ReferenceCounted<FloatTexture> eta_incident = MakeConstantTexture(1.0);
  ReferenceCounted<FloatTexture> eta_transmitted = MakeConstantTexture(1.5);
  ReferenceCounted<FloatTexture> sigma = MakeConstantTexture(1.0);

  EXPECT_FALSE(MakePlasticMaterial(ReferenceCounted<ReflectorTexture>(),
                                   ReferenceCounted<ReflectorTexture>(),
                                   eta_incident, eta_transmitted,
                                   std::move(sigma), false));
  EXPECT_FALSE(MakePlasticMaterial(ReferenceCounted<ReflectorTexture>(),
                                   reflectance,
                                   ReferenceCounted<FloatTexture>(),
                                   eta_transmitted, std::move(sigma), false));
  EXPECT_FALSE(MakePlasticMaterial(
      ReferenceCounted<ReflectorTexture>(), reflectance, eta_incident,
      ReferenceCounted<FloatTexture>(), std::move(sigma), false));
  EXPECT_TRUE(MakePlasticMaterial(
      reflectance, ReferenceCounted<ReflectorTexture>(), eta_incident,
      eta_transmitted, std::move(sigma), false));
  EXPECT_TRUE(MakePlasticMaterial(reflectance, reflectance,
                                  ReferenceCounted<FloatTexture>(),
                                  eta_transmitted, std::move(sigma), false));
  EXPECT_TRUE(MakePlasticMaterial(reflectance, reflectance, eta_incident,
                                  ReferenceCounted<FloatTexture>(),
                                  std::move(sigma), false));
  EXPECT_TRUE(MakePlasticMaterial(ReferenceCounted<ReflectorTexture>(),
                                  reflectance, eta_incident, eta_transmitted,
                                  std::move(sigma), false));
}

TEST(PlasticMaterialTest, NoBxdfs) {
  ReferenceCounted<ReflectorTexture> reflectance = MakeBlackTexture();
  ReferenceCounted<FloatTexture> eta_incident = MakeConstantTexture(1.5);
  ReferenceCounted<FloatTexture> eta_transmitted = MakeConstantTexture(1.0);
  ReferenceCounted<FloatTexture> sigma = MakeConstantTexture(1.0);

  ReferenceCounted<Material> material =
      MakePlasticMaterial(reflectance, reflectance, eta_incident,
                          eta_transmitted, std::move(sigma), false);

  EXPECT_FALSE(material->Evaluate(
      TextureCoordinates{Point(0.0, 0.0, 0.0), {}, {0.0, 0.0}, std::nullopt},
      GetSpectralAllocator(), GetBxdfAllocator()));
}

TEST(PlasticMaterialTest, LambertianOnly) {
  ReferenceCounted<MockReflector> reflector =
      MakeReferenceCounted<MockReflector>();
  EXPECT_CALL(*reflector, Reflectance(_)).Times(1).WillOnce(Return(1.0));

  ReferenceCounted<ReflectorTexture> reflectance =
      MakeConstantTexture(reflector);
  ReferenceCounted<ReflectorTexture> specular = MakeBlackTexture();
  ReferenceCounted<FloatTexture> eta_incident = MakeConstantTexture(1.5);
  ReferenceCounted<FloatTexture> eta_transmitted = MakeConstantTexture(1.0);
  ReferenceCounted<FloatTexture> sigma = MakeConstantTexture(1.0);

  ReferenceCounted<Material> material =
      MakePlasticMaterial(reflectance, specular, eta_incident, eta_transmitted,
                          std::move(sigma), false);

  const Bxdf* result = material->Evaluate(
      TextureCoordinates{Point(0.0, 0.0, 0.0), {}, {0.0, 0.0}, std::nullopt},
      GetSpectralAllocator(), GetBxdfAllocator());
  ASSERT_TRUE(result);

  const Reflector* returned_reflector = result->ReflectanceDiffuse(
      Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0), Bxdf::Hemisphere::BRDF,
      GetSpectralAllocator());
  ASSERT_TRUE(returned_reflector);
  EXPECT_NEAR(std::numbers::inv_pi, returned_reflector->Reflectance(1.0),
              0.0001);
}

TEST(PlasticMaterialTest, SpecularOnly) {
  ReferenceCounted<MockReflector> reflector =
      MakeReferenceCounted<MockReflector>();
  EXPECT_CALL(*reflector, Reflectance(_)).Times(1).WillOnce(Return(1.0));

  ReferenceCounted<ReflectorTexture> reflectance = MakeBlackTexture();
  ReferenceCounted<ReflectorTexture> specular = MakeConstantTexture(reflector);
  ReferenceCounted<FloatTexture> eta_incident = MakeConstantTexture(1.5);
  ReferenceCounted<FloatTexture> eta_transmitted = MakeConstantTexture(1.0);
  ReferenceCounted<FloatTexture> sigma = MakeConstantTexture(1.0);

  ReferenceCounted<Material> material =
      MakePlasticMaterial(reflectance, specular, eta_incident, eta_transmitted,
                          std::move(sigma), false);

  const Bxdf* result = material->Evaluate(
      TextureCoordinates{Point(0.0, 0.0, 0.0), {}, {0.0, 0.0}, std::nullopt},
      GetSpectralAllocator(), GetBxdfAllocator());
  ASSERT_TRUE(result);

  const Reflector* returned_reflector = result->ReflectanceDiffuse(
      Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0), Bxdf::Hemisphere::BRDF,
      GetSpectralAllocator());
  ASSERT_TRUE(returned_reflector);
  EXPECT_NEAR(0.0031830, returned_reflector->Reflectance(1.0), 0.0001);
}

TEST(PlasticMaterialTest, Remap) {
  ReferenceCounted<MockReflector> reflector =
      MakeReferenceCounted<MockReflector>();
  EXPECT_CALL(*reflector, Reflectance(_)).Times(1).WillOnce(Return(1.0));

  ReferenceCounted<ReflectorTexture> reflectance = MakeBlackTexture();
  ReferenceCounted<ReflectorTexture> specular = MakeConstantTexture(reflector);
  ReferenceCounted<FloatTexture> eta_incident = MakeConstantTexture(1.5);
  ReferenceCounted<FloatTexture> eta_transmitted = MakeConstantTexture(1.0);
  ReferenceCounted<FloatTexture> sigma = MakeConstantTexture(1.0);

  ReferenceCounted<Material> material =
      MakePlasticMaterial(reflectance, specular, eta_incident, eta_transmitted,
                          std::move(sigma), true);

  const Bxdf* result = material->Evaluate(
      TextureCoordinates{Point(0.0, 0.0, 0.0), {}, {0.0, 0.0}, std::nullopt},
      GetSpectralAllocator(), GetBxdfAllocator());
  ASSERT_TRUE(result);

  const Reflector* returned_reflector = result->ReflectanceDiffuse(
      Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0), Bxdf::Hemisphere::BRDF,
      GetSpectralAllocator());
  ASSERT_TRUE(returned_reflector);
  EXPECT_NEAR(0.0012107628, returned_reflector->Reflectance(1.0), 0.0001);
}

TEST(PlasticMaterialTest, Both) {
  ReferenceCounted<MockReflector> reflector =
      MakeReferenceCounted<MockReflector>();
  EXPECT_CALL(*reflector, Reflectance(_)).Times(2).WillRepeatedly(Return(1.0));

  ReferenceCounted<ReflectorTexture> reflectance =
      MakeConstantTexture(reflector);
  ReferenceCounted<ReflectorTexture> specular = MakeConstantTexture(reflector);
  ReferenceCounted<FloatTexture> eta_incident = MakeConstantTexture(1.5);
  ReferenceCounted<FloatTexture> eta_transmitted = MakeConstantTexture(1.0);
  ReferenceCounted<FloatTexture> sigma = MakeConstantTexture(1.0);

  ReferenceCounted<Material> material =
      MakePlasticMaterial(reflectance, specular, eta_incident, eta_transmitted,
                          std::move(sigma), false);

  const Bxdf* result = material->Evaluate(
      TextureCoordinates{Point(0.0, 0.0, 0.0), {}, {0.0, 0.0}, std::nullopt},
      GetSpectralAllocator(), GetBxdfAllocator());
  ASSERT_TRUE(result);

  const Reflector* returned_reflector = result->ReflectanceDiffuse(
      Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0), Bxdf::Hemisphere::BRDF,
      GetSpectralAllocator());
  ASSERT_TRUE(returned_reflector);
  EXPECT_NEAR(0.321492, returned_reflector->Reflectance(1.0), 0.01);
}

}  // namespace
}  // namespace materials
}  // namespace iris
