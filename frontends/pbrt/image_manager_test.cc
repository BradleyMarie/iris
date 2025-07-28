#include "frontends/pbrt/image_manager.h"

#include <filesystem>
#include <memory>
#include <set>
#include <unordered_map>

#include "frontends/pbrt/spectrum_managers/color_spectrum_manager.h"
#include "frontends/pbrt/texture_manager.h"
#include "googletest/include/gtest/gtest.h"
#include "iris/reference_counted.h"
#include "iris/reflector.h"
#include "iris/textures/image_texture.h"
#include "tools/cpp/runfiles/runfiles.h"

namespace iris {
namespace pbrt_frontend {
namespace {

using ::bazel::tools::cpp::runfiles::Runfiles;
using ::iris::pbrt_frontend::spectrum_managers::ColorSpectrumManager;
using ::iris::textures::Image2D;
using ::testing::ExitedWithCode;

std::string RawRunfilePath(const std::string& path) {
  std::unique_ptr<Runfiles> runfiles(Runfiles::CreateForTest());
  const char* base_path = "_main/frontends/pbrt/test_data/";
  return runfiles->Rlocation(base_path + path);
}

TEST(ImageManager, MissingFile) {
  std::filesystem::path path = std::filesystem::current_path();
  ColorSpectrumManager spectrum_manager(path, true);
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(std::filesystem::current_path(), texture_manager,
                             spectrum_manager);

  EXPECT_EXIT(image_manager.LoadFloatImageFromSDR("notarealfile", true),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Failed to load image: notarealfile");
  EXPECT_EXIT(image_manager.LoadReflectorImageFromSDR("notarealfile", true),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Failed to load image: notarealfile");
}

TEST(ImageManager, FloatImageSizesCorrect) {
  std::filesystem::path path = std::filesystem::current_path();
  ColorSpectrumManager spectrum_manager(path, true);
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(std::filesystem::current_path(), texture_manager,
                             spectrum_manager);

  std::set<std::shared_ptr<Image2D<visual>>> images;
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadFloatImageFromSDR(
                      RawRunfilePath("g8.png"), true))
                  .second);
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadFloatImageFromSDR(
                      RawRunfilePath("g16.png"), true))
                  .second);
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadFloatImageFromSDR(
                      RawRunfilePath("ga8.png"), true))
                  .second);
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadFloatImageFromSDR(
                      RawRunfilePath("ga16.png"), true))
                  .second);
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadFloatImageFromSDR(
                      RawRunfilePath("rgb8.png"), true))
                  .second);
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadFloatImageFromSDR(
                      RawRunfilePath("rgb16.png"), true))
                  .second);
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadFloatImageFromSDR(
                      RawRunfilePath("rgba8.png"), true))
                  .second);
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadFloatImageFromSDR(
                      RawRunfilePath("rgba16.png"), true))
                  .second);
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadFloatImageFromSDR(
                      RawRunfilePath("g8.png"), false))
                  .second);
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadFloatImageFromSDR(
                      RawRunfilePath("g16.png"), false))
                  .second);
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadFloatImageFromSDR(
                      RawRunfilePath("ga8.png"), false))
                  .second);
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadFloatImageFromSDR(
                      RawRunfilePath("ga16.png"), false))
                  .second);
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadFloatImageFromSDR(
                      RawRunfilePath("rgb8.png"), false))
                  .second);
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadFloatImageFromSDR(
                      RawRunfilePath("rgb16.png"), false))
                  .second);
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadFloatImageFromSDR(
                      RawRunfilePath("rgba8.png"), false))
                  .second);
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadFloatImageFromSDR(
                      RawRunfilePath("rgba16.png"), false))
                  .second);

  // HDR
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadFloatImageFromHDR(
                      RawRunfilePath("rgbw.exr"), true))
                  .second);
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadFloatImageFromHDR(
                      RawRunfilePath("rgbw.exr"), false))
                  .second);

  for (const auto& image : images) {
    auto size = image->Size();
    EXPECT_EQ(size.first, 2u);
    EXPECT_EQ(size.second, 2u);
  }
}

