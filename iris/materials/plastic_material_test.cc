#include "iris/materials/plastic_material.h"

#include <numbers>

#include "googlemock/include/gmock/gmock.h"
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
using ::testing::_;
using ::testing::Return;

TEST(PlasticMaterialTest, NoBxdfs) {
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> reflectance =
      MakeReferenceCounted<
          ConstantPointerTexture2D<Reflector, SpectralAllocator>>(
          ReferenceCounted<Reflector>());
  ReferenceCounted<ValueTexture2D<visual>> eta_incident =
      MakeReferenceCounted<ConstantValueTexture2D<visual>>(1.5);
  ReferenceCounted<ValueTexture2D<visual>> eta_transmitted =
      MakeReferenceCounted<ConstantValueTexture2D<visual>>(1.0);
  ReferenceCounted<ValueTexture2D<visual>> sigma =
      MakeReferenceCounted<ConstantValueTexture2D<visual>>(1.0);
  PlasticMaterial material(reflectance, reflectance, eta_incident,
                           eta_transmitted, std::move(sigma), false);

  EXPECT_FALSE(material.Evaluate(TextureCoordinates{{0.0, 0.0}, std::nullopt},
                                 GetSpectralAllocator(), GetBxdfAllocator()));
}

TEST(PlasticMaterialTest, LambertianOnly) {
  ReferenceCounted<MockReflector> reflector =
      MakeReferenceCounted<MockReflector>();
  EXPECT_CALL(*reflector, Reflectance(_)).Times(1).WillOnce(Return(1.0));

  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> reflectance =
      MakeReferenceCounted<
          ConstantPointerTexture2D<Reflector, SpectralAllocator>>(reflector);
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> specular =
      MakeReferenceCounted<
          ConstantPointerTexture2D<Reflector, SpectralAllocator>>(
          ReferenceCounted<Reflector>());
  ReferenceCounted<ValueTexture2D<visual>> eta_incident =
      MakeReferenceCounted<ConstantValueTexture2D<visual>>(1.5);
  ReferenceCounted<ValueTexture2D<visual>> eta_transmitted =
      MakeReferenceCounted<ConstantValueTexture2D<visual>>(1.0);
  ReferenceCounted<ValueTexture2D<visual>> sigma =
      MakeReferenceCounted<ConstantValueTexture2D<visual>>(1.0);
  PlasticMaterial material(reflectance, specular, eta_incident, eta_transmitted,
                           std::move(sigma), false);

  const Bxdf* result =
      material.Evaluate(TextureCoordinates{{0.0, 0.0}, std::nullopt},
                        GetSpectralAllocator(), GetBxdfAllocator());
  ASSERT_TRUE(result);

  const Reflector* returned_reflector =
      result->Reflectance(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0),
                          Bxdf::Hemisphere::BRDF, GetSpectralAllocator());
  ASSERT_TRUE(returned_reflector);
  EXPECT_NEAR(std::numbers::inv_pi, returned_reflector->Reflectance(1.0),
              0.0001);
}

TEST(PlasticMaterialTest, SpecularOnly) {
  ReferenceCounted<MockReflector> reflector =
      MakeReferenceCounted<MockReflector>();
  EXPECT_CALL(*reflector, Reflectance(_)).Times(1).WillOnce(Return(1.0));

  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> reflectance =
      MakeReferenceCounted<
          ConstantPointerTexture2D<Reflector, SpectralAllocator>>(
          ReferenceCounted<Reflector>());
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> specular =
      MakeReferenceCounted<
          ConstantPointerTexture2D<Reflector, SpectralAllocator>>(reflector);
  ReferenceCounted<ValueTexture2D<visual>> eta_incident =
      MakeReferenceCounted<ConstantValueTexture2D<visual>>(1.5);
  ReferenceCounted<ValueTexture2D<visual>> eta_transmitted =
      MakeReferenceCounted<ConstantValueTexture2D<visual>>(1.0);
  ReferenceCounted<ValueTexture2D<visual>> sigma =
      MakeReferenceCounted<ConstantValueTexture2D<visual>>(1.0);
  PlasticMaterial material(reflectance, specular, eta_incident, eta_transmitted,
                           std::move(sigma), false);

  const Bxdf* result =
      material.Evaluate(TextureCoordinates{{0.0, 0.0}, std::nullopt},
                        GetSpectralAllocator(), GetBxdfAllocator());
  ASSERT_TRUE(result);

  const Reflector* returned_reflector =
      result->Reflectance(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0),
                          Bxdf::Hemisphere::BRDF, GetSpectralAllocator());
  ASSERT_TRUE(returned_reflector);
  EXPECT_NEAR(0.0031830, returned_reflector->Reflectance(1.0), 0.0001);
}

