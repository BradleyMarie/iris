#define _USE_MATH_DEFINES
#include "iris/materials/plastic_material.h"

#include <cmath>
#include <memory>

#include "googlemock/include/gmock/gmock.h"
#include "googletest/include/gtest/gtest.h"
#include "iris/reflectors/mock_reflector.h"
#include "iris/testing/bxdf_allocator.h"
#include "iris/testing/spectral_allocator.h"
#include "iris/textures/constant_texture.h"

TEST(PlasticMaterialTest, NoBxdfs) {
  auto reflectance =
      iris::MakeReferenceCounted<iris::textures::ConstantPointerTexture2D<
          iris::Reflector, iris::SpectralAllocator>>(
          iris::ReferenceCounted<iris::Reflector>());
  auto eta_incident = iris::MakeReferenceCounted<
      iris::textures::ConstantValueTexture2D<iris::visual>>(
      static_cast<iris::visual>(1.5));
  auto eta_transmitted = iris::MakeReferenceCounted<
      iris::textures::ConstantValueTexture2D<iris::visual>>(
      static_cast<iris::visual>(1.0));
  auto sigma = iris::MakeReferenceCounted<
      iris::textures::ConstantValueTexture2D<iris::visual>>(
      static_cast<iris::visual>(1.0));
  iris::materials::PlasticMaterial material(reflectance, reflectance,
                                            eta_incident, eta_transmitted,
                                            std::move(sigma), false);

  auto* result = material.Evaluate(
      iris::TextureCoordinates{{0.0, 0.0}, std::nullopt},
      iris::testing::GetSpectralAllocator(), iris::testing::GetBxdfAllocator());
  ASSERT_FALSE(result);
}

TEST(PlasticMaterialTest, LambertianOnly) {
  auto reflector =
      iris::MakeReferenceCounted<iris::reflectors::MockReflector>();
  EXPECT_CALL(*reflector, Reflectance(testing::_))
      .Times(1)
      .WillOnce(testing::Return(1.0));

  auto reflectance =
      iris::MakeReferenceCounted<iris::textures::ConstantPointerTexture2D<
          iris::Reflector, iris::SpectralAllocator>>(reflector);
  auto specular =
      iris::MakeReferenceCounted<iris::textures::ConstantPointerTexture2D<
          iris::Reflector, iris::SpectralAllocator>>(
          iris::ReferenceCounted<iris::Reflector>());
  auto eta_incident = iris::MakeReferenceCounted<
      iris::textures::ConstantValueTexture2D<iris::visual>>(
      static_cast<iris::visual>(1.5));
  auto eta_transmitted = iris::MakeReferenceCounted<
      iris::textures::ConstantValueTexture2D<iris::visual>>(
      static_cast<iris::visual>(1.0));
  auto sigma = iris::MakeReferenceCounted<
      iris::textures::ConstantValueTexture2D<iris::visual>>(
      static_cast<iris::visual>(1.0));
  iris::materials::PlasticMaterial material(reflectance, specular, eta_incident,
                                            eta_transmitted, std::move(sigma),
                                            false);

  auto* result = material.Evaluate(
      iris::TextureCoordinates{{0.0, 0.0}, std::nullopt},
      iris::testing::GetSpectralAllocator(), iris::testing::GetBxdfAllocator());
  ASSERT_TRUE(result);

  auto* returned_reflector = result->Reflectance(
      iris::Vector(0.0, 0.0, 1.0), iris::Vector(0.0, 0.0, 1.0), nullptr,
      iris::Bxdf::Hemisphere::BRDF, iris::testing::GetSpectralAllocator());
  ASSERT_TRUE(returned_reflector);
  EXPECT_NEAR(M_1_PI, returned_reflector->Reflectance(1.0), 0.0001);
}

TEST(PlasticMaterialTest, SpecularOnly) {
  auto reflector =
      iris::MakeReferenceCounted<iris::reflectors::MockReflector>();
  EXPECT_CALL(*reflector, Reflectance(testing::_))
      .Times(1)
      .WillOnce(testing::Return(1.0));

  auto reflectance =
      iris::MakeReferenceCounted<iris::textures::ConstantPointerTexture2D<
          iris::Reflector, iris::SpectralAllocator>>(
          iris::ReferenceCounted<iris::Reflector>());
  auto specular =
      iris::MakeReferenceCounted<iris::textures::ConstantPointerTexture2D<
          iris::Reflector, iris::SpectralAllocator>>(reflector);
  auto eta_incident = iris::MakeReferenceCounted<
      iris::textures::ConstantValueTexture2D<iris::visual>>(
      static_cast<iris::visual>(1.5));
  auto eta_transmitted = iris::MakeReferenceCounted<
      iris::textures::ConstantValueTexture2D<iris::visual>>(
      static_cast<iris::visual>(1.0));
  auto sigma = iris::MakeReferenceCounted<
      iris::textures::ConstantValueTexture2D<iris::visual>>(
      static_cast<iris::visual>(1.0));
  iris::materials::PlasticMaterial material(reflectance, specular, eta_incident,
                                            eta_transmitted, std::move(sigma),
                                            false);

  auto* result = material.Evaluate(
      iris::TextureCoordinates{{0.0, 0.0}, std::nullopt},
      iris::testing::GetSpectralAllocator(), iris::testing::GetBxdfAllocator());
  ASSERT_TRUE(result);

  auto* returned_reflector = result->Reflectance(
      iris::Vector(0.0, 0.0, 1.0), iris::Vector(0.0, 0.0, 1.0), nullptr,
      iris::Bxdf::Hemisphere::BRDF, iris::testing::GetSpectralAllocator());
  ASSERT_TRUE(returned_reflector);
  EXPECT_NEAR(0.0031830, returned_reflector->Reflectance(1.0), 0.0001);
}