TEST(ImageManager, ReflectorImageSizesCorrect) {
  std::filesystem::path path = std::filesystem::current_path();
  ColorSpectrumManager spectrum_manager(path, true);
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(std::filesystem::current_path(), texture_manager,
                             spectrum_manager);

  std::set<std::shared_ptr<Image2D<ReferenceCounted<Reflector>>>> images;
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadReflectorImageFromSDR(
                      RawRunfilePath("g8.png"), true))
                  .second);
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadReflectorImageFromSDR(
                      RawRunfilePath("g16.png"), true))
                  .second);
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadReflectorImageFromSDR(
                      RawRunfilePath("ga8.png"), true))
                  .second);
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadReflectorImageFromSDR(
                      RawRunfilePath("ga16.png"), true))
                  .second);
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadReflectorImageFromSDR(
                      RawRunfilePath("rgb8.png"), true))
                  .second);
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadReflectorImageFromSDR(
                      RawRunfilePath("rgb16.png"), true))
                  .second);
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadReflectorImageFromSDR(
                      RawRunfilePath("rgba8.png"), true))
                  .second);
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadReflectorImageFromSDR(
                      RawRunfilePath("rgba16.png"), true))
                  .second);
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadReflectorImageFromSDR(
                      RawRunfilePath("g8.png"), false))
                  .second);
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadReflectorImageFromSDR(
                      RawRunfilePath("g16.png"), false))
                  .second);
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadReflectorImageFromSDR(
                      RawRunfilePath("ga8.png"), false))
                  .second);
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadReflectorImageFromSDR(
                      RawRunfilePath("ga16.png"), false))
                  .second);
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadReflectorImageFromSDR(
                      RawRunfilePath("rgb8.png"), false))
                  .second);
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadReflectorImageFromSDR(
                      RawRunfilePath("rgb16.png"), false))
                  .second);
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadReflectorImageFromSDR(
                      RawRunfilePath("rgba8.png"), false))
                  .second);
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadReflectorImageFromSDR(
                      RawRunfilePath("rgba16.png"), false))
                  .second);

  // HDR
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadReflectorImageFromHDR(
                      RawRunfilePath("rgbw.exr"), true))
                  .second);
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadReflectorImageFromHDR(
                      RawRunfilePath("rgbw.exr"), false))
                  .second);

  for (const auto& image : images) {
    auto size = image->Size();
    EXPECT_EQ(size.first, 2u);
    EXPECT_EQ(size.second, 2u);
  }
}

