#include "iris/normal_maps/bump_normal_map.h"

#include <optional>
#include <utility>

#include "googlemock/include/gmock/gmock.h"
#include "googletest/include/gtest/gtest.h"
#include "iris/float.h"
#include "iris/normal_map.h"
#include "iris/reference_counted.h"
#include "iris/texture_coordinates.h"
#include "iris/textures/constant_texture.h"
#include "iris/textures/image_texture.h"
#include "iris/textures/texture2d.h"

namespace iris {
namespace normal_maps {
namespace {

using ::iris::textures::ClampedImageTexture2D;
using ::iris::textures::ConstantValueTexture2D;
using ::iris::textures::Image2D;
using ::iris::textures::ValueTexture2D;

TEST(BumpNormalMap, Null) {
  EXPECT_FALSE(MakeBumpNormalMap(ReferenceCounted<ValueTexture2D<visual>>()));
}

TEST(BumpNormalMap, EvaluateNoDifferentials) {
  ReferenceCounted<NormalMap> normal_map =
      MakeBumpNormalMap(MakeReferenceCounted<ConstantValueTexture2D<visual>>(
          static_cast<visual>(0.0)));

  Vector result =
      normal_map->Evaluate(TextureCoordinates{{0.0, 0.0}, std::nullopt},
                           std::nullopt, Vector(0.0, 0.0, 1.0));
  EXPECT_EQ(Vector(0.0, 0.0, 1.0), result);
}

TEST(BumpNormalMap, EvaluateNoNormalMapDifferentials) {
  ReferenceCounted<NormalMap> normal_map =
      MakeBumpNormalMap(MakeReferenceCounted<ConstantValueTexture2D<visual>>(
          static_cast<visual>(0.0)));

  Vector result = normal_map->Evaluate(
      TextureCoordinates{{0.0, 0.0}, {{1.0, 1.0, 1.0, 1.0}}}, std::nullopt,
      Vector(0.0, 0.0, 1.0));
  EXPECT_EQ(Vector(0.0, 0.0, 1.0), result);
}

TEST(BumpNormalMap, EvaluateNoTextureCoordinateDifferentials) {
  ReferenceCounted<NormalMap> normal_map =
      MakeBumpNormalMap(MakeReferenceCounted<ConstantValueTexture2D<visual>>(
          static_cast<visual>(0.0)));

  Vector result =
      normal_map->Evaluate(TextureCoordinates{{0.0, 0.0}, std::nullopt},
                           {{NormalMap::Differentials::DX_DY,
                             {Vector(1.0, 0.0, 0.0), Vector(0.0, 1.0, 0.0)}}},
                           Vector(0.0, 0.0, 1.0));
  EXPECT_EQ(Vector(0.0, 0.0, 1.0), result);
}

TEST(BumpNormalMap, EvaluateNoDUVTextureCoordinateDifferentials) {
  ReferenceCounted<NormalMap> normal_map =
      MakeBumpNormalMap(MakeReferenceCounted<ConstantValueTexture2D<visual>>(
          static_cast<visual>(0.0)));

  Vector result = normal_map->Evaluate(
      TextureCoordinates{{0.0, 0.0}, {{0.0, 0.0, 0.0, 0.0}}},
      {{NormalMap::Differentials::DX_DY,
        {Vector(1.0, 0.0, 0.0), Vector(0.0, 1.0, 0.0)}}},
      Vector(0.0, 0.0, 1.0));
  EXPECT_EQ(Vector(0.0, 0.0, 1.0), result);
}

TEST(BumpNormalMap, EvaluateConstantValueXY) {
  ReferenceCounted<NormalMap> normal_map =
      MakeBumpNormalMap(MakeReferenceCounted<ConstantValueTexture2D<visual>>(
          static_cast<visual>(0.0)));

  Vector result = normal_map->Evaluate(
      TextureCoordinates{{0.0, 0.0}, {{1.0, 1.0, 1.0, 1.0}}},
      {{NormalMap::Differentials::DX_DY,
        {Vector(1.0, 0.0, 0.0), Vector(0.0, 1.0, 0.0)}}},
      Vector(0.0, 0.0, 1.0));
  EXPECT_EQ(Vector(0.0, 0.0, 1.0), result);
}

TEST(BumpNormalMap, EvaluateConstantValueUV) {
  ReferenceCounted<NormalMap> normal_map =
      MakeBumpNormalMap(MakeReferenceCounted<ConstantValueTexture2D<visual>>(
          static_cast<visual>(0.0)));

  Vector result = normal_map->Evaluate(
      TextureCoordinates{{0.0, 0.0}, {{1.0, 1.0, 1.0, 1.0}}},
      {{NormalMap::Differentials::DU_DV,
        {Vector(1.0, 0.0, 0.0), Vector(0.0, 1.0, 0.0)}}},
      Vector(0.0, 0.0, 1.0));
  EXPECT_EQ(Vector(0.0, 0.0, 1.0), result);
}

TEST(BumpNormalMap, EvaluateXY) {
  std::vector<float> values;
  values.push_back(0.0);
  values.push_back(1.0);
  values.push_back(1.0);
  values.push_back(2.0);
  std::shared_ptr<Image2D<float>> image =
      std::make_shared<Image2D<float>>(values, std::pair<size_t, size_t>(2, 2));

  ReferenceCounted<NormalMap> normal_map =
      MakeBumpNormalMap(MakeReferenceCounted<ClampedImageTexture2D<float>>(
          image, std::nullopt, std::nullopt, std::nullopt, std::nullopt));

  Vector result = normal_map->Evaluate(
      TextureCoordinates{{0.0, 0.0}, {{1.0, 0.0, 0.0, 1.0}}},
      {{NormalMap::Differentials::DX_DY,
        {Vector(1.0, 0.0, 0.0), Vector(0.0, 1.0, 0.0)}}},
      Vector(0.0, 0.0, 1.0));
  EXPECT_NEAR(-1.0, result.x, 0.001);
  EXPECT_NEAR(-1.0, result.y, 0.001);
  EXPECT_NEAR(+1.0, result.z, 0.001);
}

TEST(BumpNormalMap, EvaluateNoDUTextureCoordinateDifferentials) {
  ReferenceCounted<NormalMap> normal_map =
      MakeBumpNormalMap(MakeReferenceCounted<ConstantValueTexture2D<visual>>(
          static_cast<visual>(0.0)));

  Vector result = normal_map->Evaluate(
      TextureCoordinates{{0.0, 0.0}, {{0.0, 1.0, 0.0, 1.0}}},
      {{NormalMap::Differentials::DU_DV,
        {Vector(1.0, 0.0, 0.0), Vector(0.0, 1.0, 0.0)}}},
      Vector(0.0, 0.0, 1.0));
  EXPECT_EQ(Vector(0.0, 0.0, 1.0), result);
}

TEST(BumpNormalMap, EvaluateNoDVTextureCoordinateDifferentials) {
  ReferenceCounted<NormalMap> normal_map =
      MakeBumpNormalMap(MakeReferenceCounted<ConstantValueTexture2D<visual>>(
          static_cast<visual>(0.0)));

  Vector result = normal_map->Evaluate(
      TextureCoordinates{{0.0, 0.0}, {{1.0, 0.0, 1.0, 0.0}}},
      {{NormalMap::Differentials::DU_DV,
        {Vector(1.0, 0.0, 0.0), Vector(0.0, 1.0, 0.0)}}},
      Vector(0.0, 0.0, 1.0));
  EXPECT_EQ(Vector(0.0, 0.0, 1.0), result);
}

TEST(BumpNormalMap, EvaluateUV) {
  std::vector<float> values;
  values.push_back(0.0);
  values.push_back(1.0);
  values.push_back(1.0);
  values.push_back(2.0);
  std::shared_ptr<Image2D<float>> image =
      std::make_shared<Image2D<float>>(values, std::pair<size_t, size_t>(2, 2));

  ReferenceCounted<NormalMap> normal_map =
      MakeBumpNormalMap(MakeReferenceCounted<ClampedImageTexture2D<float>>(
          image, std::nullopt, std::nullopt, std::nullopt, std::nullopt));

  Vector result = normal_map->Evaluate(
      TextureCoordinates{{0.0, 0.0}, {{1.0, 0.0, 0.0, 1.0}}},
      {{NormalMap::Differentials::DU_DV,
        {Vector(1.0, 0.0, 0.0), Vector(0.0, 1.0, 0.0)}}},
      Vector(0.0, 0.0, 1.0));
  EXPECT_NEAR(-1.0, result.x, 0.001);
  EXPECT_NEAR(-1.0, result.y, 0.001);
  EXPECT_NEAR(+1.0, result.z, 0.001);
}

}  // namespace
}  // namespace normal_maps
}  // namespace iris
