#include "frontends/pbrt/textures/imagemap.h"

#include <cstdlib>
#include <filesystem>
#include <limits>
#include <memory>

#include "frontends/pbrt/image_manager.h"
#include "frontends/pbrt/spectrum_managers/test_spectrum_manager.h"
#include "frontends/pbrt/texture_manager.h"
#include "googletest/include/gtest/gtest.h"
#include "pbrt_proto/v3/pbrt.pb.h"
#include "tools/cpp/runfiles/runfiles.h"

namespace iris {
namespace pbrt_frontend {
namespace textures {
namespace {

using ::bazel::tools::cpp::runfiles::Runfiles;
using ::iris::pbrt_frontend::spectrum_managers::TestSpectrumManager;
using ::pbrt_proto::v3::FloatTexture;
using ::pbrt_proto::v3::SpectrumTexture;
using ::testing::ExitedWithCode;

std::string RunfilePath(const std::string& path) {
  std::unique_ptr<Runfiles> runfiles(Runfiles::CreateForTest());
  return runfiles->Rlocation("_main/frontends/pbrt/textures/test_data/" + path);
}

TEST(FloatImageMap, Empty) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(std::filesystem::current_path(), texture_manager,
                             spectrum_manager);

  FloatTexture::ImageMap imagemap;

  EXPECT_EXIT(MakeImageMap(imagemap, image_manager, texture_manager),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Missing required image parameter: filename");
}

TEST(FloatImageMap, InfiniteMaxAnisotropy) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(std::filesystem::current_path(), texture_manager,
                             spectrum_manager);

  FloatTexture::ImageMap imagemap;
  imagemap.set_maxanisotropy(std::numeric_limits<double>::infinity());

  EXPECT_EXIT(MakeImageMap(imagemap, image_manager, texture_manager),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: maxanisotropy");
}

TEST(FloatImageMap, NegativeMaxAnisotropy) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(std::filesystem::current_path(), texture_manager,
                             spectrum_manager);

  FloatTexture::ImageMap imagemap;
  imagemap.set_maxanisotropy(-1.0);

  EXPECT_EXIT(MakeImageMap(imagemap, image_manager, texture_manager),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: maxanisotropy");
}

TEST(FloatImageMap, Scale) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(std::filesystem::current_path(), texture_manager,
                             spectrum_manager);

  FloatTexture::ImageMap imagemap;
  imagemap.set_maxanisotropy(std::numeric_limits<double>::infinity());

  EXPECT_EXIT(MakeImageMap(imagemap, image_manager, texture_manager),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: maxanisotropy");
}

TEST(FloatImageMap, InfiniteUDelta) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(std::filesystem::current_path(), texture_manager,
                             spectrum_manager);

  FloatTexture::ImageMap imagemap;
  imagemap.set_udelta(std::numeric_limits<double>::infinity());

  EXPECT_EXIT(MakeImageMap(imagemap, image_manager, texture_manager),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: udelta");
}

TEST(FloatImageMap, InfiniteUScale) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(std::filesystem::current_path(), texture_manager,
                             spectrum_manager);

  FloatTexture::ImageMap imagemap;
  imagemap.set_uscale(std::numeric_limits<double>::infinity());

  EXPECT_EXIT(MakeImageMap(imagemap, image_manager, texture_manager),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: uscale");
}

TEST(FloatImageMap, ZeroUScale) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(std::filesystem::current_path(), texture_manager,
                             spectrum_manager);

  FloatTexture::ImageMap imagemap;
  imagemap.set_uscale(0.0);

  EXPECT_EXIT(MakeImageMap(imagemap, image_manager, texture_manager),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: uscale");
}

TEST(FloatImageMap, InfiniteVDelta) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(std::filesystem::current_path(), texture_manager,
                             spectrum_manager);

  FloatTexture::ImageMap imagemap;
  imagemap.set_vdelta(std::numeric_limits<double>::infinity());

  EXPECT_EXIT(MakeImageMap(imagemap, image_manager, texture_manager),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: vdelta");
}

TEST(FloatImageMap, InfiniteVScale) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(std::filesystem::current_path(), texture_manager,
                             spectrum_manager);

  FloatTexture::ImageMap imagemap;
  imagemap.set_vscale(std::numeric_limits<double>::infinity());

  EXPECT_EXIT(MakeImageMap(imagemap, image_manager, texture_manager),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: vscale");
}

TEST(FloatImageMap, ZeroVScale) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(std::filesystem::current_path(), texture_manager,
                             spectrum_manager);

  FloatTexture::ImageMap imagemap;
  imagemap.set_vscale(0.0);

  EXPECT_EXIT(MakeImageMap(imagemap, image_manager, texture_manager),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: vscale");
}

TEST(FloatImageMap, PngSucceeds) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(std::filesystem::current_path(), texture_manager,
                             spectrum_manager);

  FloatTexture::ImageMap imagemap;
  imagemap.set_filename(RunfilePath("image.png"));

  EXPECT_TRUE(MakeImageMap(imagemap, image_manager, texture_manager));
}

TEST(FloatImageMap, NoExtension) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(std::filesystem::current_path(), texture_manager,
                             spectrum_manager);

  FloatTexture::ImageMap imagemap;
  imagemap.set_filename(RunfilePath("image"));

