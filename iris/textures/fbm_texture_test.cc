#include "iris/textures/fbm_texture.h"

#include <limits>

#include "googletest/include/gtest/gtest.h"
#include "iris/reference_counted.h"
#include "iris/reflectors/mock_reflector.h"
#include "iris/textures/constant_texture.h"

namespace iris {
namespace textures {
namespace {

using ::iris::reflectors::MockReflector;
using ::iris::textures::MakeConstantTexture;

TEST(FbmFloatTexture, Null) {
  EXPECT_FALSE(MakeFbmTexture(1u, -1.0));
  EXPECT_FALSE(MakeFbmTexture(1u, std::numeric_limits<visual_t>::infinity()));
  EXPECT_TRUE(MakeFbmTexture(1u, 1.0));
}

TEST(FbmReflectorTexture, Null) {
  EXPECT_FALSE(MakeFbmTexture(ReferenceCounted<ReflectorTexture>(), 1u, 0.5));
  EXPECT_FALSE(MakeFbmTexture(
      MakeConstantTexture(MakeReferenceCounted<MockReflector>()), 1u, -0.5));
  EXPECT_FALSE(
      MakeFbmTexture(MakeConstantTexture(MakeReferenceCounted<MockReflector>()),
                     1u, std::numeric_limits<visual_t>::infinity()));
  EXPECT_TRUE(MakeFbmTexture(
      MakeConstantTexture(MakeReferenceCounted<MockReflector>()), 1u, 1.0));
}

}  // namespace
}  // namespace textures
}  // namespace iris
