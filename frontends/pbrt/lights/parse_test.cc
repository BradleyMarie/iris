#include "frontends/pbrt/lights/parse.h"

#include "frontends/pbrt/lights/infinite.h"
#include "googletest/include/gtest/gtest.h"

TEST(Parse, TooFewParameters) {
  std::stringstream input("");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  EXPECT_EXIT(iris::pbrt_frontend::lights::Parse(tokenizer),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Too few parameters to directive: LightSource");
}

TEST(Parse, NotAString) {
  std::stringstream input("1.0");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  EXPECT_EXIT(iris::pbrt_frontend::lights::Parse(tokenizer),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Parameter to LightSource must be a string");
}

TEST(Parse, InvalidType) {
  std::stringstream input("\"NotAType\"");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  EXPECT_EXIT(iris::pbrt_frontend::lights::Parse(tokenizer),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Unsupported type for directive LightSource: NotAType");
}

TEST(Parse, Infinite) {
  std::stringstream input("\"infinite\"");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  EXPECT_EQ(iris::pbrt_frontend::lights::g_infinite_builder.get(),
            &iris::pbrt_frontend::lights::Parse(tokenizer));
}