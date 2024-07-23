#include "frontends/pbrt/image_manager.h"

#include <set>

#include "frontends/pbrt/spectrum_managers/color_spectrum_manager.h"
#include "googletest/include/gtest/gtest.h"
#include "tools/cpp/runfiles/runfiles.h"

using bazel::tools::cpp::runfiles::Runfiles;

std::string RawRunfilePath(const std::string& path) {
  std::unique_ptr<Runfiles> runfiles(Runfiles::CreateForTest());
  const char* base_path = "__main__/frontends/pbrt/test_data/";
  return runfiles->Rlocation(base_path + path);
}

TEST(ImageManager, MissingFile) {
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::spectrum_managers::ColorSpectrumManager spectrum_manager(
      true);
  iris::pbrt_frontend::ImageManager image_manager(texture_manager,
                                                  spectrum_manager);
  EXPECT_EXIT(image_manager.LoadFloatImageFromSDR("notarealfile", true),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Failed to load image: notarealfile");
  EXPECT_EXIT(image_manager.LoadReflectorImageFromSDR("notarealfile", true),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Failed to load image: notarealfile");
}

TEST(ImageManager, FloatImageSizesCorrect) {
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::spectrum_managers::ColorSpectrumManager spectrum_manager(
      true);
  iris::pbrt_frontend::ImageManager image_manager(texture_manager,
                                                  spectrum_manager);

  std::set<std::shared_ptr<iris::textures::Image2D<iris::visual>>> images;
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

  for (const auto& image : images) {
    auto size = image->Size();
    EXPECT_EQ(size.first, 2u);
    EXPECT_EQ(size.second, 2u);
  }
}

TEST(ImageManager, ReflectorImageSizesCorrect) {
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::spectrum_managers::ColorSpectrumManager spectrum_manager(
      true);
  iris::pbrt_frontend::ImageManager image_manager(texture_manager,
                                                  spectrum_manager);

  std::set<std::shared_ptr<
      iris::textures::Image2D<iris::ReferenceCounted<iris::Reflector>>>>
      images;
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

  for (const auto& image : images) {
    auto size = image->Size();
    EXPECT_EQ(size.first, 2u);
    EXPECT_EQ(size.second, 2u);
  }
}

TEST(ImageManager, ReusesFloatImages) {
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::spectrum_managers::ColorSpectrumManager spectrum_manager(
      true);
  iris::pbrt_frontend::ImageManager image_manager(texture_manager,
                                                  spectrum_manager);

  std::set<std::shared_ptr<iris::textures::Image2D<iris::visual>>> images;
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
}

TEST(ImageManager, ReusesReflectorImages) {
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::spectrum_managers::ColorSpectrumManager spectrum_manager(
      true);
  iris::pbrt_frontend::ImageManager image_manager(texture_manager,
                                                  spectrum_manager);

  std::set<std::shared_ptr<
      iris::textures::Image2D<iris::ReferenceCounted<iris::Reflector>>>>
      images;
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
}

TEST(ImageManager, ReusesUniformReflectors) {
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::spectrum_managers::ColorSpectrumManager spectrum_manager(
      true);
  iris::pbrt_frontend::ImageManager image_manager(texture_manager,
                                                  spectrum_manager);

  auto g8 =
      image_manager.LoadReflectorImageFromSDR(RawRunfilePath("g8.png"), false);
  auto g8_corrected =
      image_manager.LoadReflectorImageFromSDR(RawRunfilePath("g8.png"), true);
  EXPECT_NE(g8->Get(0, 0), g8_corrected->Get(0, 0));
  EXPECT_EQ(g8->Get(0, 1), g8_corrected->Get(0, 1));
  EXPECT_NE(g8->Get(1, 0), g8_corrected->Get(1, 0));
  EXPECT_NE(g8->Get(1, 1), g8_corrected->Get(1, 1));

  auto ga8 =
      image_manager.LoadReflectorImageFromSDR(RawRunfilePath("ga8.png"), false);
  EXPECT_EQ(ga8->Get(0, 0), g8->Get(0, 0));
  EXPECT_EQ(ga8->Get(0, 1), g8->Get(0, 1));
  EXPECT_EQ(ga8->Get(1, 0), g8->Get(1, 0));
  EXPECT_EQ(ga8->Get(1, 1), g8->Get(1, 1));

  auto ga8_corrected =
      image_manager.LoadReflectorImageFromSDR(RawRunfilePath("ga8.png"), true);
  EXPECT_EQ(ga8_corrected->Get(0, 0), g8_corrected->Get(0, 0));
  EXPECT_EQ(ga8_corrected->Get(0, 1), g8_corrected->Get(0, 1));
  EXPECT_EQ(ga8_corrected->Get(1, 0), g8_corrected->Get(1, 0));
  EXPECT_EQ(ga8_corrected->Get(1, 1), g8_corrected->Get(1, 1));
}

TEST(ImageManager, ReusesColorReflectors) {
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::spectrum_managers::ColorSpectrumManager spectrum_manager(
      true);
  iris::pbrt_frontend::ImageManager image_manager(texture_manager,
                                                  spectrum_manager);

  auto rgb8 = image_manager.LoadReflectorImageFromSDR(
      RawRunfilePath("rgb8.png"), false);
  auto rgb8_corrected =
      image_manager.LoadReflectorImageFromSDR(RawRunfilePath("rgb8.png"), true);
  EXPECT_NE(rgb8->Get(0, 0), rgb8_corrected->Get(0, 0));
  EXPECT_NE(rgb8->Get(0, 1), rgb8_corrected->Get(0, 1));
  EXPECT_NE(rgb8->Get(1, 0), rgb8_corrected->Get(1, 0));
  EXPECT_NE(rgb8->Get(1, 1), rgb8_corrected->Get(1, 1));

  auto rgba8 = image_manager.LoadReflectorImageFromSDR(
      RawRunfilePath("rgba8.png"), false);
  EXPECT_EQ(rgba8->Get(0, 0), rgb8->Get(0, 0));
  EXPECT_EQ(rgba8->Get(0, 1), rgb8->Get(0, 1));
  EXPECT_EQ(rgba8->Get(1, 0), rgb8->Get(1, 0));
  EXPECT_EQ(rgba8->Get(1, 1), rgb8->Get(1, 1));

  auto rgba8_corrected = image_manager.LoadReflectorImageFromSDR(
      RawRunfilePath("rgba8.png"), true);
  EXPECT_EQ(rgba8_corrected->Get(0, 0), rgb8_corrected->Get(0, 0));
  EXPECT_EQ(rgba8_corrected->Get(0, 1), rgb8_corrected->Get(0, 1));
  EXPECT_EQ(rgba8_corrected->Get(1, 0), rgb8_corrected->Get(1, 0));
  EXPECT_EQ(rgba8_corrected->Get(1, 1), rgb8_corrected->Get(1, 1));
}

TEST(ImageManager, LinearFloatValues) {
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::spectrum_managers::ColorSpectrumManager spectrum_manager(
      true);
  iris::pbrt_frontend::ImageManager image_manager(texture_manager,
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
}

TEST(ImageManager, LinearReflectorValues) {
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::spectrum_managers::ColorSpectrumManager spectrum_manager(
      true);
  iris::pbrt_frontend::ImageManager image_manager(texture_manager,
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
}