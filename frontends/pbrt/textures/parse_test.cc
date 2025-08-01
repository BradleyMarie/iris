#include "frontends/pbrt/textures/parse.h"

#include <cstdlib>
#include <filesystem>

#include "frontends/pbrt/image_manager.h"
#include "frontends/pbrt/spectrum_managers/test_spectrum_manager.h"
#include "frontends/pbrt/texture_manager.h"
#include "googletest/include/gtest/gtest.h"
#include "iris/matrix.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace {

using ::iris::pbrt_frontend::spectrum_managers::TestSpectrumManager;
using ::pbrt_proto::v3::FloatTexture;
using ::pbrt_proto::v3::SpectrumTexture;
using ::testing::ExitedWithCode;

TEST(ParseFloatTexture, Empty) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(std::filesystem::current_path(), texture_manager,
                             spectrum_manager);

  FloatTexture float_texture;
  float_texture.set_name("A");

  ParseFloatTexture(float_texture, image_manager, texture_manager,
                    Matrix::Identity());
  EXPECT_EXIT(texture_manager.GetFloatTexture("A"),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: No float texture defined with name: \"A\"");
}

TEST(ParseFloatTexture, Bilerp) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(std::filesystem::current_path(), texture_manager,
                             spectrum_manager);

  FloatTexture float_texture;
  float_texture.set_name("A");
  float_texture.mutable_bilerp();

  EXPECT_EXIT(ParseFloatTexture(float_texture, image_manager, texture_manager,
                                Matrix::Identity()),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Unsupported FloatTexture type: bilerp");
}

TEST(ParseFloatTexture, Checkerboard2D) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(std::filesystem::current_path(), texture_manager,
                             spectrum_manager);

  FloatTexture float_texture;
  float_texture.set_name("A");
  float_texture.mutable_checkerboard2d();

  EXPECT_EXIT(ParseFloatTexture(float_texture, image_manager, texture_manager,
                                Matrix::Identity()),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Unsupported FloatTexture type: checkerboard");
}

TEST(ParseFloatTexture, Checkerboard3D) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(std::filesystem::current_path(), texture_manager,
                             spectrum_manager);

  FloatTexture float_texture;
  float_texture.set_name("A");
  float_texture.mutable_checkerboard3d();

  EXPECT_EXIT(ParseFloatTexture(float_texture, image_manager, texture_manager,
                                Matrix::Identity()),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Unsupported FloatTexture type: checkerboard");
}

TEST(ParseFloatTexture, Constant) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(std::filesystem::current_path(), texture_manager,
                             spectrum_manager);

  FloatTexture float_texture;
  float_texture.set_name("A");
  float_texture.mutable_constant();

  ParseFloatTexture(float_texture, image_manager, texture_manager,
                    Matrix::Identity());
  EXPECT_TRUE(texture_manager.GetFloatTexture("A"));
}

TEST(ParseFloatTexture, Dots) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(std::filesystem::current_path(), texture_manager,
                             spectrum_manager);

  FloatTexture float_texture;
  float_texture.set_name("A");
  float_texture.mutable_dots();

  EXPECT_EXIT(ParseFloatTexture(float_texture, image_manager, texture_manager,
                                Matrix::Identity()),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Unsupported FloatTexture type: dots");
}

TEST(ParseFloatTexture, Fbm) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(std::filesystem::current_path(), texture_manager,
                             spectrum_manager);

  FloatTexture float_texture;
  float_texture.set_name("A");
  float_texture.mutable_fbm();

  ParseFloatTexture(float_texture, image_manager, texture_manager,
                    Matrix::Identity());
  EXPECT_TRUE(texture_manager.GetFloatTexture("A"));
}

TEST(ParseFloatTexture, ImageMap) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(std::filesystem::current_path(), texture_manager,
                             spectrum_manager);

  FloatTexture float_texture;
  float_texture.set_name("A");
  float_texture.mutable_imagemap();

  EXPECT_EXIT(ParseFloatTexture(float_texture, image_manager, texture_manager,
                                Matrix::Identity()),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Missing required image parameter: filename");
}

TEST(ParseFloatTexture, Marble) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(std::filesystem::current_path(), texture_manager,
                             spectrum_manager);

  FloatTexture float_texture;
  float_texture.set_name("A");
  float_texture.mutable_marble();

  ParseFloatTexture(float_texture, image_manager, texture_manager,
                    Matrix::Identity());
  EXPECT_EXIT(texture_manager.GetReflectorTexture("A"),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: No spectrum texture defined with name: \"A\"");
}

