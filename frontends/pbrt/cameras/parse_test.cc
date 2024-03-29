#include "frontends/pbrt/cameras/parse.h"

#include "googletest/include/gtest/gtest.h"

TEST(Parse, TooFewParameters) {
  std::stringstream input("");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  EXPECT_EXIT(iris::pbrt_frontend::cameras::Parse(tokenizer),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Too few parameters to directive: Camera");
}

TEST(Parse, NotAString) {
  std::stringstream input("1.0");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  EXPECT_EXIT(iris::pbrt_frontend::cameras::Parse(tokenizer),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Parameter to Camera must be a string");
}

TEST(Parse, InvalidType) {
  std::stringstream input("\"NotAType\"");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  EXPECT_EXIT(iris::pbrt_frontend::cameras::Parse(tokenizer),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Unsupported type for directive Camera: NotAType");
}

TEST(Parse, Orthographic) {
  std::stringstream input("\"orthographic\"");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::cameras::Parse(tokenizer);
}

TEST(Parse, Perspective) {
  std::stringstream input("\"perspective\"");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::cameras::Parse(tokenizer);
}

TEST(Default, Default) {
  std::stringstream input("\"perspective\"");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  EXPECT_EQ(&iris::pbrt_frontend::cameras::Parse(tokenizer),
            &iris::pbrt_frontend::cameras::Default());
}