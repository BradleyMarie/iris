#include "frontends/pbrt/materials/parse.h"

#include "frontends/pbrt/spectrum_managers/test_spectrum_manager.h"
#include "googletest/include/gtest/gtest.h"

TEST(Parse, TooFewParameters) {
  std::stringstream input("");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  EXPECT_EXIT(iris::pbrt_frontend::materials::Parse(tokenizer),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Too few parameters to directive: Material");
}

TEST(Parse, NotAString) {
  std::stringstream input("1.0");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  EXPECT_EXIT(iris::pbrt_frontend::materials::Parse(tokenizer),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Parameter to Material must be a string");
}

TEST(Parse, InvalidType) {
  std::stringstream input("\"NotAType\"");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  EXPECT_EXIT(iris::pbrt_frontend::materials::Parse(tokenizer),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Unsupported type for directive Material: NotAType");
}

TEST(Parse, Glass) {
  std::stringstream input("\"glass\"");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::materials::Parse(tokenizer);
}

TEST(Parse, Matte) {
  std::stringstream input("\"matte\"");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::materials::Parse(tokenizer);
}

TEST(Parse, Mirror) {
  std::stringstream input("\"mirror\"");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::materials::Parse(tokenizer);
}

TEST(Parse, Plastic) {
  std::stringstream input("\"plastic\"");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::materials::Parse(tokenizer);
}

TEST(ParseNamed, TooFewParameters) {
  std::stringstream input("");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::MaterialManager material_manager;
  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;

  EXPECT_EXIT(iris::pbrt_frontend::materials::ParseNamed(
                  tokenizer, std::filesystem::current_path(), material_manager,
                  spectrum_manager, texture_manager),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Too few parameters to directive: MakeNamedMaterial");
}

TEST(ParseNamed, NotAString) {
  std::stringstream input("1.0");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::MaterialManager material_manager;
  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;

  EXPECT_EXIT(iris::pbrt_frontend::materials::ParseNamed(
                  tokenizer, std::filesystem::current_path(), material_manager,
                  spectrum_manager, texture_manager),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Parameter to MakeNamedMaterial must be a string");
}

TEST(ParseNamed, Duplicated) {
  std::stringstream input("\"name\" \"float a\" 1.0 \"float a\" 1.0");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::MaterialManager material_manager;
  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;

  EXPECT_EXIT(iris::pbrt_frontend::materials::ParseNamed(
                  tokenizer, std::filesystem::current_path(), material_manager,
                  spectrum_manager, texture_manager),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: A parameter was specified twice: a");
}

TEST(ParseNamed, NoType) {
  std::stringstream input("\"name\" \"float a\" 1.0");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::MaterialManager material_manager;
  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;

  EXPECT_EXIT(iris::pbrt_frontend::materials::ParseNamed(
                  tokenizer, std::filesystem::current_path(), material_manager,
                  spectrum_manager, texture_manager),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Missing required parameter to MakeNamedMaterial: type");
}

TEST(ParseNamed, BadType) {
  std::stringstream input("\"name\" \"string type\" \"NotAType\"");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::MaterialManager material_manager;
  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;

  EXPECT_EXIT(
      iris::pbrt_frontend::materials::ParseNamed(
          tokenizer, std::filesystem::current_path(), material_manager,
          spectrum_manager, texture_manager),
      testing::ExitedWithCode(EXIT_FAILURE),
      "ERROR: Unsupported type for directive MakeNamedMaterial: NotAType");
}

TEST(ParseNamed, WithArgs) {
  std::stringstream input(
      "\"name\" \"string type\" \"matte\" \"float Kd\" 1.0 \"float sigma\" "
      "1.0");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::MaterialManager material_manager;
  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;

  iris::pbrt_frontend::materials::ParseNamed(
      tokenizer, std::filesystem::current_path(), material_manager,
      spectrum_manager, texture_manager);
  EXPECT_NE(nullptr, material_manager.Get("name"));
}

TEST(ParseNamed, Glass) {
  std::stringstream input("\"name\" \"string type\" \"glass\"");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::MaterialManager material_manager;
  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;

  iris::pbrt_frontend::materials::ParseNamed(
      tokenizer, std::filesystem::current_path(), material_manager,
      spectrum_manager, texture_manager);
  EXPECT_NE(nullptr, material_manager.Get("name"));
}

TEST(ParseNamed, Matte) {
  std::stringstream input("\"name\" \"string type\" \"matte\"");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::MaterialManager material_manager;
  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;

  iris::pbrt_frontend::materials::ParseNamed(
      tokenizer, std::filesystem::current_path(), material_manager,
      spectrum_manager, texture_manager);
  EXPECT_NE(nullptr, material_manager.Get("name"));
}

TEST(ParseNamed, Mirror) {
  std::stringstream input("\"name\" \"string type\" \"mirror\"");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::MaterialManager material_manager;
  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;

  iris::pbrt_frontend::materials::ParseNamed(
      tokenizer, std::filesystem::current_path(), material_manager,
      spectrum_manager, texture_manager);
  EXPECT_NE(nullptr, material_manager.Get("name"));
}

TEST(ParseNamed, Plastic) {
  std::stringstream input("\"name\" \"string type\" \"plastic\"");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::MaterialManager material_manager;
  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;

  iris::pbrt_frontend::materials::ParseNamed(
      tokenizer, std::filesystem::current_path(), material_manager,
      spectrum_manager, texture_manager);
  EXPECT_NE(nullptr, material_manager.Get("name"));
}

TEST(Default, Default) {
  std::stringstream input("\"matte\"");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  EXPECT_EQ(&iris::pbrt_frontend::materials::Parse(tokenizer),
            &iris::pbrt_frontend::materials::Default());
}