TEST(ImageManager, ReusesFloatImages) {
  std::filesystem::path path = std::filesystem::current_path();
  ColorSpectrumManager spectrum_manager(path, true);
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(std::filesystem::current_path(), texture_manager,
                             spectrum_manager);

  std::set<std::shared_ptr<Image2D<visual>>> images;
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadFloatImageFromSDR(
                      RawRunfilePath("g8.png"), true))
                  .second);
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadFloatImageFromSDR(
                      RawRunfilePath("g16.png"), true))
                  .second);
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadFloatImageFromSDR(
                      RawRunfilePath("ga8.png"), true))
                  .second);
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadFloatImageFromSDR(
                      RawRunfilePath("ga16.png"), true))
                  .second);
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadFloatImageFromSDR(
                      RawRunfilePath("rgb8.png"), true))
                  .second);
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadFloatImageFromSDR(
                      RawRunfilePath("rgb16.png"), true))
                  .second);
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadFloatImageFromSDR(
                      RawRunfilePath("rgba8.png"), true))
                  .second);
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadFloatImageFromSDR(
                      RawRunfilePath("rgba16.png"), true))
                  .second);
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadFloatImageFromSDR(
                      RawRunfilePath("g8.png"), false))
                  .second);
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadFloatImageFromSDR(
                      RawRunfilePath("g16.png"), false))
                  .second);
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadFloatImageFromSDR(
                      RawRunfilePath("ga8.png"), false))
                  .second);
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadFloatImageFromSDR(
                      RawRunfilePath("ga16.png"), false))
                  .second);
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadFloatImageFromSDR(
                      RawRunfilePath("rgb8.png"), false))
                  .second);
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadFloatImageFromSDR(
                      RawRunfilePath("rgb16.png"), false))
                  .second);
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadFloatImageFromSDR(
                      RawRunfilePath("rgba8.png"), false))
                  .second);
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadFloatImageFromSDR(
                      RawRunfilePath("rgba16.png"), false))
                  .second);
  EXPECT_FALSE(images
                   .emplace(image_manager.LoadFloatImageFromSDR(
                       RawRunfilePath("g8.png"), true))
                   .second);
  EXPECT_FALSE(images
                   .emplace(image_manager.LoadFloatImageFromSDR(
                       RawRunfilePath("g16.png"), true))
                   .second);
  EXPECT_FALSE(images
                   .emplace(image_manager.LoadFloatImageFromSDR(
                       RawRunfilePath("ga8.png"), true))
                   .second);
  EXPECT_FALSE(images
                   .emplace(image_manager.LoadFloatImageFromSDR(
                       RawRunfilePath("ga16.png"), true))
                   .second);
  EXPECT_FALSE(images
                   .emplace(image_manager.LoadFloatImageFromSDR(
                       RawRunfilePath("rgb8.png"), true))
                   .second);
  EXPECT_FALSE(images
                   .emplace(image_manager.LoadFloatImageFromSDR(
                       RawRunfilePath("rgb16.png"), true))
                   .second);
  EXPECT_FALSE(images
                   .emplace(image_manager.LoadFloatImageFromSDR(
                       RawRunfilePath("rgba8.png"), true))
                   .second);
  EXPECT_FALSE(images
                   .emplace(image_manager.LoadFloatImageFromSDR(
                       RawRunfilePath("rgba16.png"), true))
                   .second);
  EXPECT_FALSE(images
                   .emplace(image_manager.LoadFloatImageFromSDR(
                       RawRunfilePath("g8.png"), false))
                   .second);
  EXPECT_FALSE(images
                   .emplace(image_manager.LoadFloatImageFromSDR(
                       RawRunfilePath("g16.png"), false))
                   .second);
  EXPECT_FALSE(images
                   .emplace(image_manager.LoadFloatImageFromSDR(
                       RawRunfilePath("ga8.png"), false))
                   .second);
  EXPECT_FALSE(images
                   .emplace(image_manager.LoadFloatImageFromSDR(
                       RawRunfilePath("ga16.png"), false))
                   .second);
  EXPECT_FALSE(images
                   .emplace(image_manager.LoadFloatImageFromSDR(
                       RawRunfilePath("rgb8.png"), false))
                   .second);
  EXPECT_FALSE(images
                   .emplace(image_manager.LoadFloatImageFromSDR(
                       RawRunfilePath("rgb16.png"), false))
                   .second);
  EXPECT_FALSE(images
                   .emplace(image_manager.LoadFloatImageFromSDR(
                       RawRunfilePath("rgba8.png"), false))
                   .second);
  EXPECT_FALSE(images
                   .emplace(image_manager.LoadFloatImageFromSDR(
                       RawRunfilePath("rgba16.png"), false))
                   .second);

  // HDR
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadFloatImageFromHDR(
                      RawRunfilePath("rgbw.exr"), true))
                  .second);
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadFloatImageFromHDR(
                      RawRunfilePath("rgbw.exr"), false))
                  .second);
  EXPECT_FALSE(images
                   .emplace(image_manager.LoadFloatImageFromHDR(
                       RawRunfilePath("rgbw.exr"), true))
                   .second);
  EXPECT_FALSE(images
                   .emplace(image_manager.LoadFloatImageFromHDR(
                       RawRunfilePath("rgbw.exr"), false))
                   .second);
}

