#include "iris/textures/constant_texture.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/point.h"
#include "iris/reference_counted.h"
#include "iris/reflectors/mock_reflector.h"
#include "iris/testing/spectral_allocator.h"
#include "iris/texture_coordinates.h"

namespace iris {
namespace textures {
namespace {

using ::iris::reflectors::MockReflector;
using ::iris::testing::GetSpectralAllocator;

TEST(ConstantFloatTexture, Test) {
  EXPECT_FALSE(MakeConstantTexture(0.0));
  EXPECT_EQ(1.0, MakeConstantTexture(1.0)->Evaluate(
                     TextureCoordinates{Point(0.0, 0.0, 0.0),
                                        Vector(0.0, 0.0, 0.0),
                                        Vector(0.0, 0.0, 0.0),
                                        {0.0, 0.0},
                                        0.0,
                                        0.0,
                                        0.0,
                                        0.0}));
}

TEST(ConstantReflectorTexture, Test) {
  EXPECT_FALSE(MakeConstantTexture(ReferenceCounted<Reflector>()));
  ReferenceCounted<Reflector> value = MakeReferenceCounted<MockReflector>();
  EXPECT_EQ(value.Get(), MakeConstantTexture(value)->Evaluate(
                             TextureCoordinates{Point(0.0, 0.0, 0.0),
                                                Vector(0.0, 0.0, 0.0),
                                                Vector(0.0, 0.0, 0.0),
                                                {0.0, 0.0},
                                                0.0,
                                                0.0,
                                                0.0,
                                                0.0},
                             GetSpectralAllocator()));
}

}  // namespace
}  // namespace textures
}  // namespace iris