  EXPECT_EXIT(MakeImageMap(imagemap, image_manager, texture_manager),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Unsupported image file \\(no extension\\): image");
}

TEST(FloatImageMap, BadExtension) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(std::filesystem::current_path(), texture_manager,
                             spectrum_manager);

  FloatTexture::ImageMap imagemap;
  imagemap.set_filename(RunfilePath("image.txt"));

  EXPECT_EXIT(MakeImageMap(imagemap, image_manager, texture_manager),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Unsupported image file type: .txt");
}

TEST(SpectrumImageMap, Empty) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(std::filesystem::current_path(), texture_manager,
                             spectrum_manager);

  SpectrumTexture::ImageMap imagemap;

  EXPECT_EXIT(MakeImageMap(imagemap, image_manager, texture_manager),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Missing required image parameter: filename");
}

TEST(SpectrumImageMap, InfiniteMaxAnisotropy) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(std::filesystem::current_path(), texture_manager,
                             spectrum_manager);

  SpectrumTexture::ImageMap imagemap;
  imagemap.set_maxanisotropy(std::numeric_limits<double>::infinity());

  EXPECT_EXIT(MakeImageMap(imagemap, image_manager, texture_manager),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: maxanisotropy");
}

TEST(SpectrumImageMap, NegativeMaxAnisotropy) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(std::filesystem::current_path(), texture_manager,
                             spectrum_manager);

  SpectrumTexture::ImageMap imagemap;
  imagemap.set_maxanisotropy(-1.0);

  EXPECT_EXIT(MakeImageMap(imagemap, image_manager, texture_manager),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: maxanisotropy");
}

TEST(SpectrumImageMap, Scale) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(std::filesystem::current_path(), texture_manager,
                             spectrum_manager);

  SpectrumTexture::ImageMap imagemap;
  imagemap.set_maxanisotropy(std::numeric_limits<double>::infinity());

  EXPECT_EXIT(MakeImageMap(imagemap, image_manager, texture_manager),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: maxanisotropy");
}

TEST(SpectrumImageMap, InfiniteUDelta) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(std::filesystem::current_path(), texture_manager,
                             spectrum_manager);

  SpectrumTexture::ImageMap imagemap;
  imagemap.set_udelta(std::numeric_limits<double>::infinity());

  EXPECT_EXIT(MakeImageMap(imagemap, image_manager, texture_manager),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: udelta");
}

TEST(SpectrumImageMap, InfiniteUScale) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(std::filesystem::current_path(), texture_manager,
                             spectrum_manager);

  SpectrumTexture::ImageMap imagemap;
  imagemap.set_uscale(std::numeric_limits<double>::infinity());

  EXPECT_EXIT(MakeImageMap(imagemap, image_manager, texture_manager),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: uscale");
}

TEST(SpectrumImageMap, ZeroUScale) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(std::filesystem::current_path(), texture_manager,
                             spectrum_manager);

  SpectrumTexture::ImageMap imagemap;
  imagemap.set_uscale(0.0);

  EXPECT_EXIT(MakeImageMap(imagemap, image_manager, texture_manager),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: uscale");
}

TEST(SpectrumImageMap, InfiniteVDelta) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(std::filesystem::current_path(), texture_manager,
                             spectrum_manager);

  SpectrumTexture::ImageMap imagemap;
  imagemap.set_vdelta(std::numeric_limits<double>::infinity());

  EXPECT_EXIT(MakeImageMap(imagemap, image_manager, texture_manager),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: vdelta");
}

TEST(SpectrumImageMap, InfiniteVScale) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(std::filesystem::current_path(), texture_manager,
                             spectrum_manager);

  SpectrumTexture::ImageMap imagemap;
  imagemap.set_vscale(std::numeric_limits<double>::infinity());

  EXPECT_EXIT(MakeImageMap(imagemap, image_manager, texture_manager),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: vscale");
}

TEST(SpectrumImageMap, ZeroVScale) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(std::filesystem::current_path(), texture_manager,
                             spectrum_manager);

  SpectrumTexture::ImageMap imagemap;
  imagemap.set_vscale(0.0);

  EXPECT_EXIT(MakeImageMap(imagemap, image_manager, texture_manager),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: vscale");
}

TEST(SpectrumImageMap, PngSucceeds) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(std::filesystem::current_path(), texture_manager,
                             spectrum_manager);

  SpectrumTexture::ImageMap imagemap;
  imagemap.set_filename(RunfilePath("image.png"));

  EXPECT_TRUE(MakeImageMap(imagemap, image_manager, texture_manager));
}

TEST(SpectrumImageMap, NoExtension) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(std::filesystem::current_path(), texture_manager,
                             spectrum_manager);

  SpectrumTexture::ImageMap imagemap;
  imagemap.set_filename(RunfilePath("image"));

  EXPECT_EXIT(MakeImageMap(imagemap, image_manager, texture_manager),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Unsupported image file \\(no extension\\): image");
}

TEST(SpectrumImageMap, BadExtension) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(std::filesystem::current_path(), texture_manager,
                             spectrum_manager);

  SpectrumTexture::ImageMap imagemap;
  imagemap.set_filename(RunfilePath("image.txt"));

  EXPECT_EXIT(MakeImageMap(imagemap, image_manager, texture_manager),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Unsupported image file type: .txt");
}

}  // namespace
}  // namespace textures
}  // namespace pbrt_frontend
}  // namespace iris
