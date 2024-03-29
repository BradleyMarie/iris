#define _USE_MATH_DEFINES
#include "iris/materials/matte_material.h"

#include <cmath>
#include <memory>

#include "googlemock/include/gmock/gmock.h"
#include "googletest/include/gtest/gtest.h"
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

TEST(MatteMaterialTest, Evaluate) {
  auto reflector =
      iris::MakeReferenceCounted<iris::reflectors::MockReflector>();
  EXPECT_CALL(*reflector, Reflectance(testing::_))
      .Times(1)
      .WillOnce(testing::Return(static_cast<iris::visual_t>(1.0)));

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

  auto* returned_reflector = result->Reflectance(
      iris::Vector(0.0, 0.0, 1.0), iris::Vector(0.0, 0.0, 1.0), nullptr,
      iris::Bxdf::Hemisphere::BRDF, iris::testing::GetSpectralAllocator());
  ASSERT_TRUE(returned_reflector);
  EXPECT_NEAR(M_1_PI, returned_reflector->Reflectance(1.0), 0.0001);
}