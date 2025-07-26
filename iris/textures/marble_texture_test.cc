#include "iris/textures/marble_texture.h"

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

TEST(MarbleReflectorTexture, Null) {
  EXPECT_FALSE(MakeMarbleTexture(Matrix::Identity(), 1u, 0.5, 1.0, 1.0,
                                 ReferenceCounted<ReflectorTexture>(),
                                 ReferenceCounted<ReflectorTexture>(),
                                 ReferenceCounted<ReflectorTexture>()));
  EXPECT_FALSE(MakeMarbleTexture(
      Matrix::Identity(), 1u, -0.5, 1.0, 1.0,
      MakeConstantTexture(MakeReferenceCounted<MockReflector>()),
      MakeConstantTexture(MakeReferenceCounted<MockReflector>()),
      MakeConstantTexture(MakeReferenceCounted<MockReflector>())));
  EXPECT_FALSE(MakeMarbleTexture(
      Matrix::Identity(), 1u, std::numeric_limits<visual_t>::infinity(), 1.0,
      1.0, MakeConstantTexture(MakeReferenceCounted<MockReflector>()),
      MakeConstantTexture(MakeReferenceCounted<MockReflector>()),
      MakeConstantTexture(MakeReferenceCounted<MockReflector>())));
  EXPECT_FALSE(MakeMarbleTexture(
      Matrix::Identity(), 1u, 0.5, std::numeric_limits<visual_t>::infinity(),
      1.0, MakeConstantTexture(MakeReferenceCounted<MockReflector>()),
      MakeConstantTexture(MakeReferenceCounted<MockReflector>()),
      MakeConstantTexture(MakeReferenceCounted<MockReflector>())));
  EXPECT_FALSE(MakeMarbleTexture(
      Matrix::Identity(), 1u, 0.5, 1.0,
      std::numeric_limits<visual_t>::infinity(),
      MakeConstantTexture(MakeReferenceCounted<MockReflector>()),
      MakeConstantTexture(MakeReferenceCounted<MockReflector>()),
      MakeConstantTexture(MakeReferenceCounted<MockReflector>())));
  EXPECT_TRUE(MakeMarbleTexture(
      Matrix::Identity(), 1u, 0.5, 1.0, 1.0,
      MakeConstantTexture(MakeReferenceCounted<MockReflector>()),
      MakeConstantTexture(MakeReferenceCounted<MockReflector>()),
      MakeConstantTexture(MakeReferenceCounted<MockReflector>())));
}

}  // namespace
}  // namespace textures
}  // namespace iris
