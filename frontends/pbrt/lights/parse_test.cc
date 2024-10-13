#include "frontends/pbrt/lights/parse.h"

#include "frontends/pbrt/lights/distant.h"
#include "frontends/pbrt/lights/infinite.h"
#include "googletest/include/gtest/gtest.h"

namespace iris {
namespace pbrt_frontend {
namespace lights {
namespace {

using ::testing::ExitedWithCode;

TEST(Parse, TooFewParameters) {
  std::stringstream input("");
  Tokenizer tokenizer(input);
  EXPECT_EXIT(Parse(tokenizer), ExitedWithCode(EXIT_FAILURE),
              "ERROR: Too few parameters to directive: LightSource");
}

TEST(Parse, NotAString) {
  std::stringstream input("1.0");
  Tokenizer tokenizer(input);
  EXPECT_EXIT(Parse(tokenizer), ExitedWithCode(EXIT_FAILURE),
              "ERROR: Parameter to LightSource must be a string");
}

TEST(Parse, InvalidType) {
  std::stringstream input("\"NotAType\"");
  Tokenizer tokenizer(input);
  EXPECT_EXIT(Parse(tokenizer), ExitedWithCode(EXIT_FAILURE),
              "ERROR: Unsupported type for directive LightSource: NotAType");
}

TEST(Parse, Distant) {
  std::stringstream input("\"distant\"");
  Tokenizer tokenizer(input);
  EXPECT_EQ(g_distant_builder.get(), &Parse(tokenizer));
}

TEST(Parse, Infinite) {
  std::stringstream input("\"infinite\"");
  Tokenizer tokenizer(input);
  EXPECT_EQ(g_infinite_builder.get(), &Parse(tokenizer));
}

}  // namespace
}  // namespace lights
}  // namespace pbrt_frontend
}  // namespace iris