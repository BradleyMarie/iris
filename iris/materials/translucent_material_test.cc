#define _USE_MATH_DEFINES
#include "iris/materials/translucent_material.h"

#include <cmath>
#include <memory>

#include "googlemock/include/gmock/gmock.h"
#include "googletest/include/gtest/gtest.h"
#include "iris/bxdfs/composite_bxdf.h"
#include "iris/bxdfs/lambertian_bxdf.h"
#include "iris/bxdfs/microfacet_bxdf.h"
#include "iris/reflectors/mock_reflector.h"
#include "iris/testing/bxdf_allocator.h"
#include "iris/testing/spectral_allocator.h"
#include "iris/textures/constant_texture.h"

TEST(TranslucentMaterialTest, NoBxdfs) {
  auto reflectance =
      iris::MakeReferenceCounted<iris::textures::ConstantPointerTexture2D<
          iris::Reflector, iris::SpectralAllocator>>(
          iris::ReferenceCounted<iris::Reflector>());
  auto transmittance =
      iris::MakeReferenceCounted<iris::textures::ConstantPointerTexture2D<
          iris::Reflector, iris::SpectralAllocator>>(
          iris::ReferenceCounted<iris::Reflector>());
  auto diffuse =
      iris::MakeReferenceCounted<iris::textures::ConstantPointerTexture2D<
          iris::Reflector, iris::SpectralAllocator>>(
          iris::ReferenceCounted<iris::Reflector>());
  auto specular =
      iris::MakeReferenceCounted<iris::textures::ConstantPointerTexture2D<
          iris::Reflector, iris::SpectralAllocator>>(
          iris::ReferenceCounted<iris::Reflector>());
  auto eta_incident = iris::MakeReferenceCounted<
      iris::textures::ConstantValueTexture2D<iris::visual>>(
      static_cast<iris::visual>(1.0));
  auto eta_transmitted = iris::MakeReferenceCounted<
      iris::textures::ConstantValueTexture2D<iris::visual>>(
      static_cast<iris::visual>(1.5));
  auto sigma = iris::MakeReferenceCounted<
      iris::textures::ConstantValueTexture2D<iris::visual>>(
      static_cast<iris::visual>(1.0));
  iris::materials::TranslucentMaterial material(reflectance, transmittance,
                                                diffuse, specular, eta_incident,
                                                eta_transmitted, sigma, false);

  auto* result = material.Evaluate(
      iris::TextureCoordinates{{0.0, 0.0}, std::nullopt},
      iris::testing::GetSpectralAllocator(), iris::testing::GetBxdfAllocator());
  ASSERT_FALSE(result);
}

TEST(TranslucentMaterialTest, DiffuseReflection) {
  auto reflector =
      iris::MakeReferenceCounted<iris::reflectors::MockReflector>();

  auto reflectance =
      iris::MakeReferenceCounted<iris::textures::ConstantPointerTexture2D<
          iris::Reflector, iris::SpectralAllocator>>(reflector);
  auto transmittance =
      iris::MakeReferenceCounted<iris::textures::ConstantPointerTexture2D<
          iris::Reflector, iris::SpectralAllocator>>(
          iris::ReferenceCounted<iris::Reflector>());
  auto diffuse =
      iris::MakeReferenceCounted<iris::textures::ConstantPointerTexture2D<
          iris::Reflector, iris::SpectralAllocator>>(reflector);
  auto specular =
      iris::MakeReferenceCounted<iris::textures::ConstantPointerTexture2D<
          iris::Reflector, iris::SpectralAllocator>>(
          iris::ReferenceCounted<iris::Reflector>());
  auto eta_incident = iris::MakeReferenceCounted<
      iris::textures::ConstantValueTexture2D<iris::visual>>(
      static_cast<iris::visual>(1.0));
  auto eta_transmitted = iris::MakeReferenceCounted<
      iris::textures::ConstantValueTexture2D<iris::visual>>(
      static_cast<iris::visual>(1.5));
  auto sigma = iris::MakeReferenceCounted<
      iris::textures::ConstantValueTexture2D<iris::visual>>(
      static_cast<iris::visual>(1.0));
  iris::materials::TranslucentMaterial material(reflectance, transmittance,
                                                diffuse, specular, eta_incident,
                                                eta_transmitted, sigma, false);

  auto* result = material.Evaluate(
      iris::TextureCoordinates{{0.0, 0.0}, std::nullopt},
      iris::testing::GetSpectralAllocator(), iris::testing::GetBxdfAllocator());
  ASSERT_TRUE(result);
  EXPECT_TRUE(dynamic_cast<const iris::bxdfs::LambertianBrdf*>(result));
}

