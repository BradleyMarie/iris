#include "frontends/pbrt/textures/parse.h"

#include "frontends/pbrt/image_manager.h"
#include "frontends/pbrt/spectrum_managers/test_spectrum_manager.h"
#include "frontends/pbrt/texture_manager.h"
#include "googletest/include/gtest/gtest.h"
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
  ImageManager image_manager(texture_manager, spectrum_manager);

  FloatTexture float_texture;
  float_texture.set_name("A");

  ParseFloatTexture(float_texture, image_manager, texture_manager);
  EXPECT_EXIT(texture_manager.GetFloatTexture("A"),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: No float texture defined with name: A");
}

TEST(ParseFloatTexture, Bilerp) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(texture_manager, spectrum_manager);

  FloatTexture float_texture;
  float_texture.set_name("A");
  float_texture.mutable_bilerp();

  ParseFloatTexture(float_texture, image_manager, texture_manager);
  EXPECT_EXIT(texture_manager.GetFloatTexture("A"),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: No float texture defined with name: A");
}

TEST(ParseFloatTexture, Checkerboard2D) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(texture_manager, spectrum_manager);

  FloatTexture float_texture;
  float_texture.set_name("A");
  float_texture.mutable_checkerboard2d();

  ParseFloatTexture(float_texture, image_manager, texture_manager);
  EXPECT_EXIT(texture_manager.GetFloatTexture("A"),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: No float texture defined with name: A");
}

TEST(ParseFloatTexture, Checkerboard3D) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(texture_manager, spectrum_manager);

  FloatTexture float_texture;
  float_texture.set_name("A");
  float_texture.mutable_checkerboard3d();

  ParseFloatTexture(float_texture, image_manager, texture_manager);
  EXPECT_EXIT(texture_manager.GetFloatTexture("A"),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: No float texture defined with name: A");
}

TEST(ParseFloatTexture, Constant) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(texture_manager, spectrum_manager);

  FloatTexture float_texture;
  float_texture.set_name("A");
  float_texture.mutable_constant();

  ParseFloatTexture(float_texture, image_manager, texture_manager);
  EXPECT_TRUE(texture_manager.GetFloatTexture("A"));
}

TEST(ParseFloatTexture, Dots) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(texture_manager, spectrum_manager);

  FloatTexture float_texture;
  float_texture.set_name("A");
  float_texture.mutable_dots();

  ParseFloatTexture(float_texture, image_manager, texture_manager);
  EXPECT_EXIT(texture_manager.GetFloatTexture("A"),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: No float texture defined with name: A");
}

TEST(ParseFloatTexture, Fbm) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(texture_manager, spectrum_manager);

  FloatTexture float_texture;
  float_texture.set_name("A");
  float_texture.mutable_fbm();

  ParseFloatTexture(float_texture, image_manager, texture_manager);
  EXPECT_EXIT(texture_manager.GetFloatTexture("A"),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: No float texture defined with name: A");
}

TEST(ParseFloatTexture, ImageMap) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(texture_manager, spectrum_manager);

  FloatTexture float_texture;
  float_texture.set_name("A");
  float_texture.mutable_imagemap();

  EXPECT_EXIT(ParseFloatTexture(float_texture, image_manager, texture_manager),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Missing required image parameter: filename");
}

TEST(ParseFloatTexture, Marble) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(texture_manager, spectrum_manager);

  FloatTexture float_texture;
  float_texture.set_name("A");
  float_texture.mutable_marble();

  ParseFloatTexture(float_texture, image_manager, texture_manager);
  EXPECT_EXIT(texture_manager.GetFloatTexture("A"),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: No float texture defined with name: A");
}

TEST(ParseFloatTexture, Mix) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(texture_manager, spectrum_manager);

  FloatTexture float_texture;
  float_texture.set_name("A");
  float_texture.mutable_mix();

  ParseFloatTexture(float_texture, image_manager, texture_manager);
  EXPECT_EXIT(texture_manager.GetFloatTexture("A"),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: No float texture defined with name: A");
}

