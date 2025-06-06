#include "iris/textures/image_texture.h"

#include <memory>
#include <optional>
#include <utility>
#include <vector>

#include "googletest/include/gtest/gtest.h"
#include "iris/reference_counted.h"
#include "iris/reflectors/mock_reflector.h"
#include "iris/texture_coordinates.h"

namespace iris {
namespace textures {
namespace {

using ::iris::reflectors::MockReflector;

TEST(Image2D, Size) {
  std::vector<float> values = {1.0, 2.0};
  Image2D<float> image(values, {2, 1});
  EXPECT_EQ(2u, image.Size().first);
  EXPECT_EQ(1u, image.Size().second);
}

TEST(Image2D, Get) {
  std::vector<float> values = {1.0, 2.0, 3.0, 4.0};
  Image2D<float> image(values, {2, 2});
  EXPECT_EQ(1.0, image.Get(0u, 0u));
  EXPECT_EQ(2.0, image.Get(0u, 1u));
  EXPECT_EQ(3.0, image.Get(1u, 0u));
  EXPECT_EQ(4.0, image.Get(1u, 1u));
}

TEST(Image2D, GetBordered) {
  std::vector<float> values = {1.0};
  Image2D<float> image(values, {1, 1});
  EXPECT_EQ(1.0, image.GetBordered(0.5, 0.5, 2.0));
  EXPECT_EQ(2.0, image.GetBordered(-0.5, 0.5, 2.0));
  EXPECT_EQ(2.0, image.GetBordered(1.5, 0.5, 2.0));
  EXPECT_EQ(2.0, image.GetBordered(0.5, -0.5, 2.0));
  EXPECT_EQ(2.0, image.GetBordered(0.5, 1.5, 2.0));
}

TEST(Image2D, GetClamped) {
  std::vector<float> values = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0};
  Image2D<float> image(values, {3, 3});
  EXPECT_EQ(1.0, image.GetClamped(-0.5, -0.5));
  EXPECT_EQ(2.0, image.GetClamped(0.5, -0.5));
  EXPECT_EQ(3.0, image.GetClamped(1.5, -0.5));
  EXPECT_EQ(4.0, image.GetClamped(-0.5, 0.5));
  EXPECT_EQ(5.0, image.GetClamped(0.5, 0.5));
  EXPECT_EQ(6.0, image.GetClamped(1.5, 0.5));
  EXPECT_EQ(7.0, image.GetClamped(-0.5, 1.5));
  EXPECT_EQ(8.0, image.GetClamped(0.5, 1.5));
  EXPECT_EQ(9.0, image.GetClamped(1.5, 1.5));
}

TEST(Image2D, GetRepeated) {
  std::vector<float> values = {1.0, 2.0, 3.0, 4.0};
  Image2D<float> image(values, {2, 2});
  EXPECT_EQ(1.0, image.GetRepeated(0.0, 0.0));
  EXPECT_EQ(1.0, image.GetRepeated(1.0, 1.0));
  EXPECT_EQ(1.0, image.GetRepeated(-0.75, 0.25));
  EXPECT_EQ(2.0, image.GetRepeated(-0.25, 0.25));
  EXPECT_EQ(1.0, image.GetRepeated(0.25, 0.25));
  EXPECT_EQ(2.0, image.GetRepeated(0.75, 0.25));
  EXPECT_EQ(1.0, image.GetRepeated(1.25, 0.25));
  EXPECT_EQ(2.0, image.GetRepeated(1.75, 0.25));
  EXPECT_EQ(1.0, image.GetRepeated(0.25, -0.75));
  EXPECT_EQ(3.0, image.GetRepeated(0.25, -0.25));
  EXPECT_EQ(1.0, image.GetRepeated(0.25, 0.25));
  EXPECT_EQ(3.0, image.GetRepeated(0.25, 0.75));
  EXPECT_EQ(1.0, image.GetRepeated(0.25, 1.25));
  EXPECT_EQ(3.0, image.GetRepeated(0.25, 1.75));
}

TEST(Image2D, ComputeSampleCoordinates) {
  std::vector<float> values = {1.0, 1.0, 2.0, 2.0, 1.0, 1.0, 2.0, 2.0};
  Image2D<float> image(values, {2, 4});

  Image2D<float>::SampleCoordinates result0 =
      image.ComputeSampleCoordinates(0.0, 0.0);
  EXPECT_EQ(result0.low_coordinates[0][0], -0.125);
  EXPECT_EQ(result0.low_coordinates[0][1], -0.25);
  EXPECT_EQ(result0.low_coordinates[1][0], 0.125);
  EXPECT_EQ(result0.low_coordinates[1][1], -0.25);
  EXPECT_EQ(result0.high_coordinates[0][0], -0.125);
  EXPECT_EQ(result0.high_coordinates[0][1], 0.25);
  EXPECT_EQ(result0.high_coordinates[1][0], 0.125);
  EXPECT_EQ(result0.high_coordinates[1][1], 0.25);
  EXPECT_EQ(result0.left_right_interpolation, 0.5);
  EXPECT_EQ(result0.low_high_interpolation, 0.5);

  Image2D<float>::SampleCoordinates result1 =
      image.ComputeSampleCoordinates(0.5, 0.5);
  EXPECT_EQ(result1.low_coordinates[0][0], 0.375);
  EXPECT_EQ(result1.low_coordinates[0][1], 0.25);
  EXPECT_EQ(result1.low_coordinates[1][0], 0.625);
  EXPECT_EQ(result1.low_coordinates[1][1], 0.25);
  EXPECT_EQ(result1.high_coordinates[0][0], 0.375);
  EXPECT_EQ(result1.high_coordinates[0][1], 0.75);
  EXPECT_EQ(result1.high_coordinates[1][0], 0.625);
  EXPECT_EQ(result1.high_coordinates[1][1], 0.75);
  EXPECT_EQ(result1.left_right_interpolation, 0.5);
  EXPECT_EQ(result1.low_high_interpolation, 0.5);

  Image2D<float>::SampleCoordinates result2 =
      image.ComputeSampleCoordinates(1.5, 0.5);
  EXPECT_EQ(result2.low_coordinates[0][0], 1.375);
  EXPECT_EQ(result2.low_coordinates[0][1], 0.25);
  EXPECT_EQ(result2.low_coordinates[1][0], 1.625);
  EXPECT_EQ(result2.low_coordinates[1][1], 0.25);
  EXPECT_EQ(result2.high_coordinates[0][0], 1.375);
  EXPECT_EQ(result2.high_coordinates[0][1], 0.75);
  EXPECT_EQ(result2.high_coordinates[1][0], 1.625);
  EXPECT_EQ(result2.high_coordinates[1][1], 0.75);
  EXPECT_EQ(result2.left_right_interpolation, 0.5);
  EXPECT_EQ(result2.low_high_interpolation, 0.5);

  Image2D<float>::SampleCoordinates result4 =
      image.ComputeSampleCoordinates(0.5, 1.5);
  EXPECT_EQ(result4.low_coordinates[0][0], 0.375);
  EXPECT_EQ(result4.low_coordinates[0][1], 1.25);
  EXPECT_EQ(result4.low_coordinates[1][0], 0.625);
  EXPECT_EQ(result4.low_coordinates[1][1], 1.25);
  EXPECT_EQ(result4.high_coordinates[0][0], 0.375);
  EXPECT_EQ(result4.high_coordinates[0][1], 1.75);
  EXPECT_EQ(result4.high_coordinates[1][0], 0.625);
  EXPECT_EQ(result4.high_coordinates[1][1], 1.75);
  EXPECT_EQ(result4.left_right_interpolation, 0.5);
  EXPECT_EQ(result4.low_high_interpolation, 0.5);

  Image2D<float>::SampleCoordinates result5 =
      image.ComputeSampleCoordinates(0.125, 0.25);
  EXPECT_EQ(result5.low_coordinates[0][0], 0.0);
  EXPECT_EQ(result5.low_coordinates[0][1], 0.0);
  EXPECT_EQ(result5.low_coordinates[1][0], 0.25);
  EXPECT_EQ(result5.low_coordinates[1][1], 0.0);
  EXPECT_EQ(result5.high_coordinates[0][0], 0.0);
  EXPECT_EQ(result5.high_coordinates[0][1], 0.5);
  EXPECT_EQ(result5.high_coordinates[1][0], 0.25);
  EXPECT_EQ(result5.high_coordinates[1][1], 0.5);
  EXPECT_EQ(result5.left_right_interpolation, 0.0);
  EXPECT_EQ(result5.low_high_interpolation, 0.0);

  Image2D<float>::SampleCoordinates result6 =
      image.ComputeSampleCoordinates(1.125, 1.25);
  EXPECT_EQ(result6.low_coordinates[0][0], 1.0);
  EXPECT_EQ(result6.low_coordinates[0][1], 1.0);
  EXPECT_EQ(result6.low_coordinates[1][0], 1.25);
  EXPECT_EQ(result6.low_coordinates[1][1], 1.0);
  EXPECT_EQ(result6.high_coordinates[0][0], 1.0);
  EXPECT_EQ(result6.high_coordinates[0][1], 1.5);
  EXPECT_EQ(result6.high_coordinates[1][0], 1.25);
  EXPECT_EQ(result6.high_coordinates[1][1], 1.5);
  EXPECT_EQ(result6.left_right_interpolation, 0.0);
  EXPECT_EQ(result6.low_high_interpolation, 0.0);

  Image2D<float>::SampleCoordinates result7 =
      image.ComputeSampleCoordinates(-0.875, -0.75);
  EXPECT_EQ(result7.low_coordinates[0][0], -1.0);
  EXPECT_EQ(result7.low_coordinates[0][1], -1.0);
  EXPECT_EQ(result7.low_coordinates[1][0], -0.75);
  EXPECT_EQ(result7.low_coordinates[1][1], -1.0);
  EXPECT_EQ(result7.high_coordinates[0][0], -1.0);
  EXPECT_EQ(result7.high_coordinates[0][1], -0.5);
  EXPECT_EQ(result7.high_coordinates[1][0], -0.75);
  EXPECT_EQ(result7.high_coordinates[1][1], -0.5);
  EXPECT_EQ(result7.left_right_interpolation, 0.0);
  EXPECT_EQ(result7.low_high_interpolation, 0.0);
}

TEST(BorderedSpectralImageTexture2D, Test) {
  std::vector<ReferenceCounted<Reflector>> reflectors;
  reflectors.push_back(MakeReferenceCounted<MockReflector>());
  reflectors.push_back(MakeReferenceCounted<MockReflector>());
  reflectors.push_back(MakeReferenceCounted<MockReflector>());
  reflectors.push_back(MakeReferenceCounted<MockReflector>());
  std::shared_ptr<Image2D<ReferenceCounted<Reflector>>> image =
      std::make_shared<Image2D<ReferenceCounted<Reflector>>>(
          reflectors, std::pair<size_t, size_t>(2, 2));

  BorderedSpectralImageTexture2D<Reflector> texture(
      image, ReferenceCounted<Reflector>(), std::nullopt, std::nullopt,
      std::nullopt, std::nullopt);
  EXPECT_EQ(reflectors[0].Get(),
            texture.Evaluate(TextureCoordinates{{0.25, 0.25}}));
  EXPECT_EQ(nullptr, texture.Evaluate(TextureCoordinates{{-0.25, 0.25}}));
}

TEST(ClampedSpectralImageTexture2D, Test) {
  std::vector<ReferenceCounted<Reflector>> reflectors;
  reflectors.push_back(MakeReferenceCounted<MockReflector>());
  reflectors.push_back(MakeReferenceCounted<MockReflector>());
  reflectors.push_back(MakeReferenceCounted<MockReflector>());
  reflectors.push_back(MakeReferenceCounted<MockReflector>());
  std::shared_ptr<Image2D<ReferenceCounted<Reflector>>> image =
      std::make_shared<Image2D<ReferenceCounted<Reflector>>>(
          reflectors, std::pair<size_t, size_t>(2, 2));

  ClampedSpectralImageTexture2D<Reflector> texture(
      image, std::nullopt, std::nullopt, std::nullopt, std::nullopt);
  EXPECT_EQ(reflectors[3].Get(),
            texture.Evaluate(TextureCoordinates{{1.25, 1.25}}));
}

TEST(RepeatedSpectralImageTexture2D, Test) {
  std::vector<ReferenceCounted<Reflector>> reflectors;
  reflectors.push_back(MakeReferenceCounted<MockReflector>());
  reflectors.push_back(MakeReferenceCounted<MockReflector>());
  reflectors.push_back(MakeReferenceCounted<MockReflector>());
  reflectors.push_back(MakeReferenceCounted<MockReflector>());
  std::shared_ptr<Image2D<ReferenceCounted<Reflector>>> image =
      std::make_shared<Image2D<ReferenceCounted<Reflector>>>(
          reflectors, std::pair<size_t, size_t>(2, 2));

  RepeatedSpectralImageTexture2D<Reflector> texture(
      image, std::nullopt, std::nullopt, std::nullopt, std::nullopt);
  EXPECT_EQ(reflectors[0].Get(),
            texture.Evaluate(TextureCoordinates{{1.25, 1.25}}));
}

TEST(BorderedImageTexture2D, Test) {
  std::vector<float> reflectors;
  reflectors.push_back(1.0);
  reflectors.push_back(2.0);
  reflectors.push_back(3.0);
  reflectors.push_back(4.0);
  std::shared_ptr<Image2D<float>> image = std::make_shared<Image2D<float>>(
      reflectors, std::pair<size_t, size_t>(2, 2));

  BorderedImageTexture2D<float> texture(image, 0.0, std::nullopt, std::nullopt,
                                        std::nullopt, std::nullopt);
  EXPECT_EQ(reflectors[0], texture.Evaluate(TextureCoordinates{{0.25, 0.25}}));
  EXPECT_EQ(0.0, texture.Evaluate(TextureCoordinates{{-0.25, 0.25}}));
  EXPECT_EQ(0.25, texture.Evaluate(TextureCoordinates{{0.0, 0.0}}));
  EXPECT_EQ(2.5, texture.Evaluate(TextureCoordinates{{0.5, 0.5}}));
  EXPECT_EQ(1.0, texture.Evaluate(TextureCoordinates{{1.0, 1.0}}));
}

TEST(ClampedImageTexture2D, Test) {
  std::vector<float> reflectors;
  reflectors.push_back(1.0);
  reflectors.push_back(2.0);
  reflectors.push_back(3.0);
  reflectors.push_back(4.0);
  std::shared_ptr<Image2D<float>> image = std::make_shared<Image2D<float>>(
      reflectors, std::pair<size_t, size_t>(2, 2));

  ClampedImageTexture2D<float> texture(image, std::nullopt, std::nullopt,
                                       std::nullopt, std::nullopt);
  EXPECT_EQ(reflectors[3], texture.Evaluate(TextureCoordinates{{1.25, 1.25}}));
  EXPECT_EQ(1.0, texture.Evaluate(TextureCoordinates{{0.0, 0.0}}));
  EXPECT_EQ(2.5, texture.Evaluate(TextureCoordinates{{0.5, 0.5}}));
  EXPECT_EQ(4.0, texture.Evaluate(TextureCoordinates{{1.0, 1.0}}));
}

TEST(RepeatedImageTexture2D, Test) {
  std::vector<float> reflectors;
  reflectors.push_back(1.0);
  reflectors.push_back(2.0);
  reflectors.push_back(3.0);
  reflectors.push_back(4.0);
  std::shared_ptr<Image2D<float>> image = std::make_shared<Image2D<float>>(
      reflectors, std::pair<size_t, size_t>(2, 2));

  RepeatedImageTexture2D<float> texture(image, std::nullopt, std::nullopt,
                                        std::nullopt, std::nullopt);
  EXPECT_EQ(reflectors[0], texture.Evaluate(TextureCoordinates{{1.25, 1.25}}));
  EXPECT_EQ(2.5, texture.Evaluate(TextureCoordinates{{0.0, 0.0}}));
  EXPECT_EQ(2.5, texture.Evaluate(TextureCoordinates{{0.5, 0.5}}));
  EXPECT_EQ(2.5, texture.Evaluate(TextureCoordinates{{1.0, 1.0}}));
  EXPECT_EQ(2.5, texture.Evaluate(TextureCoordinates{{1.5, 1.5}}));
  EXPECT_EQ(2.5, texture.Evaluate(TextureCoordinates{{2.0, 2.0}}));
}

}  // namespace
}  // namespace textures
}  // namespace iris