TEST(TranslucentMaterialTest, DiffuseTransmission) {
  auto reflector =
      iris::MakeReferenceCounted<iris::reflectors::MockReflector>();

  auto reflectance =
      iris::MakeReferenceCounted<iris::textures::ConstantPointerTexture2D<
          iris::Reflector, iris::SpectralAllocator>>(
          iris::ReferenceCounted<iris::Reflector>());
  auto transmittance =
      iris::MakeReferenceCounted<iris::textures::ConstantPointerTexture2D<
          iris::Reflector, iris::SpectralAllocator>>(reflector);
  auto diffuse =
      iris::MakeReferenceCounted<iris::textures::ConstantPointerTexture2D<
          iris::Reflector, iris::SpectralAllocator>>(reflector);
  auto specular =
      iris::MakeReferenceCounted<iris::textures::ConstantPointerTexture2D<
          iris::Reflector, iris::SpectralAllocator>>(
          iris::ReferenceCounted<iris::Reflector>());
  auto eta_incident = iris::MakeReferenceCounted<
      iris::textures::ConstantValueTexture2D<iris::visual>>(
      static_cast<iris::visual>(1.0));
  auto eta_transmitted = iris::MakeReferenceCounted<
      iris::textures::ConstantValueTexture2D<iris::visual>>(
      static_cast<iris::visual>(1.5));
  auto sigma = iris::MakeReferenceCounted<
      iris::textures::ConstantValueTexture2D<iris::visual>>(
      static_cast<iris::visual>(1.0));
  iris::materials::TranslucentMaterial material(reflectance, transmittance,
                                                diffuse, specular, eta_incident,
                                                eta_transmitted, sigma, false);

  auto* result = material.Evaluate(
      iris::TextureCoordinates{{0.0, 0.0}, std::nullopt},
      iris::testing::GetSpectralAllocator(), iris::testing::GetBxdfAllocator());
  ASSERT_TRUE(result);
  EXPECT_TRUE(dynamic_cast<const iris::bxdfs::LambertianBtdf*>(result));
}

TEST(TranslucentMaterialTest, SpecularReflection) {
  auto reflector =
      iris::MakeReferenceCounted<iris::reflectors::MockReflector>();

  auto reflectance =
      iris::MakeReferenceCounted<iris::textures::ConstantPointerTexture2D<
          iris::Reflector, iris::SpectralAllocator>>(reflector);
  auto transmittance =
      iris::MakeReferenceCounted<iris::textures::ConstantPointerTexture2D<
          iris::Reflector, iris::SpectralAllocator>>(
          iris::ReferenceCounted<iris::Reflector>());
  auto diffuse =
      iris::MakeReferenceCounted<iris::textures::ConstantPointerTexture2D<
          iris::Reflector, iris::SpectralAllocator>>(
          iris::ReferenceCounted<iris::Reflector>());
  auto specular =
      iris::MakeReferenceCounted<iris::textures::ConstantPointerTexture2D<
          iris::Reflector, iris::SpectralAllocator>>(reflector);
  auto eta_incident = iris::MakeReferenceCounted<
      iris::textures::ConstantValueTexture2D<iris::visual>>(
      static_cast<iris::visual>(1.0));
  auto eta_transmitted = iris::MakeReferenceCounted<
      iris::textures::ConstantValueTexture2D<iris::visual>>(
      static_cast<iris::visual>(1.5));
  auto sigma = iris::MakeReferenceCounted<
      iris::textures::ConstantValueTexture2D<iris::visual>>(
      static_cast<iris::visual>(1.0));
  iris::materials::TranslucentMaterial material(reflectance, transmittance,
                                                diffuse, specular, eta_incident,
                                                eta_transmitted, sigma, false);

  auto* result = material.Evaluate(
      iris::TextureCoordinates{{0.0, 0.0}, std::nullopt},
      iris::testing::GetSpectralAllocator(), iris::testing::GetBxdfAllocator());
  ASSERT_TRUE(result);

  auto* cast = dynamic_cast<const iris::bxdfs::MicrofacetBrdf<
      iris::bxdfs::TrowbridgeReitzDistribution,
      iris::bxdfs::FresnelDielectric>*>(result);
  EXPECT_TRUE(cast);
}