TEST(ParseFloatTexture, Mix) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(std::filesystem::current_path(), texture_manager,
                             spectrum_manager);

  FloatTexture float_texture;
  float_texture.set_name("A");
  float_texture.mutable_mix();

  ParseFloatTexture(float_texture, image_manager, texture_manager,
                    Matrix::Identity());
  EXPECT_TRUE(texture_manager.GetFloatTexture("A"));
}

TEST(ParseFloatTexture, Ptex) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(std::filesystem::current_path(), texture_manager,
                             spectrum_manager);

  FloatTexture float_texture;
  float_texture.set_name("A");
  float_texture.mutable_ptex();

  EXPECT_EXIT(ParseFloatTexture(float_texture, image_manager, texture_manager,
                                Matrix::Identity()),
              ExitedWithCode(EXIT_FAILURE), "ERROR: Failed to open ptex file");
}

TEST(ParseFloatTexture, Scale) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(std::filesystem::current_path(), texture_manager,
                             spectrum_manager);

  FloatTexture float_texture;
  float_texture.set_name("A");
  float_texture.mutable_scale();

  ParseFloatTexture(float_texture, image_manager, texture_manager,
                    Matrix::Identity());
  EXPECT_TRUE(texture_manager.GetFloatTexture("A"));
}

TEST(ParseFloatTexture, Windy) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(std::filesystem::current_path(), texture_manager,
                             spectrum_manager);

  FloatTexture float_texture;
  float_texture.set_name("A");
  float_texture.mutable_windy();

  ParseFloatTexture(float_texture, image_manager, texture_manager,
                    Matrix::Identity());
  EXPECT_TRUE(texture_manager.GetFloatTexture("A"));
}

TEST(ParseFloatTexture, Wrinkled) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(std::filesystem::current_path(), texture_manager,
                             spectrum_manager);

  FloatTexture float_texture;
  float_texture.set_name("A");
  float_texture.mutable_wrinkled();

  ParseFloatTexture(float_texture, image_manager, texture_manager,
                    Matrix::Identity());
  EXPECT_TRUE(texture_manager.GetFloatTexture("A"));
}

TEST(ParseSpectrumTexture, Empty) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(std::filesystem::current_path(), texture_manager,
                             spectrum_manager);

  SpectrumTexture spectrum_texture;
  spectrum_texture.set_name("A");

  ParseSpectrumTexture(spectrum_texture, image_manager, texture_manager,
                       Matrix::Identity());
  EXPECT_EXIT(texture_manager.GetReflectorTexture("A"),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: No spectrum texture defined with name: \"A\"");
}

TEST(ParseSpectrumTexture, Bilerp) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(std::filesystem::current_path(), texture_manager,
                             spectrum_manager);

  SpectrumTexture spectrum_texture;
  spectrum_texture.set_name("A");
  spectrum_texture.mutable_bilerp();

  EXPECT_EXIT(ParseSpectrumTexture(spectrum_texture, image_manager,
                                   texture_manager, Matrix::Identity()),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Unsupported SpectrumTexture type: bilerp");
}

TEST(ParseSpectrumTexture, Checkerboard2D) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(std::filesystem::current_path(), texture_manager,
                             spectrum_manager);

  SpectrumTexture spectrum_texture;
  spectrum_texture.set_name("A");
  spectrum_texture.mutable_checkerboard2d();

  EXPECT_EXIT(ParseSpectrumTexture(spectrum_texture, image_manager,
                                   texture_manager, Matrix::Identity()),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Unsupported SpectrumTexture type: checkerboard");
}

TEST(ParseSpectrumTexture, Checkerboard3D) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(std::filesystem::current_path(), texture_manager,
                             spectrum_manager);

  SpectrumTexture spectrum_texture;
  spectrum_texture.set_name("A");
  spectrum_texture.mutable_checkerboard3d();

  EXPECT_EXIT(ParseSpectrumTexture(spectrum_texture, image_manager,
                                   texture_manager, Matrix::Identity()),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Unsupported SpectrumTexture type: checkerboard");
}

TEST(ParseSpectrumTexture, Constant) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(std::filesystem::current_path(), texture_manager,
                             spectrum_manager);

  SpectrumTexture spectrum_texture;
  spectrum_texture.set_name("A");
  spectrum_texture.mutable_constant();

  ParseSpectrumTexture(spectrum_texture, image_manager, texture_manager,
                       Matrix::Identity());
  EXPECT_TRUE(texture_manager.GetReflectorTexture("A"));
}

