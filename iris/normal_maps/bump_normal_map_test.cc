#include "iris/normal_maps/bump_normal_map.h"

#include <optional>
#include <utility>

#include "googlemock/include/gmock/gmock.h"
#include "googletest/include/gtest/gtest.h"
#include "iris/float.h"
#include "iris/normal_map.h"
#include "iris/point.h"
#include "iris/reference_counted.h"
#include "iris/texture_coordinates.h"
#include "iris/textures/float_texture.h"
#include "iris/textures/image_texture.h"
#include "iris/textures/test_util.h"

namespace iris {
namespace normal_maps {
namespace {

using ::iris::textures::FloatTexture;
using ::iris::textures::Image2D;
using ::iris::textures::MakeClampedImageTexture;
using ::iris::textures::MakeZeroTexture;

TEST(BumpNormalMap, Null) {
  EXPECT_FALSE(MakeBumpNormalMap(ReferenceCounted<FloatTexture>()));
}

TEST(BumpNormalMap, EvaluateNoDifferentials) {
  ReferenceCounted<NormalMap> normal_map = MakeBumpNormalMap(MakeZeroTexture());

  Vector result = normal_map->Evaluate(TextureCoordinates{Point(0.0, 0.0, 0.0),
                                                          Vector(0.0, 0.0, 0.0),
                                                          Vector(0.0, 0.0, 0.0),
                                                          {0.0, 0.0},
                                                          0.0,
                                                          0.0,
                                                          0.0,
                                                          0.0},
                                       std::nullopt, Vector(0.0, 0.0, 1.0));
  EXPECT_EQ(Vector(0.0, 0.0, 1.0), result);
}

TEST(BumpNormalMap, EvaluateNoNormalMapDifferentials) {
  ReferenceCounted<NormalMap> normal_map = MakeBumpNormalMap(MakeZeroTexture());

  Vector result = normal_map->Evaluate(TextureCoordinates{Point(0.0, 0.0, 0.0),
                                                          Vector(0.0, 0.0, 0.0),
                                                          Vector(0.0, 0.0, 0.0),
                                                          {0.0, 0.0},
                                                          1.0,
                                                          1.0,
                                                          1.0,
                                                          1.0},
                                       std::nullopt, Vector(0.0, 0.0, 1.0));
  EXPECT_EQ(Vector(0.0, 0.0, 1.0), result);
}

TEST(BumpNormalMap, EvaluateNoTextureCoordinateDifferentials) {
  ReferenceCounted<NormalMap> normal_map = MakeBumpNormalMap(MakeZeroTexture());

  Vector result =
      normal_map->Evaluate(TextureCoordinates{Point(0.0, 0.0, 0.0),
                                              Vector(0.0, 0.0, 0.0),
                                              Vector(0.0, 0.0, 0.0),
                                              {0.0, 0.0},
                                              0.0,
                                              0.0,
                                              0.0,
                                              0.0},
                           {{NormalMap::Differentials::DX_DY,
                             {Vector(1.0, 0.0, 0.0), Vector(0.0, 1.0, 0.0)}}},
                           Vector(0.0, 0.0, 1.0));
  EXPECT_EQ(Vector(0.0, 0.0, 1.0), result);
}

TEST(BumpNormalMap, EvaluateNoDUVTextureCoordinateDifferentials) {
  ReferenceCounted<NormalMap> normal_map = MakeBumpNormalMap(MakeZeroTexture());

  Vector result =
      normal_map->Evaluate(TextureCoordinates{Point(0.0, 0.0, 0.0),
                                              Vector(0.0, 0.0, 0.0),
                                              Vector(0.0, 0.0, 0.0),
                                              {0.0, 0.0},
                                              0.0,
                                              0.0,
                                              0.0,
                                              0.0},
                           {{NormalMap::Differentials::DX_DY,
                             {Vector(1.0, 0.0, 0.0), Vector(0.0, 1.0, 0.0)}}},
                           Vector(0.0, 0.0, 1.0));
  EXPECT_EQ(Vector(0.0, 0.0, 1.0), result);
}

TEST(BumpNormalMap, EvaluateConstantValueXY) {
  ReferenceCounted<NormalMap> normal_map = MakeBumpNormalMap(MakeZeroTexture());

  Vector result =
      normal_map->Evaluate(TextureCoordinates{Point(0.0, 0.0, 0.0),
                                              Vector(0.0, 0.0, 0.0),
                                              Vector(0.0, 0.0, 0.0),
                                              {0.0, 0.0},
                                              1.0,
                                              1.0,
                                              1.0,
                                              1.0},
                           {{NormalMap::Differentials::DX_DY,
                             {Vector(1.0, 0.0, 0.0), Vector(0.0, 1.0, 0.0)}}},
                           Vector(0.0, 0.0, 1.0));
  EXPECT_EQ(Vector(0.0, 0.0, 1.0), result);
}

TEST(BumpNormalMap, EvaluateConstantValueUV) {
  ReferenceCounted<NormalMap> normal_map = MakeBumpNormalMap(MakeZeroTexture());

  Vector result =
      normal_map->Evaluate(TextureCoordinates{Point(0.0, 0.0, 0.0),
                                              Vector(0.0, 0.0, 0.0),
                                              Vector(0.0, 0.0, 0.0),
                                              {0.0, 0.0},
                                              .0,
                                              .0,
                                              .0,
                                              .0},
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
      MakeBumpNormalMap(MakeClampedImageTexture(image, 1.0, 1.0, 0.0, 0.0));

  Vector result =
      normal_map->Evaluate(TextureCoordinates{Point(0.0, 0.0, 0.0),
                                              Vector(0.0, 0.0, 0.0),
                                              Vector(0.0, 0.0, 0.0),
                                              {0.0, 0.0},
                                              1.0,
                                              0.0,
                                              0.0,
                                              1.0},
                           {{NormalMap::Differentials::DX_DY,
                             {Vector(1.0, 0.0, 0.0), Vector(0.0, 1.0, 0.0)}}},
                           Vector(0.0, 0.0, 1.0));
  EXPECT_NEAR(0.0, result.x, 0.001);
  EXPECT_NEAR(0.0, result.y, 0.001);
  EXPECT_NEAR(1.0, result.z, 0.001);
}

TEST(BumpNormalMap, EvaluateNoDUTextureCoordinateDifferentials) {
  ReferenceCounted<NormalMap> normal_map = MakeBumpNormalMap(MakeZeroTexture());

  Vector result =
      normal_map->Evaluate(TextureCoordinates{Point(0.0, 0.0, 0.0),
                                              Vector(0.0, 0.0, 0.0),
                                              Vector(0.0, 0.0, 0.0),
                                              {0.0, 0.0},
                                              0.0,
                                              1.0,
                                              1.0,
                                              0.0},
                           {{NormalMap::Differentials::DU_DV,
                             {Vector(1.0, 0.0, 0.0), Vector(0.0, 1.0, 0.0)}}},
                           Vector(0.0, 0.0, 1.0));
  EXPECT_EQ(Vector(0.0, 0.0, 1.0), result);
}

TEST(BumpNormalMap, EvaluateNoDVTextureCoordinateDifferentials) {
  ReferenceCounted<NormalMap> normal_map = MakeBumpNormalMap(MakeZeroTexture());

  Vector result =
      normal_map->Evaluate(TextureCoordinates{Point(0.0, 0.0, 0.0),
                                              Vector(0.0, 0.0, 0.0),
                                              Vector(0.0, 0.0, 0.0),
                                              {0.0, 0.0},
                                              1.0,
                                              0.0,
                                              1.0,
                                              0.0},
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
      MakeBumpNormalMap(MakeClampedImageTexture(image, 1.0, 1.0, 0.0, 0.0));

  Vector result =
      normal_map->Evaluate(TextureCoordinates{Point(0.0, 0.0, 0.0),
                                              Vector(0.0, 0.0, 0.0),
                                              Vector(0.0, 0.0, 0.0),
                                              {0.0, 0.0},
                                              1.0,
                                              0.0,
                                              0.0,
                                              1.0},
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