TEST(ImageManager, ReusesReflectorImages) {
  std::filesystem::path path = std::filesystem::current_path();
  ColorSpectrumManager spectrum_manager(path, true);
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(std::filesystem::current_path(), texture_manager,
                             spectrum_manager);

  std::set<std::shared_ptr<Image2D<ReferenceCounted<Reflector>>>> images;
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadReflectorImageFromSDR(
                      RawRunfilePath("g8.png"), true))
                  .second);
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadReflectorImageFromSDR(
                      RawRunfilePath("g16.png"), true))
                  .second);
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadReflectorImageFromSDR(
                      RawRunfilePath("ga8.png"), true))
                  .second);
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadReflectorImageFromSDR(
                      RawRunfilePath("ga16.png"), true))
                  .second);
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadReflectorImageFromSDR(
                      RawRunfilePath("rgb8.png"), true))
                  .second);
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadReflectorImageFromSDR(
                      RawRunfilePath("rgb16.png"), true))
                  .second);
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadReflectorImageFromSDR(
                      RawRunfilePath("rgba8.png"), true))
                  .second);
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadReflectorImageFromSDR(
                      RawRunfilePath("rgba16.png"), true))
                  .second);
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadReflectorImageFromSDR(
                      RawRunfilePath("g8.png"), false))
                  .second);
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadReflectorImageFromSDR(
                      RawRunfilePath("g16.png"), false))
                  .second);
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadReflectorImageFromSDR(
                      RawRunfilePath("ga8.png"), false))
                  .second);
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadReflectorImageFromSDR(
                      RawRunfilePath("ga16.png"), false))
                  .second);
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadReflectorImageFromSDR(
                      RawRunfilePath("rgb8.png"), false))
                  .second);
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadReflectorImageFromSDR(
                      RawRunfilePath("rgb16.png"), false))
                  .second);
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadReflectorImageFromSDR(
                      RawRunfilePath("rgba8.png"), false))
                  .second);
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadReflectorImageFromSDR(
                      RawRunfilePath("rgba16.png"), false))
                  .second);
  EXPECT_FALSE(images
                   .emplace(image_manager.LoadReflectorImageFromSDR(
                       RawRunfilePath("g8.png"), true))
                   .second);
  EXPECT_FALSE(images
                   .emplace(image_manager.LoadReflectorImageFromSDR(
                       RawRunfilePath("g16.png"), true))
                   .second);
  EXPECT_FALSE(images
                   .emplace(image_manager.LoadReflectorImageFromSDR(
                       RawRunfilePath("ga8.png"), true))
                   .second);
  EXPECT_FALSE(images
                   .emplace(image_manager.LoadReflectorImageFromSDR(
                       RawRunfilePath("ga16.png"), true))
                   .second);
  EXPECT_FALSE(images
                   .emplace(image_manager.LoadReflectorImageFromSDR(
                       RawRunfilePath("rgb8.png"), true))
                   .second);
  EXPECT_FALSE(images
                   .emplace(image_manager.LoadReflectorImageFromSDR(
                       RawRunfilePath("rgb16.png"), true))
                   .second);
  EXPECT_FALSE(images
                   .emplace(image_manager.LoadReflectorImageFromSDR(
                       RawRunfilePath("rgba8.png"), true))
                   .second);
  EXPECT_FALSE(images
                   .emplace(image_manager.LoadReflectorImageFromSDR(
                       RawRunfilePath("rgba16.png"), true))
                   .second);
  EXPECT_FALSE(images
                   .emplace(image_manager.LoadReflectorImageFromSDR(
                       RawRunfilePath("g8.png"), false))
                   .second);
  EXPECT_FALSE(images
                   .emplace(image_manager.LoadReflectorImageFromSDR(
                       RawRunfilePath("g16.png"), false))
                   .second);
  EXPECT_FALSE(images
                   .emplace(image_manager.LoadReflectorImageFromSDR(
                       RawRunfilePath("ga8.png"), false))
                   .second);
  EXPECT_FALSE(images
                   .emplace(image_manager.LoadReflectorImageFromSDR(
                       RawRunfilePath("ga16.png"), false))
                   .second);
  EXPECT_FALSE(images
                   .emplace(image_manager.LoadReflectorImageFromSDR(
                       RawRunfilePath("rgb8.png"), false))
                   .second);
  EXPECT_FALSE(images
                   .emplace(image_manager.LoadReflectorImageFromSDR(
                       RawRunfilePath("rgb16.png"), false))
                   .second);
  EXPECT_FALSE(images
                   .emplace(image_manager.LoadReflectorImageFromSDR(
                       RawRunfilePath("rgba8.png"), false))
                   .second);
  EXPECT_FALSE(images
                   .emplace(image_manager.LoadReflectorImageFromSDR(
                       RawRunfilePath("rgba16.png"), false))
                   .second);

  // HDR
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadReflectorImageFromHDR(
                      RawRunfilePath("rgbw.exr"), true))
                  .second);
  EXPECT_TRUE(images
                  .emplace(image_manager.LoadReflectorImageFromHDR(
                      RawRunfilePath("rgbw.exr"), false))
                  .second);
  EXPECT_FALSE(images
                   .emplace(image_manager.LoadReflectorImageFromHDR(
                       RawRunfilePath("rgbw.exr"), true))
                   .second);
  EXPECT_FALSE(images
                   .emplace(image_manager.LoadReflectorImageFromHDR(
                       RawRunfilePath("rgbw.exr"), false))
                   .second);
}