TEST(ParseFloatTexture, PTex) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(texture_manager, spectrum_manager);

  FloatTexture float_texture;
  float_texture.set_name("A");
  float_texture.mutable_ptex();

  ParseFloatTexture(float_texture, image_manager, texture_manager);
  EXPECT_EXIT(texture_manager.GetFloatTexture("A"),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: No float texture defined with name: A");
}

TEST(ParseFloatTexture, Scale) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(texture_manager, spectrum_manager);

  FloatTexture float_texture;
  float_texture.set_name("A");
  float_texture.mutable_scale();

  ParseFloatTexture(float_texture, image_manager, texture_manager);
  EXPECT_TRUE(texture_manager.GetFloatTexture("A"));
}

TEST(ParseFloatTexture, Windy) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(texture_manager, spectrum_manager);

  FloatTexture float_texture;
  float_texture.set_name("A");
  float_texture.mutable_windy();

  ParseFloatTexture(float_texture, image_manager, texture_manager);
  EXPECT_EXIT(texture_manager.GetFloatTexture("A"),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: No float texture defined with name: A");
}

TEST(ParseFloatTexture, Wrinkled) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(texture_manager, spectrum_manager);

  FloatTexture float_texture;
  float_texture.set_name("A");
  float_texture.mutable_wrinkled();

  ParseFloatTexture(float_texture, image_manager, texture_manager);
  EXPECT_EXIT(texture_manager.GetFloatTexture("A"),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: No float texture defined with name: A");
}

TEST(ParseSpectrumTexture, Empty) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(texture_manager, spectrum_manager);

  SpectrumTexture spectrum_texture;
  spectrum_texture.set_name("A");

  ParseSpectrumTexture(spectrum_texture, image_manager, texture_manager);
  EXPECT_EXIT(texture_manager.GetSpectrumTexture("A"),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: No spectrum texture defined with name: A");
}

TEST(ParseSpectrumTexture, Bilerp) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(texture_manager, spectrum_manager);

  SpectrumTexture spectrum_texture;
  spectrum_texture.set_name("A");
  spectrum_texture.mutable_bilerp();

  ParseSpectrumTexture(spectrum_texture, image_manager, texture_manager);
  EXPECT_EXIT(texture_manager.GetSpectrumTexture("A"),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: No spectrum texture defined with name: A");
}

TEST(ParseSpectrumTexture, Checkerboard2D) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(texture_manager, spectrum_manager);

  SpectrumTexture spectrum_texture;
  spectrum_texture.set_name("A");
  spectrum_texture.mutable_checkerboard2d();

  ParseSpectrumTexture(spectrum_texture, image_manager, texture_manager);
  EXPECT_EXIT(texture_manager.GetSpectrumTexture("A"),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: No spectrum texture defined with name: A");
}

TEST(ParseSpectrumTexture, Checkerboard3D) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(texture_manager, spectrum_manager);

  SpectrumTexture spectrum_texture;
  spectrum_texture.set_name("A");
  spectrum_texture.mutable_checkerboard3d();

  ParseSpectrumTexture(spectrum_texture, image_manager, texture_manager);
  EXPECT_EXIT(texture_manager.GetSpectrumTexture("A"),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: No spectrum texture defined with name: A");
}

TEST(ParseSpectrumTexture, Constant) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(texture_manager, spectrum_manager);

  SpectrumTexture spectrum_texture;
  spectrum_texture.set_name("A");
  spectrum_texture.mutable_constant();

  ParseSpectrumTexture(spectrum_texture, image_manager, texture_manager);
  EXPECT_TRUE(texture_manager.GetSpectrumTexture("A"));
}

TEST(ParseSpectrumTexture, Dots) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(texture_manager, spectrum_manager);

  SpectrumTexture spectrum_texture;
  spectrum_texture.set_name("A");
  spectrum_texture.mutable_dots();

  ParseSpectrumTexture(spectrum_texture, image_manager, texture_manager);
  EXPECT_EXIT(texture_manager.GetSpectrumTexture("A"),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: No spectrum texture defined with name: A");
}

