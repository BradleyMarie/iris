#include "frontends/pbrt/textures/parse.h"

#include "frontends/pbrt/textures/constant.h"
#include "frontends/pbrt/textures/image.h"
#include "frontends/pbrt/textures/scale.h"
#include "googletest/include/gtest/gtest.h"

TEST(Parse, NoName) {
  std::stringstream input("");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  std::string texture_name;
  EXPECT_EXIT(iris::pbrt_frontend::textures::Parse(tokenizer, texture_name),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Too few parameters to directive: Texture");
}

TEST(Parse, NameNotAString) {
  std::stringstream input("1.0");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  std::string texture_name;
  EXPECT_EXIT(iris::pbrt_frontend::textures::Parse(tokenizer, texture_name),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Name of Texture must be a string");
}

TEST(Parse, NoValueType) {
  std::stringstream input("\"name\"");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  std::string texture_name;
  EXPECT_EXIT(iris::pbrt_frontend::textures::Parse(tokenizer, texture_name),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Too few parameters to directive: Texture");
}

TEST(Parse, ValueTypeNotAString) {
  std::stringstream input("\"name\" 1.0");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  std::string texture_name;
  EXPECT_EXIT(iris::pbrt_frontend::textures::Parse(tokenizer, texture_name),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Value type of Texture must be a string");
}

TEST(Parse, InvalidValueType) {
  std::stringstream input("\"name\" \"NotAType\"");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  std::string texture_name;
  EXPECT_EXIT(iris::pbrt_frontend::textures::Parse(tokenizer, texture_name),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Unsupported value type for directive Texture: NotAType");
}

TEST(Parse, NoType) {
  std::stringstream input("\"name\" \"float\"");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  std::string texture_name;
  EXPECT_EXIT(iris::pbrt_frontend::textures::Parse(tokenizer, texture_name),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Too few parameters to directive: Texture");
}

TEST(Parse, TypeNotAString) {
  std::stringstream input("\"name\" \"float\" 1.0");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  std::string texture_name;
  EXPECT_EXIT(iris::pbrt_frontend::textures::Parse(tokenizer, texture_name),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Type of Texture must be a string");
}

TEST(Parse, InvalidFloatType) {
  std::stringstream input("\"name\" \"float\" \"NotAType\"");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  std::string texture_name;
  EXPECT_EXIT(iris::pbrt_frontend::textures::Parse(tokenizer, texture_name),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Unsupported type for directive Texture: NotAType");
}

TEST(Parse, InvalidColorType) {
  std::stringstream input("\"name\" \"color\" \"NotAType\"");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  std::string texture_name;
  EXPECT_EXIT(iris::pbrt_frontend::textures::Parse(tokenizer, texture_name),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Unsupported type for directive Texture: NotAType");
}

TEST(Parse, InvalidSpectrumType) {
  std::stringstream input("\"name\" \"spectrum\" \"NotAType\"");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  std::string texture_name;
  EXPECT_EXIT(iris::pbrt_frontend::textures::Parse(tokenizer, texture_name),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Unsupported type for directive Texture: NotAType");
}

TEST(Parse, ConstantFloat) {
  std::stringstream input("\"name\" \"float\" \"constant\"");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  std::string texture_name;
  EXPECT_EQ(iris::pbrt_frontend::textures::g_float_constant_builder.get(),
            &iris::pbrt_frontend::textures::Parse(tokenizer, texture_name));
  EXPECT_EQ("name", texture_name);
}

TEST(Parse, ConstantColor) {
  std::stringstream input("\"name\" \"color\" \"constant\"");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  std::string texture_name;
  EXPECT_EQ(iris::pbrt_frontend::textures::g_spectrum_constant_builder.get(),
            &iris::pbrt_frontend::textures::Parse(tokenizer, texture_name));
  EXPECT_EQ("name", texture_name);
}

TEST(Parse, ConstantSpectrum) {
  std::stringstream input("\"name\" \"spectrum\" \"constant\"");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  std::string texture_name;
  EXPECT_EQ(iris::pbrt_frontend::textures::g_spectrum_constant_builder.get(),
            &iris::pbrt_frontend::textures::Parse(tokenizer, texture_name));
  EXPECT_EQ("name", texture_name);
}

TEST(Parse, Imageloat) {
  std::stringstream input("\"name\" \"float\" \"image\"");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  std::string texture_name;
  EXPECT_EQ(iris::pbrt_frontend::textures::g_float_image_builder.get(),
            &iris::pbrt_frontend::textures::Parse(tokenizer, texture_name));
  EXPECT_EQ("name", texture_name);
}

TEST(Parse, ImageSpectrum) {
  std::stringstream input("\"name\" \"spectrum\" \"image\"");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  std::string texture_name;
  EXPECT_EQ(iris::pbrt_frontend::textures::g_spectrum_image_builder.get(),
            &iris::pbrt_frontend::textures::Parse(tokenizer, texture_name));
  EXPECT_EQ("name", texture_name);
}

TEST(Parse, ScaleFloat) {
  std::stringstream input("\"name\" \"float\" \"scale\"");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  std::string texture_name;
  EXPECT_EQ(iris::pbrt_frontend::textures::g_float_scale_builder.get(),
            &iris::pbrt_frontend::textures::Parse(tokenizer, texture_name));
  EXPECT_EQ("name", texture_name);
}

TEST(Parse, ScaleSpectrum) {
  std::stringstream input("\"name\" \"spectrum\" \"scale\"");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  std::string texture_name;
  EXPECT_EQ(iris::pbrt_frontend::textures::g_spectrum_scale_builder.get(),
            &iris::pbrt_frontend::textures::Parse(tokenizer, texture_name));
  EXPECT_EQ("name", texture_name);
}