#include "iris/textures/image_texture.h"

#include <vector>

#include "googletest/include/gtest/gtest.h"
#include "iris/reflectors/mock_reflector.h"

TEST(Image2D, Size) {
  std::vector<float> values = {1.0, 2.0};
  iris::textures::Image2D<float> image(values, {2, 1});
  EXPECT_EQ(2u, image.Size().first);
  EXPECT_EQ(1u, image.Size().second);
}

TEST(Image2D, Get) {
  std::vector<float> values = {1.0, 2.0, 3.0, 4.0};
  iris::textures::Image2D<float> image(values, {2, 2});
  EXPECT_EQ(1.0, image.Get(0u, 0u));
  EXPECT_EQ(2.0, image.Get(0u, 1u));
  EXPECT_EQ(3.0, image.Get(1u, 0u));
  EXPECT_EQ(4.0, image.Get(1u, 1u));
}

TEST(Image2D, GetBordered) {
  std::vector<float> values = {1.0};
  iris::textures::Image2D<float> image(values, {1, 1});
  EXPECT_EQ(1.0, image.GetBordered(0.5, 0.5, 2.0));
  EXPECT_EQ(2.0, image.GetBordered(-0.5, 0.5, 2.0));
  EXPECT_EQ(2.0, image.GetBordered(1.5, 0.5, 2.0));
  EXPECT_EQ(2.0, image.GetBordered(0.5, -0.5, 2.0));
  EXPECT_EQ(2.0, image.GetBordered(0.5, 1.5, 2.0));
}

TEST(Image2D, GetClamped) {
  std::vector<float> values = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0};
  iris::textures::Image2D<float> image(values, {3, 3});
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
  iris::textures::Image2D<float> image(values, {2, 2});
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

TEST(BorderedSpectralImageTexture2D, Test) {
  std::vector<iris::ReferenceCounted<iris::Reflector>> reflectors;
  reflectors.push_back(
      iris::MakeReferenceCounted<iris::reflectors::MockReflector>());
  reflectors.push_back(
      iris::MakeReferenceCounted<iris::reflectors::MockReflector>());
  reflectors.push_back(
      iris::MakeReferenceCounted<iris::reflectors::MockReflector>());
  reflectors.push_back(
      iris::MakeReferenceCounted<iris::reflectors::MockReflector>());
  auto image = std::make_shared<
      iris::textures::Image2D<iris::ReferenceCounted<iris::Reflector>>>(
      reflectors, std::pair<size_t, size_t>(2, 2));

  iris::textures::BorderedSpectralImageTexture2D<iris::Reflector> texture(
      image, iris::ReferenceCounted<iris::Reflector>(), std::nullopt,
      std::nullopt, std::nullopt, std::nullopt);
  EXPECT_EQ(reflectors[0].Get(),
            texture.Evaluate(iris::TextureCoordinates{{0.25, 0.25}}));
  EXPECT_EQ(nullptr, texture.Evaluate(iris::TextureCoordinates{{-0.25, 0.25}}));
}

TEST(ClampedSpectralImageTexture2D, Test) {
  std::vector<iris::ReferenceCounted<iris::Reflector>> reflectors;
  reflectors.push_back(
      iris::MakeReferenceCounted<iris::reflectors::MockReflector>());
  reflectors.push_back(
      iris::MakeReferenceCounted<iris::reflectors::MockReflector>());
  reflectors.push_back(
      iris::MakeReferenceCounted<iris::reflectors::MockReflector>());
  reflectors.push_back(
      iris::MakeReferenceCounted<iris::reflectors::MockReflector>());
  auto image = std::make_shared<
      iris::textures::Image2D<iris::ReferenceCounted<iris::Reflector>>>(
      reflectors, std::pair<size_t, size_t>(2, 2));

  iris::textures::ClampedSpectralImageTexture2D<iris::Reflector> texture(
      image, std::nullopt, std::nullopt, std::nullopt, std::nullopt);
  EXPECT_EQ(reflectors[3].Get(),
            texture.Evaluate(iris::TextureCoordinates{{1.25, 1.25}}));
}

TEST(RepeatedSpectralImageTexture2D, Test) {
  std::vector<iris::ReferenceCounted<iris::Reflector>> reflectors;
  reflectors.push_back(
      iris::MakeReferenceCounted<iris::reflectors::MockReflector>());
  reflectors.push_back(
      iris::MakeReferenceCounted<iris::reflectors::MockReflector>());
  reflectors.push_back(
      iris::MakeReferenceCounted<iris::reflectors::MockReflector>());
  reflectors.push_back(
      iris::MakeReferenceCounted<iris::reflectors::MockReflector>());
  auto image = std::make_shared<
      iris::textures::Image2D<iris::ReferenceCounted<iris::Reflector>>>(
      reflectors, std::pair<size_t, size_t>(2, 2));

  iris::textures::RepeatedSpectralImageTexture2D<iris::Reflector> texture(
      image, std::nullopt, std::nullopt, std::nullopt, std::nullopt);
  EXPECT_EQ(reflectors[0].Get(),
            texture.Evaluate(iris::TextureCoordinates{{1.25, 1.25}}));
}

TEST(BorderedImageTexture2D, Test) {
  std::vector<float> reflectors;
  reflectors.push_back(1.0);
  reflectors.push_back(2.0);
  reflectors.push_back(3.0);
  reflectors.push_back(4.0);
  auto image = std::make_shared<iris::textures::Image2D<float>>(
      reflectors, std::pair<size_t, size_t>(2, 2));

  iris::textures::BorderedImageTexture2D<float> texture(
      image, 0.0, std::nullopt, std::nullopt, std::nullopt, std::nullopt);
  EXPECT_EQ(reflectors[0],
            texture.Evaluate(iris::TextureCoordinates{{0.25, 0.25}}));
  EXPECT_EQ(0.0, texture.Evaluate(iris::TextureCoordinates{{-0.25, 0.25}}));
}

TEST(ClampedImageTexture2D, Test) {
  std::vector<float> reflectors;
  reflectors.push_back(1.0);
  reflectors.push_back(2.0);
  reflectors.push_back(3.0);
  reflectors.push_back(4.0);
  auto image = std::make_shared<iris::textures::Image2D<float>>(
      reflectors, std::pair<size_t, size_t>(2, 2));

  iris::textures::ClampedImageTexture2D<float> texture(
      image, std::nullopt, std::nullopt, std::nullopt, std::nullopt);
  EXPECT_EQ(reflectors[3],
            texture.Evaluate(iris::TextureCoordinates{{1.25, 1.25}}));
}

TEST(RepeatedImageTexture2D, Test) {
  std::vector<float> reflectors;
  reflectors.push_back(1.0);
  reflectors.push_back(2.0);
  reflectors.push_back(3.0);
  reflectors.push_back(4.0);
  auto image = std::make_shared<iris::textures::Image2D<float>>(
      reflectors, std::pair<size_t, size_t>(2, 2));

  iris::textures::RepeatedImageTexture2D<float> texture(
      image, std::nullopt, std::nullopt, std::nullopt, std::nullopt);
  EXPECT_EQ(reflectors[0],
            texture.Evaluate(iris::TextureCoordinates{{1.25, 1.25}}));
}