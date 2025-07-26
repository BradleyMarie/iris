#include "iris/textures/wrinkled_texture.h"

#include <limits>

#include "googletest/include/gtest/gtest.h"
#include "iris/matrix.h"
#include "iris/reference_counted.h"
#include "iris/reflectors/mock_reflector.h"
#include "iris/textures/constant_texture.h"

namespace iris {
namespace textures {
namespace {

using ::iris::reflectors::MockReflector;
using ::iris::textures::MakeConstantTexture;

TEST(WrinkledFloatTexture, Null) {
  EXPECT_FALSE(MakeWrinkledTexture(Matrix::Identity(), 1u, -1.0));
  EXPECT_FALSE(MakeWrinkledTexture(Matrix::Identity(), 1u,
                                   std::numeric_limits<visual_t>::infinity()));
  EXPECT_TRUE(MakeWrinkledTexture(Matrix::Identity(), 1u, 1.0));
}

TEST(WrinkledReflectorTexture, Null) {
  EXPECT_FALSE(MakeWrinkledTexture(Matrix::Identity(), 1u, 0.5,
                                   ReferenceCounted<ReflectorTexture>()));
  EXPECT_FALSE(MakeWrinkledTexture(
      Matrix::Identity(), 1u, -0.5,
      MakeConstantTexture(MakeReferenceCounted<MockReflector>())));
  EXPECT_FALSE(MakeWrinkledTexture(
      Matrix::Identity(), 1u, std::numeric_limits<visual_t>::infinity(),
      MakeConstantTexture(MakeReferenceCounted<MockReflector>())));
  EXPECT_TRUE(MakeWrinkledTexture(
      Matrix::Identity(), 1u, 1.0,
      MakeConstantTexture(MakeReferenceCounted<MockReflector>())));
}

}  // namespace
}  // namespace textures
}  // namespace iris