TEST(ParseSpectrumTexture, Fbm) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(texture_manager, spectrum_manager);

  SpectrumTexture spectrum_texture;
  spectrum_texture.set_name("A");
  spectrum_texture.mutable_fbm();

  ParseSpectrumTexture(spectrum_texture, image_manager, texture_manager);
  EXPECT_EXIT(texture_manager.GetSpectrumTexture("A"),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: No spectrum texture defined with name: A");
}

TEST(ParseSpectrumTexture, ImageMap) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(texture_manager, spectrum_manager);

  SpectrumTexture spectrum_texture;
  spectrum_texture.set_name("A");
  spectrum_texture.mutable_imagemap();

  EXPECT_EXIT(
      ParseSpectrumTexture(spectrum_texture, image_manager, texture_manager),
      ExitedWithCode(EXIT_FAILURE),
      "ERROR: Missing required image parameter: filename");
}

TEST(ParseSpectrumTexture, Marble) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(texture_manager, spectrum_manager);

  SpectrumTexture spectrum_texture;
  spectrum_texture.set_name("A");
  spectrum_texture.mutable_marble();

  ParseSpectrumTexture(spectrum_texture, image_manager, texture_manager);
  EXPECT_EXIT(texture_manager.GetSpectrumTexture("A"),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: No spectrum texture defined with name: A");
}

TEST(ParseSpectrumTexture, Mix) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(texture_manager, spectrum_manager);

  SpectrumTexture spectrum_texture;
  spectrum_texture.set_name("A");
  spectrum_texture.mutable_mix();

  ParseSpectrumTexture(spectrum_texture, image_manager, texture_manager);
  EXPECT_EXIT(texture_manager.GetSpectrumTexture("A"),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: No spectrum texture defined with name: A");
}

TEST(ParseSpectrumTexture, PTex) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(texture_manager, spectrum_manager);

  SpectrumTexture spectrum_texture;
  spectrum_texture.set_name("A");
  spectrum_texture.mutable_ptex();

  ParseSpectrumTexture(spectrum_texture, image_manager, texture_manager);
  EXPECT_EXIT(texture_manager.GetSpectrumTexture("A"),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: No spectrum texture defined with name: A");
}

TEST(ParseSpectrumTexture, Scale) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(texture_manager, spectrum_manager);

  SpectrumTexture spectrum_texture;
  spectrum_texture.set_name("A");
  spectrum_texture.mutable_scale();

  ParseSpectrumTexture(spectrum_texture, image_manager, texture_manager);
  EXPECT_TRUE(texture_manager.GetSpectrumTexture("A"));
}

TEST(ParseSpectrumTexture, UV) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(texture_manager, spectrum_manager);

  SpectrumTexture spectrum_texture;
  spectrum_texture.set_name("A");
  spectrum_texture.mutable_uv();

  ParseSpectrumTexture(spectrum_texture, image_manager, texture_manager);
  EXPECT_EXIT(texture_manager.GetSpectrumTexture("A"),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: No spectrum texture defined with name: A");
}

TEST(ParseSpectrumTexture, Windy) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(texture_manager, spectrum_manager);

  SpectrumTexture spectrum_texture;
  spectrum_texture.set_name("A");
  spectrum_texture.mutable_windy();

  ParseSpectrumTexture(spectrum_texture, image_manager, texture_manager);
  EXPECT_EXIT(texture_manager.GetSpectrumTexture("A"),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: No spectrum texture defined with name: A");
}

TEST(ParseSpectrumTexture, Wrinkled) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(texture_manager, spectrum_manager);

  SpectrumTexture spectrum_texture;
  spectrum_texture.set_name("A");
  spectrum_texture.mutable_wrinkled();

  ParseSpectrumTexture(spectrum_texture, image_manager, texture_manager);
  EXPECT_EXIT(texture_manager.GetSpectrumTexture("A"),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: No spectrum texture defined with name: A");
}

}  // namespace
}  // namespace pbrt_frontend
}  // namespace iris