TEST(ImageManager, LinearFloatValues) {
  std::filesystem::path path = std::filesystem::current_path();
  ColorSpectrumManager spectrum_manager(path, true);
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(std::filesystem::current_path(), texture_manager,
                             spectrum_manager);

  auto g8 =
      image_manager.LoadFloatImageFromSDR(RawRunfilePath("g8.png"), false);
  EXPECT_EQ(1.0, g8->Get(0, 1));

  auto ga8 =
      image_manager.LoadFloatImageFromSDR(RawRunfilePath("ga8.png"), false);
  EXPECT_EQ(1.0, ga8->Get(0, 1));

  auto rgb8 =
      image_manager.LoadFloatImageFromSDR(RawRunfilePath("rgb8.png"), false);
  EXPECT_EQ(1.0, rgb8->Get(0, 1));

  auto rgba8 =
      image_manager.LoadFloatImageFromSDR(RawRunfilePath("rgba8.png"), false);
  EXPECT_EQ(1.0, rgba8->Get(0, 1));

  auto g16 =
      image_manager.LoadFloatImageFromSDR(RawRunfilePath("g16.png"), false);
  EXPECT_EQ(1.0, g16->Get(0, 1));

  auto ga16 =
      image_manager.LoadFloatImageFromSDR(RawRunfilePath("ga16.png"), false);
  EXPECT_EQ(1.0, ga16->Get(0, 1));

  auto rgb16 =
      image_manager.LoadFloatImageFromSDR(RawRunfilePath("rgb16.png"), false);
  EXPECT_EQ(1.0, rgb16->Get(0, 1));

  auto rgba16 =
      image_manager.LoadFloatImageFromSDR(RawRunfilePath("rgba16.png"), false);
  EXPECT_EQ(1.0, rgba16->Get(0, 1));

  // HDR
  auto rgbw =
      image_manager.LoadFloatImageFromHDR(RawRunfilePath("rgbw.exr"), false);
  EXPECT_NEAR(1.0, rgbw->Get(0, 1), 0.001);
}

