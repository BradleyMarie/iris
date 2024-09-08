#include "frontends/pbrt/materials/parse.h"

#include <sstream>

#include "frontends/pbrt/material_manager.h"
#include "frontends/pbrt/spectrum_managers/test_spectrum_manager.h"
#include "googletest/include/gtest/gtest.h"

namespace iris {
namespace pbrt_frontend {
namespace materials {
namespace {

using ::iris::pbrt_frontend::spectrum_managers::TestSpectrumManager;

TEST(Parse, TooFewParameters) {
  std::stringstream input("");
  Tokenizer tokenizer(input);
  EXPECT_EXIT(Parse(tokenizer), testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Too few parameters to directive: Material");
}

TEST(Parse, NotAString) {
  std::stringstream input("1.0");
  Tokenizer tokenizer(input);
  EXPECT_EXIT(Parse(tokenizer), testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Parameter to Material must be a string");
}

TEST(Parse, InvalidType) {
  std::stringstream input("\"NotAType\"");
  Tokenizer tokenizer(input);
  EXPECT_EXIT(Parse(tokenizer), testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Unsupported type for directive Material: NotAType");
}

TEST(Parse, Glass) {
  std::stringstream input("\"glass\"");
  Tokenizer tokenizer(input);
  Parse(tokenizer);
}

TEST(Parse, Matte) {
  std::stringstream input("\"matte\"");
  Tokenizer tokenizer(input);
  Parse(tokenizer);
}

TEST(Parse, Mirror) {
  std::stringstream input("\"mirror\"");
  Tokenizer tokenizer(input);
  Parse(tokenizer);
}

TEST(Parse, Mix) {
  std::stringstream input("\"mix\"");
  Tokenizer tokenizer(input);
  Parse(tokenizer);
}

TEST(Parse, Plastic) {
  std::stringstream input("\"plastic\"");
  Tokenizer tokenizer(input);
  Parse(tokenizer);
}

TEST(Parse, Uber) {
  std::stringstream input("\"uber\"");
  Tokenizer tokenizer(input);
  Parse(tokenizer);
}

TEST(ParseNamed, TooFewParameters) {
  std::stringstream input("");
  Tokenizer tokenizer(input);

  MaterialManager material_manager;
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager;

  EXPECT_EXIT(ParseNamed(tokenizer, std::filesystem::current_path(),
                         material_manager, spectrum_manager, texture_manager),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Too few parameters to directive: MakeNamedMaterial");
}

TEST(ParseNamed, NotAString) {
  std::stringstream input("1.0");
  Tokenizer tokenizer(input);

  MaterialManager material_manager;
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager;

  EXPECT_EXIT(ParseNamed(tokenizer, std::filesystem::current_path(),
                         material_manager, spectrum_manager, texture_manager),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Parameter to MakeNamedMaterial must be a string");
}

TEST(ParseNamed, Duplicated) {
  std::stringstream input("\"name\" \"float a\" 1.0 \"float a\" 1.0");
  Tokenizer tokenizer(input);

  MaterialManager material_manager;
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager;

  EXPECT_EXIT(ParseNamed(tokenizer, std::filesystem::current_path(),
                         material_manager, spectrum_manager, texture_manager),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: A parameter was specified twice: a");
}

TEST(ParseNamed, NoType) {
  std::stringstream input("\"name\" \"float a\" 1.0");
  Tokenizer tokenizer(input);

  MaterialManager material_manager;
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager;

  EXPECT_EXIT(ParseNamed(tokenizer, std::filesystem::current_path(),
                         material_manager, spectrum_manager, texture_manager),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Missing required parameter to MakeNamedMaterial: type");
}

TEST(ParseNamed, BadType) {
  std::stringstream input("\"name\" \"string type\" \"NotAType\"");
  Tokenizer tokenizer(input);

  MaterialManager material_manager;
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager;

  EXPECT_EXIT(
      ParseNamed(tokenizer, std::filesystem::current_path(), material_manager,
                 spectrum_manager, texture_manager),
      testing::ExitedWithCode(EXIT_FAILURE),
      "ERROR: Unsupported type for directive MakeNamedMaterial: NotAType");
}

TEST(ParseNamed, WithArgs) {
  std::stringstream input(
      "\"name\" \"string type\" \"matte\" \"float Kd\" 1.0 \"float sigma\" "
      "1.0");
  Tokenizer tokenizer(input);

  MaterialManager material_manager;
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager;

  ParseNamed(tokenizer, std::filesystem::current_path(), material_manager,
             spectrum_manager, texture_manager);
  EXPECT_NE(nullptr, material_manager.Get("name"));
}

TEST(ParseNamed, Glass) {
  std::stringstream input("\"name\" \"string type\" \"glass\"");
  Tokenizer tokenizer(input);

  MaterialManager material_manager;
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager;

  ParseNamed(tokenizer, std::filesystem::current_path(), material_manager,
             spectrum_manager, texture_manager);
  EXPECT_NE(nullptr, material_manager.Get("name"));
}

TEST(ParseNamed, Matte) {
  std::stringstream input("\"name\" \"string type\" \"matte\"");
  Tokenizer tokenizer(input);

  MaterialManager material_manager;
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager;

  ParseNamed(tokenizer, std::filesystem::current_path(), material_manager,
             spectrum_manager, texture_manager);
  EXPECT_NE(nullptr, material_manager.Get("name"));
}

TEST(ParseNamed, Mirror) {
  std::stringstream input("\"name\" \"string type\" \"mirror\"");
  Tokenizer tokenizer(input);

  MaterialManager material_manager;
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager;

  ParseNamed(tokenizer, std::filesystem::current_path(), material_manager,
             spectrum_manager, texture_manager);
  EXPECT_NE(nullptr, material_manager.Get("name"));
}

TEST(ParseNamed, Mix) {
  std::stringstream input("\"name\" \"string type\" \"mix\"");
  Tokenizer tokenizer(input);

  MaterialManager material_manager;
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager;

  EXPECT_EXIT(ParseNamed(tokenizer, std::filesystem::current_path(),
                         material_manager, spectrum_manager, texture_manager),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Missing required mix parameter: namedmaterial1");
}

TEST(ParseNamed, Plastic) {
  std::stringstream input("\"name\" \"string type\" \"plastic\"");
  Tokenizer tokenizer(input);

  MaterialManager material_manager;
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager;

  ParseNamed(tokenizer, std::filesystem::current_path(), material_manager,
             spectrum_manager, texture_manager);
  EXPECT_NE(nullptr, material_manager.Get("name"));
}

TEST(ParseNamed, Uber) {
  std::stringstream input("\"name\" \"string type\" \"uber\"");
  Tokenizer tokenizer(input);

  MaterialManager material_manager;
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager;

  ParseNamed(tokenizer, std::filesystem::current_path(), material_manager,
             spectrum_manager, texture_manager);
  EXPECT_NE(nullptr, material_manager.Get("name"));
}

TEST(Default, Default) {
  std::stringstream input("\"matte\"");
  Tokenizer tokenizer(input);
  EXPECT_EQ(&Parse(tokenizer), &Default());
}

}  // namespace
}  // namespace materials
}  // namespace pbrt_frontend
}  // namespace iris