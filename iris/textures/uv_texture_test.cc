#include "iris/textures/uv_texture.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/float.h"
#include "iris/reflectors/mock_reflector.h"
#include "iris/texture_coordinates.h"

namespace iris {
namespace textures {
namespace {

class TestUVPointerTexture2D : public UVPointerTexture2D<Reflector> {
 public:
  TestUVPointerTexture2D(const Reflector& reflector)
      : UVPointerTexture2D(
            static_cast<geometric>(1.0), static_cast<geometric>(2.0),
            static_cast<geometric>(3.0), static_cast<geometric>(4.0)),
        reflector_(reflector) {}

 protected:
  const Reflector* NestedEvaluate(
      const TextureCoordinates& coordinates) const override {
    EXPECT_EQ(4.0, coordinates.uv[0]);
    EXPECT_EQ(8.0, coordinates.uv[1]);
    return &reflector_;
  }

 private:
  const Reflector& reflector_;
};

class TestUVValueTexture2D : public UVValueTexture2D<float> {
 public:
  TestUVValueTexture2D(float value)
      : UVValueTexture2D(
            static_cast<geometric>(1.0), static_cast<geometric>(2.0),
            static_cast<geometric>(3.0), static_cast<geometric>(4.0)),
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

TEST(UVPointerTexture2D, Test) {
  reflectors::MockReflector reflector;
  TestUVPointerTexture2D texture(reflector);
  EXPECT_EQ(&reflector, texture.Evaluate(TextureCoordinates{{1.0, 2.0}}));
}

TEST(UVValueTexture2D, Test) {
  TestUVValueTexture2D texture(1.0);
  EXPECT_EQ(1.0, texture.Evaluate(TextureCoordinates{{1.0, 2.0}}));
}

}  // namespace
}  // namespace textures
}  // namespace iris