TEST(ImageManager, LinearReflectorValues) {
  std::filesystem::path path = std::filesystem::current_path();
  ColorSpectrumManager spectrum_manager(path, true);
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(std::filesystem::current_path(), texture_manager,
                             spectrum_manager);

  auto g8 =
      image_manager.LoadReflectorImageFromSDR(RawRunfilePath("g8.png"), false);
  EXPECT_EQ(1.0, g8->Get(0, 1)->Reflectance(0.5));
  EXPECT_EQ(1.0, g8->Get(0, 1)->Reflectance(1.5));
  EXPECT_EQ(1.0, g8->Get(0, 1)->Reflectance(2.5));

  auto ga8 =
      image_manager.LoadReflectorImageFromSDR(RawRunfilePath("ga8.png"), false);
  EXPECT_EQ(1.0, ga8->Get(0, 1)->Reflectance(0.5));
  EXPECT_EQ(1.0, ga8->Get(0, 1)->Reflectance(1.5));
  EXPECT_EQ(1.0, ga8->Get(0, 1)->Reflectance(2.5));

  auto rgb8 = image_manager.LoadReflectorImageFromSDR(
      RawRunfilePath("rgb8.png"), false);
  EXPECT_EQ(1.0, rgb8->Get(0, 1)->Reflectance(0.5));
  EXPECT_EQ(1.0, rgb8->Get(0, 1)->Reflectance(1.5));
  EXPECT_EQ(1.0, rgb8->Get(0, 1)->Reflectance(2.5));

  auto rgba8 = image_manager.LoadReflectorImageFromSDR(
      RawRunfilePath("rgba8.png"), false);
  EXPECT_EQ(1.0, rgba8->Get(0, 1)->Reflectance(0.5));
  EXPECT_EQ(1.0, rgba8->Get(0, 1)->Reflectance(1.5));
  EXPECT_EQ(1.0, rgba8->Get(0, 1)->Reflectance(2.5));

  auto g16 =
      image_manager.LoadReflectorImageFromSDR(RawRunfilePath("g16.png"), false);
  EXPECT_EQ(1.0, g16->Get(0, 1)->Reflectance(0.5));
  EXPECT_EQ(1.0, g16->Get(0, 1)->Reflectance(1.5));
  EXPECT_EQ(1.0, g16->Get(0, 1)->Reflectance(2.5));

  auto ga16 = image_manager.LoadReflectorImageFromSDR(
      RawRunfilePath("ga16.png"), false);
  EXPECT_EQ(1.0, ga16->Get(0, 1)->Reflectance(0.5));
  EXPECT_EQ(1.0, ga16->Get(0, 1)->Reflectance(1.5));
  EXPECT_EQ(1.0, ga16->Get(0, 1)->Reflectance(2.5));

  auto rgb16 = image_manager.LoadReflectorImageFromSDR(
      RawRunfilePath("rgb16.png"), false);
  EXPECT_EQ(1.0, rgb16->Get(0, 1)->Reflectance(0.5));
  EXPECT_EQ(1.0, rgb16->Get(0, 1)->Reflectance(1.5));
  EXPECT_EQ(1.0, rgb16->Get(0, 1)->Reflectance(2.5));

  auto rgba16 = image_manager.LoadReflectorImageFromSDR(
      RawRunfilePath("rgba16.png"), false);
  EXPECT_EQ(1.0, rgba16->Get(0, 1)->Reflectance(0.5));
  EXPECT_EQ(1.0, rgba16->Get(0, 1)->Reflectance(1.5));
  EXPECT_EQ(1.0, rgba16->Get(0, 1)->Reflectance(2.5));

  // HDR
  auto rgbw = image_manager.LoadReflectorImageFromHDR(
      RawRunfilePath("rgbw.exr"), false);
  EXPECT_NEAR(1.0, rgbw->Get(0, 1)->Reflectance(0.5), 0.001);
  EXPECT_NEAR(1.0, rgbw->Get(0, 1)->Reflectance(1.5), 0.001);
  EXPECT_NEAR(1.0, rgbw->Get(0, 1)->Reflectance(2.5), 0.001);
}

}  // namespace
}  // namespace pbrt_frontend
}  // namespace iris
