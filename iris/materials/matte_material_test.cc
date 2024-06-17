#define _USE_MATH_DEFINES
#include "iris/materials/matte_material.h"

#include <cmath>
#include <memory>

#include "googletest/include/gtest/gtest.h"
#include "iris/bxdfs/lambertian_bxdf.h"
#include "iris/bxdfs/oren_nayar_bxdf.h"
#include "iris/reflectors/mock_reflector.h"
#include "iris/testing/bxdf_allocator.h"
#include "iris/testing/spectral_allocator.h"
#include "iris/textures/constant_texture.h"

TEST(MatteMaterialTest, EvaluateEmpty) {
  auto reflectance =
      iris::MakeReferenceCounted<iris::textures::ConstantPointerTexture2D<
          iris::Reflector, iris::SpectralAllocator>>(
          iris::ReferenceCounted<iris::Reflector>());
  auto sigma = iris::MakeReferenceCounted<
      iris::textures::ConstantValueTexture2D<iris::visual>>(
      static_cast<iris::visual>(1.0));
  iris::materials::MatteMaterial material(std::move(reflectance),
                                          std::move(sigma));

  ASSERT_FALSE(
      material.Evaluate(iris::TextureCoordinates{{0.0, 0.0}, std::nullopt},
                        iris::testing::GetSpectralAllocator(),
                        iris::testing::GetBxdfAllocator()));
}

TEST(MatteMaterialTest, EvaluateLambertian) {
  auto reflector =
      iris::MakeReferenceCounted<iris::reflectors::MockReflector>();

  auto reflectance =
      iris::MakeReferenceCounted<iris::textures::ConstantPointerTexture2D<
          iris::Reflector, iris::SpectralAllocator>>(reflector);
  auto sigma = iris::MakeReferenceCounted<
      iris::textures::ConstantValueTexture2D<iris::visual>>(
      static_cast<iris::visual>(0.0));
  iris::materials::MatteMaterial material(std::move(reflectance),
                                          std::move(sigma));

  auto* result = material.Evaluate(
      iris::TextureCoordinates{{0.0, 0.0}, std::nullopt},
      iris::testing::GetSpectralAllocator(), iris::testing::GetBxdfAllocator());
  ASSERT_TRUE(result);
  EXPECT_TRUE(dynamic_cast<const iris::bxdfs::LambertianBrdf*>(result));
}

TEST(MatteMaterialTest, EvaluateOrenNayar) {
  auto reflector =
      iris::MakeReferenceCounted<iris::reflectors::MockReflector>();

  auto reflectance =
      iris::MakeReferenceCounted<iris::textures::ConstantPointerTexture2D<
          iris::Reflector, iris::SpectralAllocator>>(reflector);
  auto sigma = iris::MakeReferenceCounted<
      iris::textures::ConstantValueTexture2D<iris::visual>>(
      static_cast<iris::visual>(1.0));
  iris::materials::MatteMaterial material(std::move(reflectance),
                                          std::move(sigma));

  auto* result = material.Evaluate(
      iris::TextureCoordinates{{0.0, 0.0}, std::nullopt},
      iris::testing::GetSpectralAllocator(), iris::testing::GetBxdfAllocator());
  ASSERT_TRUE(result);
  EXPECT_TRUE(dynamic_cast<const iris::bxdfs::OrenNayarBrdf*>(result));
}