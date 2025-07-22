#include "iris/textures/internal/uv_texture.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/float.h"
#include "iris/point.h"
#include "iris/reflector.h"
#include "iris/reflectors/mock_reflector.h"
#include "iris/spectral_allocator.h"
#include "iris/testing/spectral_allocator.h"
#include "iris/texture_coordinates.h"
#include "iris/textures/float_texture.h"
#include "iris/textures/reflector_texture.h"

namespace iris {
namespace textures {
namespace internal {
namespace {

using ::iris::testing::GetSpectralAllocator;

class TestUVValueTexture2D : public UVFloatTexture {
 public:
  TestUVValueTexture2D(float value)
      : UVFloatTexture(static_cast<geometric>(1.0), static_cast<geometric>(2.0),
                       static_cast<geometric>(3.0),
                       static_cast<geometric>(4.0)),
        value_(value) {}

 protected:
  float NestedEvaluate(const TextureCoordinates& coordinates) const override {
    EXPECT_EQ(4.0, coordinates.uv[0]);
    EXPECT_EQ(8.0, coordinates.uv[1]);
    return value_;
  }

 private:
  float value_;
};

class TestUVPointerTexture2D : public UVReflectorTexture {
 public:
  TestUVPointerTexture2D(const Reflector& reflector)
      : UVReflectorTexture(
            static_cast<geometric>(1.0), static_cast<geometric>(2.0),
            static_cast<geometric>(3.0), static_cast<geometric>(4.0)),
        reflector_(reflector) {}

 protected:
  const Reflector* NestedEvaluate(const TextureCoordinates& coordinates,
                                  SpectralAllocator& allocator) const override {
    EXPECT_EQ(4.0, coordinates.uv[0]);
    EXPECT_EQ(8.0, coordinates.uv[1]);
    return &reflector_;
  }

 private:
  const Reflector& reflector_;
};

TEST(UVPointerTexture2D, Test) {
  reflectors::MockReflector reflector;
  TestUVPointerTexture2D texture(reflector);
  EXPECT_EQ(
      &reflector,
      texture.Evaluate(TextureCoordinates{Point(0.0, 0.0, 0.0), {}, {1.0, 2.0}},
                       GetSpectralAllocator()));
}

TEST(UVValueTexture2D, Test) {
  TestUVValueTexture2D texture(1.0);
  EXPECT_EQ(1.0, texture.Evaluate(
                     TextureCoordinates{Point(0.0, 0.0, 0.0), {}, {1.0, 2.0}}));
}

}  // namespace
}  // namespace internal
}  // namespace textures
}  // namespace iris