TEST(PlasticMaterialTest, Remap) {
  auto reflector =
      iris::MakeReferenceCounted<iris::reflectors::MockReflector>();
  EXPECT_CALL(*reflector, Reflectance(testing::_))
      .Times(1)
      .WillOnce(testing::Return(1.0));

  auto reflectance =
      iris::MakeReferenceCounted<iris::textures::ConstantPointerTexture2D<
          iris::Reflector, iris::SpectralAllocator>>(
          iris::ReferenceCounted<iris::Reflector>());
  auto specular =
      iris::MakeReferenceCounted<iris::textures::ConstantPointerTexture2D<
          iris::Reflector, iris::SpectralAllocator>>(reflector);
  auto eta_incident = iris::MakeReferenceCounted<
      iris::textures::ConstantValueTexture2D<iris::visual>>(
      static_cast<iris::visual>(1.5));
  auto eta_transmitted = iris::MakeReferenceCounted<
      iris::textures::ConstantValueTexture2D<iris::visual>>(
      static_cast<iris::visual>(1.0));
  auto sigma = iris::MakeReferenceCounted<
      iris::textures::ConstantValueTexture2D<iris::visual>>(
      static_cast<iris::visual>(1.0));
  iris::materials::PlasticMaterial material(reflectance, specular, eta_incident,
                                            eta_transmitted, std::move(sigma),
                                            true);

  auto* result = material.Evaluate(
      iris::TextureCoordinates{{0.0, 0.0}, std::nullopt},
      iris::testing::GetSpectralAllocator(), iris::testing::GetBxdfAllocator());
  ASSERT_TRUE(result);

  auto* returned_reflector = result->Reflectance(
      iris::Vector(0.0, 0.0, 1.0), iris::Vector(0.0, 0.0, 1.0), nullptr,
      iris::Bxdf::Hemisphere::BRDF, iris::testing::GetSpectralAllocator());
  ASSERT_TRUE(returned_reflector);
  EXPECT_NEAR(0.0012107628, returned_reflector->Reflectance(1.0), 0.0001);
}

TEST(PlasticMaterialTest, Both) {
  auto reflector =
      iris::MakeReferenceCounted<iris::reflectors::MockReflector>();
  EXPECT_CALL(*reflector, Reflectance(testing::_))
      .Times(2)
      .WillRepeatedly(testing::Return(1.0));

  auto reflectance =
      iris::MakeReferenceCounted<iris::textures::ConstantPointerTexture2D<
          iris::Reflector, iris::SpectralAllocator>>(reflector);
  auto specular =
      iris::MakeReferenceCounted<iris::textures::ConstantPointerTexture2D<
          iris::Reflector, iris::SpectralAllocator>>(reflector);
  auto eta_incident = iris::MakeReferenceCounted<
      iris::textures::ConstantValueTexture2D<iris::visual>>(
      static_cast<iris::visual>(1.5));
  auto eta_transmitted = iris::MakeReferenceCounted<
      iris::textures::ConstantValueTexture2D<iris::visual>>(
      static_cast<iris::visual>(1.0));
  auto sigma = iris::MakeReferenceCounted<
      iris::textures::ConstantValueTexture2D<iris::visual>>(
      static_cast<iris::visual>(1.0));
  iris::materials::PlasticMaterial material(reflectance, specular, eta_incident,
                                            eta_transmitted, std::move(sigma),
                                            false);

  auto* result = material.Evaluate(
      iris::TextureCoordinates{{0.0, 0.0}, std::nullopt},
      iris::testing::GetSpectralAllocator(), iris::testing::GetBxdfAllocator());
  ASSERT_TRUE(result);

  auto* returned_reflector = result->Reflectance(
      iris::Vector(0.0, 0.0, 1.0), iris::Vector(0.0, 0.0, 1.0), nullptr,
      iris::Bxdf::Hemisphere::BRDF, iris::testing::GetSpectralAllocator());
  ASSERT_TRUE(returned_reflector);
  EXPECT_NEAR(0.321492, returned_reflector->Reflectance(1.0), 0.01);
}