TEST(TranslucentMaterialTest, SpecularTransmission) {
  auto reflector =
      iris::MakeReferenceCounted<iris::reflectors::MockReflector>();

  auto reflectance =
      iris::MakeReferenceCounted<iris::textures::ConstantPointerTexture2D<
          iris::Reflector, iris::SpectralAllocator>>(
          iris::ReferenceCounted<iris::Reflector>());
  auto transmittance =
      iris::MakeReferenceCounted<iris::textures::ConstantPointerTexture2D<
          iris::Reflector, iris::SpectralAllocator>>(reflector);
  auto diffuse =
      iris::MakeReferenceCounted<iris::textures::ConstantPointerTexture2D<
          iris::Reflector, iris::SpectralAllocator>>(
          iris::ReferenceCounted<iris::Reflector>());
  auto specular =
      iris::MakeReferenceCounted<iris::textures::ConstantPointerTexture2D<
          iris::Reflector, iris::SpectralAllocator>>(reflector);
  auto eta_incident = iris::MakeReferenceCounted<
      iris::textures::ConstantValueTexture2D<iris::visual>>(
      static_cast<iris::visual>(1.0));
  auto eta_transmitted = iris::MakeReferenceCounted<
      iris::textures::ConstantValueTexture2D<iris::visual>>(
      static_cast<iris::visual>(1.5));
  auto sigma = iris::MakeReferenceCounted<
      iris::textures::ConstantValueTexture2D<iris::visual>>(
      static_cast<iris::visual>(1.0));
  iris::materials::TranslucentMaterial material(reflectance, transmittance,
                                                diffuse, specular, eta_incident,
                                                eta_transmitted, sigma, false);

  auto* result = material.Evaluate(
      iris::TextureCoordinates{{0.0, 0.0}, std::nullopt},
      iris::testing::GetSpectralAllocator(), iris::testing::GetBxdfAllocator());
  ASSERT_TRUE(result);

  auto* casted = dynamic_cast<const iris::bxdfs::MicrofacetBtdf<
      iris::bxdfs::TrowbridgeReitzDistribution,
      iris::bxdfs::FresnelDielectric>*>(result);
  EXPECT_TRUE(casted);
}

TEST(TranslucentMaterialTest, All) {
  auto reflector =
      iris::MakeReferenceCounted<iris::reflectors::MockReflector>();

  auto reflectance =
      iris::MakeReferenceCounted<iris::textures::ConstantPointerTexture2D<
          iris::Reflector, iris::SpectralAllocator>>(reflector);
  auto transmittance =
      iris::MakeReferenceCounted<iris::textures::ConstantPointerTexture2D<
          iris::Reflector, iris::SpectralAllocator>>(reflector);
  auto diffuse =
      iris::MakeReferenceCounted<iris::textures::ConstantPointerTexture2D<
          iris::Reflector, iris::SpectralAllocator>>(reflector);
  auto specular =
      iris::MakeReferenceCounted<iris::textures::ConstantPointerTexture2D<
          iris::Reflector, iris::SpectralAllocator>>(reflector);
  auto eta_incident = iris::MakeReferenceCounted<
      iris::textures::ConstantValueTexture2D<iris::visual>>(
      static_cast<iris::visual>(1.0));
  auto eta_transmitted = iris::MakeReferenceCounted<
      iris::textures::ConstantValueTexture2D<iris::visual>>(
      static_cast<iris::visual>(1.5));
  auto sigma = iris::MakeReferenceCounted<
      iris::textures::ConstantValueTexture2D<iris::visual>>(
      static_cast<iris::visual>(1.0));
  iris::materials::TranslucentMaterial material(reflectance, transmittance,
                                                diffuse, specular, eta_incident,
                                                eta_transmitted, sigma, true);

  auto* result = material.Evaluate(
      iris::TextureCoordinates{{0.0, 0.0}, std::nullopt},
      iris::testing::GetSpectralAllocator(), iris::testing::GetBxdfAllocator());
  ASSERT_TRUE(result);
  EXPECT_TRUE(
      dynamic_cast<const iris::bxdfs::internal::CompositeBxdf<4>*>(result));
}