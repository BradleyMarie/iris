#include "frontends/pbrt/materials/bumpmap.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/textures/constant_texture.h"

TEST(BumpMap, Allocates) {
  auto value_texture = iris::MakeReferenceCounted<
      iris::textures::ConstantValueTexture2D<iris::visual>>(1.0);
  auto bump_maps = iris::pbrt_frontend::materials::MakeBumpMap(value_texture);
  EXPECT_TRUE(bump_maps.first);
  EXPECT_TRUE(bump_maps.second);
}