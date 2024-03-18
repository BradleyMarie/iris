#define _USE_MATH_DEFINES
#include "iris/materials/glass_material.h"

#include <cmath>
#include <memory>

#include "googlemock/include/gmock/gmock.h"
#include "googletest/include/gtest/gtest.h"
#include "iris/bxdfs/specular_bxdf.h"
#include "iris/reflectors/mock_reflector.h"
#include "iris/testing/bxdf_allocator.h"
#include "iris/testing/spectral_allocator.h"
#include "iris/textures/constant_texture.h"

TEST(GlassMaterialTest, EvaluateEmpty) {
  auto reflectance =
      iris::MakeReferenceCounted<iris::textures::ConstantPointerTexture2D<
          iris::Reflector, iris::SpectralAllocator>>(
          iris::ReferenceCounted<iris::Reflector>());
  auto transmittance =
      iris::MakeReferenceCounted<iris::textures::ConstantPointerTexture2D<
          iris::Reflector, iris::SpectralAllocator>>(
          iris::ReferenceCounted<iris::Reflector>());
  auto eta_incident = iris::MakeReferenceCounted<
      iris::textures::ConstantValueTexture2D<iris::visual>>(
      static_cast<iris::visual>(1.0));
  auto eta_transmitted = iris::MakeReferenceCounted<
      iris::textures::ConstantValueTexture2D<iris::visual>>(
      static_cast<iris::visual>(1.0));
  iris::materials::GlassMaterial material(
      std::move(reflectance), std::move(transmittance), std::move(eta_incident),
      std::move(eta_transmitted));

  ASSERT_FALSE(
      material.Evaluate(iris::TextureCoordinates{{0.0, 0.0}, std::nullopt},
                        iris::testing::GetSpectralAllocator(),
                        iris::testing::GetBxdfAllocator()));
}

TEST(GlassMaterialTest, EvaluateBrdf) {
  auto reflector =
      iris::MakeReferenceCounted<iris::reflectors::MockReflector>();

  auto reflectance =
      iris::MakeReferenceCounted<iris::textures::ConstantPointerTexture2D<
          iris::Reflector, iris::SpectralAllocator>>(reflector);
  auto transmittance =
      iris::MakeReferenceCounted<iris::textures::ConstantPointerTexture2D<
          iris::Reflector, iris::SpectralAllocator>>(
          iris::ReferenceCounted<iris::Reflector>());
  auto eta_incident = iris::MakeReferenceCounted<
      iris::textures::ConstantValueTexture2D<iris::visual>>(
      static_cast<iris::visual>(1.0));
  auto eta_transmitted = iris::MakeReferenceCounted<
      iris::textures::ConstantValueTexture2D<iris::visual>>(
      static_cast<iris::visual>(1.0));
  iris::materials::GlassMaterial material(
      std::move(reflectance), std::move(transmittance), std::move(eta_incident),
      std::move(eta_transmitted));

  auto* result = material.Evaluate(
      iris::TextureCoordinates{{0.0, 0.0}, std::nullopt},
      iris::testing::GetSpectralAllocator(), iris::testing::GetBxdfAllocator());
  ASSERT_TRUE(result);
  EXPECT_TRUE(dynamic_cast<
              const iris::bxdfs::SpecularBrdf<iris::bxdfs::FresnelDielectric>*>(
      result));
}

TEST(GlassMaterialTest, EvaluateBtdf) {
  auto transmitter =
      iris::MakeReferenceCounted<iris::reflectors::MockReflector>();

  auto reflectance =
      iris::MakeReferenceCounted<iris::textures::ConstantPointerTexture2D<
          iris::Reflector, iris::SpectralAllocator>>(
          iris::ReferenceCounted<iris::Reflector>());
  auto transmittance =
      iris::MakeReferenceCounted<iris::textures::ConstantPointerTexture2D<
          iris::Reflector, iris::SpectralAllocator>>(transmitter);
  auto eta_incident = iris::MakeReferenceCounted<
      iris::textures::ConstantValueTexture2D<iris::visual>>(
      static_cast<iris::visual>(1.0));
  auto eta_transmitted = iris::MakeReferenceCounted<
      iris::textures::ConstantValueTexture2D<iris::visual>>(
      static_cast<iris::visual>(1.0));
  iris::materials::GlassMaterial material(
      std::move(reflectance), std::move(transmittance), std::move(eta_incident),
      std::move(eta_transmitted));

  auto* result = material.Evaluate(
      iris::TextureCoordinates{{0.0, 0.0}, std::nullopt},
      iris::testing::GetSpectralAllocator(), iris::testing::GetBxdfAllocator());
  ASSERT_TRUE(result);
  EXPECT_TRUE(dynamic_cast<
              const iris::bxdfs::SpecularBtdf<iris::bxdfs::FresnelDielectric>*>(
      result));
}

TEST(GlassMaterialTest, Evaluate) {
  auto reflector =
      iris::MakeReferenceCounted<iris::reflectors::MockReflector>();
  auto transmitter =
      iris::MakeReferenceCounted<iris::reflectors::MockReflector>();

  auto reflectance =
      iris::MakeReferenceCounted<iris::textures::ConstantPointerTexture2D<
          iris::Reflector, iris::SpectralAllocator>>(reflector);
  auto transmittance =
      iris::MakeReferenceCounted<iris::textures::ConstantPointerTexture2D<
          iris::Reflector, iris::SpectralAllocator>>(transmitter);
  auto eta_incident = iris::MakeReferenceCounted<
      iris::textures::ConstantValueTexture2D<iris::visual>>(
      static_cast<iris::visual>(1.0));
  auto eta_transmitted = iris::MakeReferenceCounted<
      iris::textures::ConstantValueTexture2D<iris::visual>>(
      static_cast<iris::visual>(1.0));
  iris::materials::GlassMaterial material(
      std::move(reflectance), std::move(transmittance), std::move(eta_incident),
      std::move(eta_transmitted));

  auto* result = material.Evaluate(
      iris::TextureCoordinates{{0.0, 0.0}, std::nullopt},
      iris::testing::GetSpectralAllocator(), iris::testing::GetBxdfAllocator());
  ASSERT_TRUE(result);
  EXPECT_TRUE(dynamic_cast<const iris::bxdfs::SpecularBxdf*>(result));
}