TEST(ParseSpectrumTexture, Dots) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(std::filesystem::current_path(), texture_manager,
                             spectrum_manager);

  SpectrumTexture spectrum_texture;
  spectrum_texture.set_name("A");
  spectrum_texture.mutable_dots();

  EXPECT_EXIT(ParseSpectrumTexture(spectrum_texture, image_manager,
                                   texture_manager, Matrix::Identity()),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Unsupported SpectrumTexture type: dots");
}

TEST(ParseSpectrumTexture, Fbm) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(std::filesystem::current_path(), texture_manager,
                             spectrum_manager);

  SpectrumTexture spectrum_texture;
  spectrum_texture.set_name("A");
  spectrum_texture.mutable_fbm();

  ParseSpectrumTexture(spectrum_texture, image_manager, texture_manager,
                       Matrix::Identity());
  EXPECT_TRUE(texture_manager.GetReflectorTexture("A"));
}

TEST(ParseSpectrumTexture, ImageMap) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(std::filesystem::current_path(), texture_manager,
                             spectrum_manager);

  SpectrumTexture spectrum_texture;
  spectrum_texture.set_name("A");
  spectrum_texture.mutable_imagemap();

  EXPECT_EXIT(ParseSpectrumTexture(spectrum_texture, image_manager,
                                   texture_manager, Matrix::Identity()),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Missing required image parameter: filename");
}

TEST(ParseSpectrumTexture, Marble) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(std::filesystem::current_path(), texture_manager,
                             spectrum_manager);

  SpectrumTexture spectrum_texture;
  spectrum_texture.set_name("A");
  spectrum_texture.mutable_marble();

  ParseSpectrumTexture(spectrum_texture, image_manager, texture_manager,
                       Matrix::Identity());
  EXPECT_TRUE(texture_manager.GetReflectorTexture("A"));
}

TEST(ParseSpectrumTexture, Mix) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(std::filesystem::current_path(), texture_manager,
                             spectrum_manager);

  SpectrumTexture spectrum_texture;
  spectrum_texture.set_name("A");
  spectrum_texture.mutable_mix();

  ParseSpectrumTexture(spectrum_texture, image_manager, texture_manager,
                       Matrix::Identity());
  EXPECT_TRUE(texture_manager.GetReflectorTexture("A"));
}

TEST(ParseSpectrumTexture, Ptex) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(std::filesystem::current_path(), texture_manager,
                             spectrum_manager);

  SpectrumTexture spectrum_texture;
  spectrum_texture.set_name("A");
  spectrum_texture.mutable_ptex();

  EXPECT_EXIT(ParseSpectrumTexture(spectrum_texture, image_manager,
                                   texture_manager, Matrix::Identity()),
              ExitedWithCode(EXIT_FAILURE), "ERROR: Failed to open ptex file");
}

TEST(ParseSpectrumTexture, Scale) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(std::filesystem::current_path(), texture_manager,
                             spectrum_manager);

  SpectrumTexture spectrum_texture;
  spectrum_texture.set_name("A");
  spectrum_texture.mutable_scale();

  ParseSpectrumTexture(spectrum_texture, image_manager, texture_manager,
                       Matrix::Identity());
  EXPECT_TRUE(texture_manager.GetReflectorTexture("A"));
}

TEST(ParseSpectrumTexture, UV) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(std::filesystem::current_path(), texture_manager,
                             spectrum_manager);

  SpectrumTexture spectrum_texture;
  spectrum_texture.set_name("A");
  spectrum_texture.mutable_uv();

  EXPECT_EXIT(ParseSpectrumTexture(spectrum_texture, image_manager,
                                   texture_manager, Matrix::Identity()),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Unsupported SpectrumTexture type: uv");
}

TEST(ParseSpectrumTexture, Windy) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(std::filesystem::current_path(), texture_manager,
                             spectrum_manager);

  SpectrumTexture spectrum_texture;
  spectrum_texture.set_name("A");
  spectrum_texture.mutable_windy();

  ParseSpectrumTexture(spectrum_texture, image_manager, texture_manager,
                       Matrix::Identity());
  EXPECT_TRUE(texture_manager.GetReflectorTexture("A"));
}

TEST(ParseSpectrumTexture, Wrinkled) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(std::filesystem::current_path(), texture_manager,
                             spectrum_manager);

  SpectrumTexture spectrum_texture;
  spectrum_texture.set_name("A");
  spectrum_texture.mutable_wrinkled();

  ParseSpectrumTexture(spectrum_texture, image_manager, texture_manager,
                       Matrix::Identity());
  EXPECT_TRUE(texture_manager.GetReflectorTexture("A"));
}

}  // namespace
}  // namespace pbrt_frontend
}  // namespace iris
