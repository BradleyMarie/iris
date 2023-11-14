#include "iris/textures/uv_texture.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/reflectors/mock_reflector.h"

class TestUVPointerTexture2D
    : public iris::textures::UVPointerTexture2D<iris::Reflector> {
 public:
  TestUVPointerTexture2D(const iris::Reflector& reflector)
      : UVPointerTexture2D(static_cast<iris::geometric>(1.0),
                           static_cast<iris::geometric>(2.0),
                           static_cast<iris::geometric>(3.0),
                           static_cast<iris::geometric>(4.0)),
        reflector_(reflector) {}

 protected:
  const iris::Reflector* NestedEvaluate(
      const iris::TextureCoordinates& coordinates) const override {
    EXPECT_EQ(4.0, coordinates.uv[0]);
    EXPECT_EQ(8.0, coordinates.uv[1]);
    return &reflector_;
  }

 private:
  const iris::Reflector& reflector_;
};

class TestUVValueTexture2D : public iris::textures::UVValueTexture2D<float> {
 public:
  TestUVValueTexture2D(float value)
      : UVValueTexture2D(static_cast<iris::geometric>(1.0),
                         static_cast<iris::geometric>(2.0),
                         static_cast<iris::geometric>(3.0),
                         static_cast<iris::geometric>(4.0)),
        value_(value) {}

 protected:
  float NestedEvaluate(
      const iris::TextureCoordinates& coordinates) const override {
    EXPECT_EQ(4.0, coordinates.uv[0]);
    EXPECT_EQ(8.0, coordinates.uv[1]);
    return value_;
  }

 private:
  float value_;
};

TEST(UVPointerTexture2D, Test) {
  iris::reflectors::MockReflector reflector;
  TestUVPointerTexture2D texture(reflector);
  EXPECT_EQ(&reflector, texture.Evaluate(iris::TextureCoordinates{{1.0, 2.0}}));
}

TEST(UVValueTexture2D, Test) {
  TestUVValueTexture2D texture(1.0);
  EXPECT_EQ(1.0, texture.Evaluate(iris::TextureCoordinates{{1.0, 2.0}}));
}