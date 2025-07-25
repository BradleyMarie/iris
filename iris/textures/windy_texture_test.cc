#include "iris/textures/windy_texture.h"

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

TEST(WindyFloatTexture, Make) {
  EXPECT_TRUE(MakeWindyTexture(Matrix::Identity()));
}

TEST(WindyReflectorTexture, Make) {
  EXPECT_FALSE(MakeWindyTexture(Matrix::Identity(),
                                ReferenceCounted<ReflectorTexture>()));
  EXPECT_TRUE(MakeWindyTexture(
      Matrix::Identity(),
      MakeConstantTexture(MakeReferenceCounted<MockReflector>())));
}

}  // namespace
}  // namespace textures
}  // namespace iris
