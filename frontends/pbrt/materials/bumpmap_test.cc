#include "frontends/pbrt/materials/bumpmap.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/textures/constant_texture.h"

namespace iris {
namespace pbrt_frontend {
namespace materials {
namespace {

using ::iris::textures::ConstantValueTexture2D;

TEST(BumpMap, Allocates) {
  ReferenceCounted<ConstantValueTexture2D<visual>> value_texture =
      MakeReferenceCounted<ConstantValueTexture2D<visual>>(
          static_cast<visual>(1.0));
  std::pair<ReferenceCounted<NormalMap>, ReferenceCounted<NormalMap>>
      bump_maps = MakeBumpMap(value_texture);
  EXPECT_TRUE(bump_maps.first);
  EXPECT_TRUE(bump_maps.second);
}

}  // namespace
}  // namespace materials
}  // namespace pbrt_frontend
}  // namespace iris