TEST(PlasticMaterialTest, Remap) {
  ReferenceCounted<MockReflector> reflector =
      MakeReferenceCounted<MockReflector>();
  EXPECT_CALL(*reflector, Reflectance(_)).Times(1).WillOnce(Return(1.0));

  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> reflectance =
      MakeReferenceCounted<
          ConstantPointerTexture2D<Reflector, SpectralAllocator>>(
          ReferenceCounted<Reflector>());
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> specular =
      MakeReferenceCounted<
          ConstantPointerTexture2D<Reflector, SpectralAllocator>>(reflector);
  ReferenceCounted<ValueTexture2D<visual>> eta_incident =
      MakeReferenceCounted<ConstantValueTexture2D<visual>>(1.5);
  ReferenceCounted<ValueTexture2D<visual>> eta_transmitted =
      MakeReferenceCounted<ConstantValueTexture2D<visual>>(1.0);
  ReferenceCounted<ValueTexture2D<visual>> sigma =
      MakeReferenceCounted<ConstantValueTexture2D<visual>>(1.0);
  PlasticMaterial material(reflectance, specular, eta_incident, eta_transmitted,
                           std::move(sigma), true);

  const Bxdf* result =
      material.Evaluate(TextureCoordinates{{0.0, 0.0}, std::nullopt},
                        GetSpectralAllocator(), GetBxdfAllocator());
  ASSERT_TRUE(result);

  const Reflector* returned_reflector =
      result->Reflectance(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0),
                          Bxdf::Hemisphere::BRDF, GetSpectralAllocator());
  ASSERT_TRUE(returned_reflector);
  EXPECT_NEAR(0.0012107628, returned_reflector->Reflectance(1.0), 0.0001);
}

TEST(PlasticMaterialTest, Both) {
  ReferenceCounted<MockReflector> reflector =
      MakeReferenceCounted<MockReflector>();
  EXPECT_CALL(*reflector, Reflectance(_)).Times(2).WillRepeatedly(Return(1.0));

  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> reflectance =
      MakeReferenceCounted<
          ConstantPointerTexture2D<Reflector, SpectralAllocator>>(reflector);
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> specular =
      MakeReferenceCounted<
          ConstantPointerTexture2D<Reflector, SpectralAllocator>>(reflector);
  ReferenceCounted<ValueTexture2D<visual>> eta_incident =
      MakeReferenceCounted<ConstantValueTexture2D<visual>>(1.5);
  ReferenceCounted<ValueTexture2D<visual>> eta_transmitted =
      MakeReferenceCounted<ConstantValueTexture2D<visual>>(1.0);
  ReferenceCounted<ValueTexture2D<visual>> sigma =
      MakeReferenceCounted<ConstantValueTexture2D<visual>>(1.0);
  PlasticMaterial material(reflectance, specular, eta_incident, eta_transmitted,
                           std::move(sigma), false);

  const Bxdf* result =
      material.Evaluate(TextureCoordinates{{0.0, 0.0}, std::nullopt},
                        GetSpectralAllocator(), GetBxdfAllocator());
  ASSERT_TRUE(result);

  const Reflector* returned_reflector =
      result->Reflectance(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0),
                          Bxdf::Hemisphere::BRDF, GetSpectralAllocator());
  ASSERT_TRUE(returned_reflector);
  EXPECT_NEAR(0.321492, returned_reflector->Reflectance(1.0), 0.01);
}

}  // namespace
}  // namespace materials
}  // namespace iris