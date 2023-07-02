#include "iris/normal_maps/bump_normal_map.h"

#include "googlemock/include/gmock/gmock.h"
#include "googletest/include/gtest/gtest.h"
#include "iris/textures/constant_texture.h"
#include "iris/textures/image_texture.h"

TEST(BumpNormalMap, EvaluateNoDifferentials) {
  auto bump = iris::MakeReferenceCounted<
      iris::textures::ConstantValueTexture2D<iris::visual>>(0.0);
  iris::normals::BumpNormalMap normal_map(std::move(bump));

  auto result = normal_map.Evaluate(iris::TextureCoordinates{}, std::nullopt,
                                    iris::Vector(0.0, 0.0, 1.0));
  EXPECT_EQ(iris::Vector(0.0, 0.0, 1.0), result);
}

TEST(BumpNormalMap, EvaluateNoNormalMapDifferentials) {
  auto bump = iris::MakeReferenceCounted<
      iris::textures::ConstantValueTexture2D<iris::visual>>(0.0);
  iris::normals::BumpNormalMap normal_map(std::move(bump));

  auto result = normal_map.Evaluate(
      iris::TextureCoordinates{{0.0, 0.0}, {{1.0, 1.0, 1.0, 1.0}}},
      std::nullopt, iris::Vector(0.0, 0.0, 1.0));
  EXPECT_EQ(iris::Vector(0.0, 0.0, 1.0), result);
}

TEST(BumpNormalMap, EvaluateNoTextureCoordinateDifferentials) {
  auto bump = iris::MakeReferenceCounted<
      iris::textures::ConstantValueTexture2D<iris::visual>>(0.0);
  iris::normals::BumpNormalMap normal_map(std::move(bump));

  auto result = normal_map.Evaluate(
      iris::TextureCoordinates{},
      {{iris::NormalMap::Differentials::DX_DY,
        {iris::Vector(1.0, 0.0, 0.0), iris::Vector(0.0, 1.0, 0.0)}}},
      iris::Vector(0.0, 0.0, 1.0));
  EXPECT_EQ(iris::Vector(0.0, 0.0, 1.0), result);
}

TEST(BumpNormalMap, EvaluateConstantValueXY) {
  auto bump = iris::MakeReferenceCounted<
      iris::textures::ConstantValueTexture2D<iris::visual>>(0.0);
  iris::normals::BumpNormalMap normal_map(std::move(bump));

  auto result = normal_map.Evaluate(
      iris::TextureCoordinates{{0.0, 0.0}, {{1.0, 1.0, 1.0, 1.0}}},
      {{iris::NormalMap::Differentials::DX_DY,
        {iris::Vector(1.0, 0.0, 0.0), iris::Vector(0.0, 1.0, 0.0)}}},
      iris::Vector(0.0, 0.0, 1.0));
  EXPECT_EQ(iris::Vector(0.0, 0.0, 1.0), result);
}

TEST(BumpNormalMap, EvaluateConstantValueUV) {
  auto bump = iris::MakeReferenceCounted<
      iris::textures::ConstantValueTexture2D<iris::visual>>(0.0);
  iris::normals::BumpNormalMap normal_map(std::move(bump));

  auto result = normal_map.Evaluate(
      iris::TextureCoordinates{{0.0, 0.0}, {{1.0, 1.0, 1.0, 1.0}}},
      {{iris::NormalMap::Differentials::DU_DV,
        {iris::Vector(1.0, 0.0, 0.0), iris::Vector(0.0, 1.0, 0.0)}}},
      iris::Vector(0.0, 0.0, 1.0));
  EXPECT_EQ(iris::Vector(0.0, 0.0, 1.0), result);
}

TEST(BumpNormalMap, EvaluateXY) {
  std::vector<float> values;
  values.push_back(0.0);
  values.push_back(1.0);
  values.push_back(1.0);
  values.push_back(2.0);
  auto image = std::make_shared<iris::textures::Image2D<float>>(
      values, std::pair<size_t, size_t>(2, 2));

  auto bump =
      iris::MakeReferenceCounted<iris::textures::ClampedImageTexture2D<float>>(
          image, std::nullopt, std::nullopt, std::nullopt, std::nullopt);
  iris::normals::BumpNormalMap normal_map(std::move(bump));

  auto result = normal_map.Evaluate(
      iris::TextureCoordinates{{0.0, 0.0}, {{1.0, 0.0, 0.0, 1.0}}},
      {{iris::NormalMap::Differentials::DX_DY,
        {iris::Vector(1.0, 0.0, 0.0), iris::Vector(0.0, 1.0, 0.0)}}},
      iris::Vector(0.0, 0.0, 1.0));
  EXPECT_NEAR(-1.0, result.x, 0.001);
  EXPECT_NEAR(-1.0, result.y, 0.001);
  EXPECT_NEAR(+1.0, result.z, 0.001);
}

TEST(BumpNormalMap, EvaluateUV) {
  std::vector<float> values;
  values.push_back(0.0);
  values.push_back(1.0);
  values.push_back(1.0);
  values.push_back(2.0);
  auto image = std::make_shared<iris::textures::Image2D<float>>(
      values, std::pair<size_t, size_t>(2, 2));

  auto bump =
      iris::MakeReferenceCounted<iris::textures::ClampedImageTexture2D<float>>(
          image, std::nullopt, std::nullopt, std::nullopt, std::nullopt);
  iris::normals::BumpNormalMap normal_map(std::move(bump));

  auto result = normal_map.Evaluate(
      iris::TextureCoordinates{{0.0, 0.0}, {{1.0, 0.0, 0.0, 1.0}}},
      {{iris::NormalMap::Differentials::DU_DV,
        {iris::Vector(1.0, 0.0, 0.0), iris::Vector(0.0, 1.0, 0.0)}}},
      iris::Vector(0.0, 0.0, 1.0));
  EXPECT_NEAR(-1.0, result.x, 0.001);
  EXPECT_NEAR(-1.0, result.y, 0.001);
  EXPECT_NEAR(+1.0, result.z, 0.001);
}