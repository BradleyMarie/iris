#include "frontends/pbrt/cameras/parse.h"

#include "googletest/include/gtest/gtest.h"

namespace iris {
namespace pbrt_frontend {
namespace cameras {
namespace {

using ::testing::ExitedWithCode;

TEST(Parse, TooFewParameters) {
  std::stringstream input("");
  Tokenizer tokenizer(input);
  EXPECT_EXIT(Parse(tokenizer), ExitedWithCode(EXIT_FAILURE),
              "ERROR: Too few parameters to directive: Camera");
}

TEST(Parse, NotAString) {
  std::stringstream input("1.0");
  Tokenizer tokenizer(input);
  EXPECT_EXIT(Parse(tokenizer), ExitedWithCode(EXIT_FAILURE),
              "ERROR: Parameter to Camera must be a string");
}

TEST(Parse, InvalidType) {
  std::stringstream input("\"NotAType\"");
  Tokenizer tokenizer(input);
  EXPECT_EXIT(Parse(tokenizer), ExitedWithCode(EXIT_FAILURE),
              "ERROR: Unsupported type for directive Camera: NotAType");
}

TEST(Parse, Orthographic) {
  std::stringstream input("\"orthographic\"");
  Tokenizer tokenizer(input);
  Parse(tokenizer);
}

TEST(Parse, Perspective) {
  std::stringstream input("\"perspective\"");
  Tokenizer tokenizer(input);
  Parse(tokenizer);
}

TEST(Default, Default) {
  std::stringstream input("\"perspective\"");
  Tokenizer tokenizer(input);
  EXPECT_EQ(&Parse(tokenizer), &Default());
}

}  // namespace
}  // namespace cameras
}  // namespace pbrt_frontend
}  // namespace iris