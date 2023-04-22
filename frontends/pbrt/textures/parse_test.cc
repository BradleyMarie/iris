#include "frontends/pbrt/textures/parse.h"

#include "googletest/include/gtest/gtest.h"

TEST(Parse, NoName) {
  std::stringstream input("");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  EXPECT_EXIT(iris::pbrt_frontend::textures::Parse(tokenizer),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Too few parameters to directive: Texture");
}

TEST(Parse, NameNotAString) {
  std::stringstream input("1.0");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  EXPECT_EXIT(iris::pbrt_frontend::textures::Parse(tokenizer),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Name of Texture must be a string");
}

TEST(Parse, NoValueType) {
  std::stringstream input("\"name\"");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  EXPECT_EXIT(iris::pbrt_frontend::textures::Parse(tokenizer),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Too few parameters to directive: Texture");
}

TEST(Parse, ValueTypeNotAString) {
  std::stringstream input("\"name\" 1.0");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  EXPECT_EXIT(iris::pbrt_frontend::textures::Parse(tokenizer),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Value type of Texture must be a string");
}

TEST(Parse, InvalidValueType) {
  std::stringstream input("\"name\" \"NotAType\"");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  EXPECT_EXIT(iris::pbrt_frontend::textures::Parse(tokenizer),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Unsupported value type for directive Texture: NotAType");
}

TEST(Parse, NoType) {
  std::stringstream input("\"name\" \"float\"");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  EXPECT_EXIT(iris::pbrt_frontend::textures::Parse(tokenizer),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Too few parameters to directive: Texture");
}

TEST(Parse, TypeNotAString) {
  std::stringstream input("\"name\" \"float\" 1.0");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  EXPECT_EXIT(iris::pbrt_frontend::textures::Parse(tokenizer),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Type of Texture must be a string");
}

TEST(Parse, InvalidFloatType) {
  std::stringstream input("\"name\" \"float\" \"NotAType\"");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  EXPECT_EXIT(iris::pbrt_frontend::textures::Parse(tokenizer),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Unsupported type for directive Texture: NotAType");
}

TEST(Parse, InvalidColorType) {
  std::stringstream input("\"name\" \"color\" \"NotAType\"");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  EXPECT_EXIT(iris::pbrt_frontend::textures::Parse(tokenizer),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Unsupported type for directive Texture: NotAType");
}

TEST(Parse, InvalidSpectrumType) {
  std::stringstream input("\"name\" \"spectrum\" \"NotAType\"");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  EXPECT_EXIT(iris::pbrt_frontend::textures::Parse(tokenizer),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Unsupported type for directive Texture: NotAType");
}