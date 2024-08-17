#include "iris/materials/mirror_material.h"

#include <memory>

#include "googlemock/include/gmock/gmock.h"
#include "googletest/include/gtest/gtest.h"
#include "iris/reflectors/mock_reflector.h"
#include "iris/testing/bxdf_allocator.h"
#include "iris/testing/spectral_allocator.h"
#include "iris/textures/constant_texture.h"

TEST(MirrorMaterialTest, EvaluateEmpty) {
  auto reflectance =
      iris::MakeReferenceCounted<iris::textures::ConstantPointerTexture2D<
          iris::Reflector, iris::SpectralAllocator>>(
          iris::ReferenceCounted<iris::Reflector>());
  iris::materials::MirrorMaterial material(std::move(reflectance));

  ASSERT_FALSE(
      material.Evaluate(iris::TextureCoordinates{{0.0, 0.0}, std::nullopt},
                        iris::testing::GetSpectralAllocator(),
                        iris::testing::GetBxdfAllocator()));
}

TEST(MirrorMaterialTest, Evaluate) {
  auto reflector =
      iris::MakeReferenceCounted<iris::reflectors::MockReflector>();
  auto reflectance =
      iris::MakeReferenceCounted<iris::textures::ConstantPointerTexture2D<
          iris::Reflector, iris::SpectralAllocator>>(reflector);
  iris::materials::MirrorMaterial material(std::move(reflectance));

  auto* result = material.Evaluate(
      iris::TextureCoordinates{{0.0, 0.0}, std::nullopt},
      iris::testing::GetSpectralAllocator(), iris::testing::GetBxdfAllocator());
  ASSERT_TRUE(result);

  auto* returned_reflector = result->Reflectance(
      iris::Vector(0.0, 0.0, 1.0), iris::Vector(0.0, 0.0, 1.0),
      iris::Bxdf::Hemisphere::BRDF, iris::testing::GetSpectralAllocator());
  EXPECT_EQ(reflector.Get(), returned_reflector);
}