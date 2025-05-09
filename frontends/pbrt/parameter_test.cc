#include "frontends/pbrt/parameter.h"

#include "frontends/pbrt/spectrum_managers/test_spectrum_manager.h"
#include "googletest/include/gtest/gtest.h"
#include "tools/cpp/runfiles/runfiles.h"

using bazel::tools::cpp::runfiles::Runfiles;

std::string UnquotedRunfilePath(const std::string& path) {
  std::unique_ptr<Runfiles> runfiles(Runfiles::CreateForTest());
  const char* base_path = "_main/frontends/pbrt/test_data/";
  return runfiles->Rlocation(base_path + path);
}
std::string RunfilePath(const std::string& path) {
  return std::string("\"") + UnquotedRunfilePath(path) + std::string("\"");
}

TEST(Parameter, BoolWrongType) {
  std::stringstream input("\"integer name\" [1]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  EXPECT_EXIT(
      parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                         iris::pbrt_frontend::Parameter::BOOL, spectrum_manager,
                         texture_manager),
      testing::ExitedWithCode(EXIT_FAILURE),
      "ERROR: Wrong type for parameter list: name");
}

TEST(Parameter, BoolTooFew) {
  std::stringstream input("\"bool name\" [\"true\" \"true\"]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                     iris::pbrt_frontend::Parameter::BOOL, spectrum_manager,
                     texture_manager);

  EXPECT_EXIT(parameter.GetBoolValues(0u, 3u),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Too few values in parameter list: name");
}

TEST(Parameter, BoolTooMany) {
  std::stringstream input("\"bool name\" [\"true\" \"true\"]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                     iris::pbrt_frontend::Parameter::BOOL, spectrum_manager,
                     texture_manager);

  EXPECT_EXIT(parameter.GetBoolValues(1u, 1u),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Too many values in parameter list: name");
}

TEST(Parameter, Bool) {
  std::stringstream input("\"bool name\" [\"true\"]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                     iris::pbrt_frontend::Parameter::BOOL, spectrum_manager,
                     texture_manager);

  ASSERT_EQ(1u, parameter.GetBoolValues(1u, 1u).size());
  EXPECT_TRUE(parameter.GetBoolValues(1u, 1u).at(0));
}

TEST(Parameter, FilePathWrongType) {
  std::stringstream input("\"integer name\" [1]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  EXPECT_EXIT(
      parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                         iris::pbrt_frontend::Parameter::FILE_PATH,
                         spectrum_manager, texture_manager),
      testing::ExitedWithCode(EXIT_FAILURE),
      "ERROR: Wrong type for parameter list: name");
}

TEST(Parameter, FilePathTooFew) {
  std::stringstream input("\"string name\" [ " +
                          RunfilePath("include_empty.pbrt") + " " +
                          RunfilePath("include_empty.pbrt") + "]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                     iris::pbrt_frontend::Parameter::FILE_PATH,
                     spectrum_manager, texture_manager);

  EXPECT_EXIT(parameter.GetFilePaths(0u, 3u),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Too few values in parameter list: name");
}

TEST(Parameter, FilePathTooMany) {
  std::stringstream input("\"string name\" [ " +
                          RunfilePath("include_empty.pbrt") + " " +
                          RunfilePath("include_empty.pbrt") + "]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                     iris::pbrt_frontend::Parameter::FILE_PATH,
                     spectrum_manager, texture_manager);

  EXPECT_EXIT(parameter.GetFilePaths(1u, 1u),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Too many values in parameter list: name");
}

TEST(Parameter, FilePathDoesNotExist) {
  std::stringstream input("\"string name\" [\"notarealpath\"]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  EXPECT_EXIT(
      parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                         iris::pbrt_frontend::Parameter::FILE_PATH,
                         spectrum_manager, texture_manager),
      testing::ExitedWithCode(EXIT_FAILURE),
      "ERROR: Could not find file specified in parameter list: name");
}

TEST(Parameter, FilePathT) {
  std::stringstream input("\"string name\" [" +
                          RunfilePath("include_empty.pbrt") + "]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                     iris::pbrt_frontend::Parameter::FILE_PATH,
                     spectrum_manager, texture_manager);

  EXPECT_EQ(1u, parameter.GetFilePaths(1u, 1u).size());
  EXPECT_EQ(std::filesystem::weakly_canonical(
                UnquotedRunfilePath("include_empty.pbrt")),
            parameter.GetFilePaths(1u, 1u).at(0));
}

TEST(Parameter, FloatWrongType) {
  std::stringstream input("\"integer name\" [1]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  EXPECT_EXIT(
      parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                         iris::pbrt_frontend::Parameter::FLOAT,
                         spectrum_manager, texture_manager),
      testing::ExitedWithCode(EXIT_FAILURE),
      "ERROR: Wrong type for parameter list: name");
}

TEST(Parameter, FloatTooFew) {
  std::stringstream input("\"float name\" [1.0 1.0]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                     iris::pbrt_frontend::Parameter::FLOAT, spectrum_manager,
                     texture_manager);

  EXPECT_EXIT(parameter.GetFloatValues(0u, 3u),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Too few values in parameter list: name");
}

TEST(Parameter, FloatTooMany) {
  std::stringstream input("\"float name\" [1.0 1.0]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                     iris::pbrt_frontend::Parameter::FLOAT, spectrum_manager,
                     texture_manager);

  EXPECT_EXIT(parameter.GetFloatValues(1u, 1u),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Too many values in parameter list: name");
}

TEST(Parameter, Float) {
  std::stringstream input("\"float name\" [1.0]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                     iris::pbrt_frontend::Parameter::FLOAT, spectrum_manager,
                     texture_manager);

  ASSERT_EQ(1u, parameter.GetFloatValues(1u, 1u).size());
  EXPECT_EQ(1.0, parameter.GetFloatValues(1u, 1u).at(0));
}

TEST(Parameter, FloatTextureWrongType) {
  std::stringstream input("\"integer name\" [3e39]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  EXPECT_EXIT(
      parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                         iris::pbrt_frontend::Parameter::FLOAT_TEXTURE,
                         spectrum_manager, texture_manager),
      testing::ExitedWithCode(EXIT_FAILURE),
      "ERROR: Wrong type for parameter list: name");
}

TEST(Parameter, FloatTextureOutOfRange) {
  std::stringstream input("\"float name\" [3e39]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  EXPECT_EXIT(
      parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                         iris::pbrt_frontend::Parameter::FLOAT_TEXTURE,
                         spectrum_manager, texture_manager),
      testing::ExitedWithCode(EXIT_FAILURE),
      "ERROR: Out of range value in parameter list: name");
}

TEST(Parameter, FloatTextureTooFew) {
  std::stringstream input("\"float name\" [1.0]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                     iris::pbrt_frontend::Parameter::FLOAT_TEXTURE,
                     spectrum_manager, texture_manager);

  EXPECT_EXIT(parameter.GetFloatTextures(2u, 2u),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Too few values in parameter list: name");
}

TEST(Parameter, FloatTextureFloat) {
  std::stringstream input("\"float name\" [1.0]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                     iris::pbrt_frontend::Parameter::FLOAT_TEXTURE,
                     spectrum_manager, texture_manager);

  EXPECT_EQ(1u, parameter.GetFloatTextures(1u, 1u).size());
  EXPECT_NE(nullptr, parameter.GetFloatTextures(1u, 1u).at(0).Get());
}

TEST(Parameter, FloatTextureTexture) {
  std::stringstream input(
      "\"float name\" [1.0] \"texture name\" [\"texture\"]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                     iris::pbrt_frontend::Parameter::FLOAT_TEXTURE,
                     spectrum_manager, texture_manager);

  EXPECT_EQ(1u, parameter.GetFloatTextures(1u, 1u).size());
  auto texture = parameter.GetFloatTextures(1u, 1u).at(0);

  texture_manager.Put("texture", texture);

  EXPECT_EQ(1u, parameter.GetFloatTextures(1u, 1u).size());
  EXPECT_NE(nullptr, parameter.GetFloatTextures(1u, 1u).at(0).Get());
}

TEST(Parameter, FloatTextureTooMany) {
  std::stringstream input("\"float name\" [1.0 1.0 1.0]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                     iris::pbrt_frontend::Parameter::FLOAT_TEXTURE,
                     spectrum_manager, texture_manager);

  EXPECT_EXIT(parameter.GetFloatTextures(1u, 1u),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Too many values in parameter list: name");
}

TEST(Parameter, IntegerWrongType) {
  std::stringstream input("\"float name\" [1]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  EXPECT_EXIT(
      parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                         iris::pbrt_frontend::Parameter::INTEGER,
                         spectrum_manager, texture_manager),
      testing::ExitedWithCode(EXIT_FAILURE),
      "ERROR: Wrong type for parameter list: name");
}

TEST(Parameter, IntegerTooFew) {
  std::stringstream input("\"integer name\" [1 1]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                     iris::pbrt_frontend::Parameter::INTEGER, spectrum_manager,
                     texture_manager);

  EXPECT_EXIT(parameter.GetIntegerValues(0u, 3u),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Too few values in parameter list: name");
}

TEST(Parameter, IntegerTooMany) {
  std::stringstream input("\"integer name\" [1 1]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                     iris::pbrt_frontend::Parameter::INTEGER, spectrum_manager,
                     texture_manager);

  EXPECT_EXIT(parameter.GetIntegerValues(1u, 1u),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Too many values in parameter list: name");
}

TEST(Parameter, Integer) {
  std::stringstream input("\"integer name\" [1]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                     iris::pbrt_frontend::Parameter::INTEGER, spectrum_manager,
                     texture_manager);

  ASSERT_EQ(1u, parameter.GetIntegerValues(1u, 1u).size());
  EXPECT_EQ(1, parameter.GetIntegerValues(1u, 1u).at(0));
}

TEST(Parameter, NormalWrongType) {
  std::stringstream input("\"float name\" [1]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  EXPECT_EXIT(
      parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                         iris::pbrt_frontend::Parameter::NORMAL,
                         spectrum_manager, texture_manager),
      testing::ExitedWithCode(EXIT_FAILURE),
      "ERROR: Wrong type for parameter list: name");
}

TEST(Parameter, NormalTooFew) {
  std::stringstream input("\"normal name\" [1 1 1]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                     iris::pbrt_frontend::Parameter::NORMAL, spectrum_manager,
                     texture_manager);

  EXPECT_EXIT(parameter.GetNormalValues(0u, 3u),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Too few values in parameter list: name");
}

TEST(Parameter, NormalTooMany) {
  std::stringstream input("\"normal name\" [1 1 1 1 1 1 1 1 1]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                     iris::pbrt_frontend::Parameter::NORMAL, spectrum_manager,
                     texture_manager);

  EXPECT_EXIT(parameter.GetNormalValues(1u, 1u),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Too many values in parameter list: name");
}

TEST(Parameter, Normal) {
  std::stringstream input("\"normal name\" [1 1 1]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                     iris::pbrt_frontend::Parameter::NORMAL, spectrum_manager,
                     texture_manager);

  ASSERT_EQ(1u, parameter.GetNormalValues(1u, 1u).size());
  EXPECT_EQ(iris::Vector(1.0, 1.0, 1.0),
            parameter.GetNormalValues(1u, 1u).at(0));
}

TEST(Parameter, Point3WrongType) {
  std::stringstream input("\"float name\" [1]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  EXPECT_EXIT(
      parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                         iris::pbrt_frontend::Parameter::POINT3,
                         spectrum_manager, texture_manager),
      testing::ExitedWithCode(EXIT_FAILURE),
      "ERROR: Wrong type for parameter list: name");
}

TEST(Parameter, Point3TooFew) {
  std::stringstream input("\"point3 name\" [1 1 1]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                     iris::pbrt_frontend::Parameter::POINT3, spectrum_manager,
                     texture_manager);

  EXPECT_EXIT(parameter.GetPoint3Values(0u, 3u),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Too few values in parameter list: name");
}

TEST(Parameter, Point3TooMany) {
  std::stringstream input("\"point3 name\" [1 1 1 1 1 1 1 1 1]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                     iris::pbrt_frontend::Parameter::POINT3, spectrum_manager,
                     texture_manager);

  EXPECT_EXIT(parameter.GetPoint3Values(1u, 1u),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Too many values in parameter list: name");
}

TEST(Parameter, Point3) {
  std::stringstream input("\"point3 name\" [1 1 1]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                     iris::pbrt_frontend::Parameter::POINT3, spectrum_manager,
                     texture_manager);

  ASSERT_EQ(1u, parameter.GetPoint3Values(1u, 1u).size());
  EXPECT_EQ(iris::Point(1.0, 1.0, 1.0),
            parameter.GetPoint3Values(1u, 1u).at(0));
}

TEST(Parameter, ReflectorWrongType) {
  std::stringstream input("\"integer name\" [1]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  EXPECT_EXIT(
      parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                         iris::pbrt_frontend::Parameter::REFLECTOR,
                         spectrum_manager, texture_manager),
      testing::ExitedWithCode(EXIT_FAILURE),
      "ERROR: Wrong type for parameter list: name");
}

TEST(Parameter, ReflectorFloatOutOfRangeNegative) {
  std::stringstream input("\"float name\" [-2]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  EXPECT_EXIT(
      parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                         iris::pbrt_frontend::Parameter::REFLECTOR,
                         spectrum_manager, texture_manager),
      testing::ExitedWithCode(EXIT_FAILURE),
      "ERROR: Out of range value in parameter list: name");
}

TEST(Parameter, ReflectorFloatOutOfRangePositive) {
  std::stringstream input("\"float name\" [2]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  EXPECT_EXIT(
      parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                         iris::pbrt_frontend::Parameter::REFLECTOR,
                         spectrum_manager, texture_manager),
      testing::ExitedWithCode(EXIT_FAILURE),
      "ERROR: Out of range value in parameter list: name");
}

TEST(Parameter, ReflectorOutOfRange) {
  std::stringstream input("\"spectrum name\" [1 2]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  EXPECT_EXIT(
      parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                         iris::pbrt_frontend::Parameter::REFLECTOR,
                         spectrum_manager, texture_manager),
      testing::ExitedWithCode(EXIT_FAILURE),
      "ERROR: Out of range value in parameter list: name");
}

TEST(Parameter, ReflectorOutOfRangeX) {
  std::stringstream input("\"rgb name\" [2 1 1]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  EXPECT_EXIT(
      parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                         iris::pbrt_frontend::Parameter::REFLECTOR,
                         spectrum_manager, texture_manager),
      testing::ExitedWithCode(EXIT_FAILURE),
      "ERROR: Out of range value in parameter list: name");
}

TEST(Parameter, ReflectorOutOfRangeY) {
  std::stringstream input("\"rgb name\" [1 2 1]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  EXPECT_EXIT(
      parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                         iris::pbrt_frontend::Parameter::REFLECTOR,
                         spectrum_manager, texture_manager),
      testing::ExitedWithCode(EXIT_FAILURE),
      "ERROR: Out of range value in parameter list: name");
}

TEST(Parameter, ReflectorOutOfRangeZ) {
  std::stringstream input("\"rgb name\" [1 1 2]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  EXPECT_EXIT(
      parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                         iris::pbrt_frontend::Parameter::REFLECTOR,
                         spectrum_manager, texture_manager),
      testing::ExitedWithCode(EXIT_FAILURE),
      "ERROR: Out of range value in parameter list: name");
}

TEST(Parameter, ReflectorFloat) {
  std::stringstream input("\"float name\" [1]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                     iris::pbrt_frontend::Parameter::REFLECTOR,
                     spectrum_manager, texture_manager);

  EXPECT_EQ(1u, parameter.GetReflectors(1u, 1u).size());
  EXPECT_NE(nullptr, parameter.GetReflectors(1u, 1u).at(0).Get());
}

TEST(Parameter, ReflectorColor) {
  std::stringstream input("\"rgb name\" [1 1 1]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                     iris::pbrt_frontend::Parameter::REFLECTOR,
                     spectrum_manager, texture_manager);

  EXPECT_EQ(1u, parameter.GetReflectors(1u, 1u).size());
  EXPECT_NE(nullptr, parameter.GetReflectors(1u, 1u).at(0).Get());
}

TEST(Parameter, ReflectorSpectrum) {
  std::stringstream input("\"spectrum name\" [1 1]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                     iris::pbrt_frontend::Parameter::REFLECTOR,
                     spectrum_manager, texture_manager);

  EXPECT_EQ(1u, parameter.GetReflectors(1u, 1u).size());
  EXPECT_NE(nullptr, parameter.GetReflectors(1u, 1u).at(0).Get());
}

TEST(Parameter, ReflectorFileNotFound) {
  std::stringstream input("\"spectrum name\" [\"notafile\"]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  EXPECT_EXIT(
      parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                         iris::pbrt_frontend::Parameter::REFLECTOR,
                         spectrum_manager, texture_manager),
      testing::ExitedWithCode(EXIT_FAILURE),
      "ERROR: Could not find file specified in parameter list: name");
}

TEST(Parameter, ReflectorEmissiveSpd) {
  std::string path = RunfilePath("emissive.spd");
  std::stringstream input("\"spectrum name\" [" + path + "]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  EXPECT_EXIT(
      parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                         iris::pbrt_frontend::Parameter::REFLECTOR,
                         spectrum_manager, texture_manager),
      testing::ExitedWithCode(EXIT_FAILURE),
      "ERROR: SPD file parsing failed with error: The input contained a sample "
      "with a spectral power greater than one");
}

TEST(Parameter, ReflectorReflectiveSpd) {
  std::string path = RunfilePath("reflective.spd");
  std::stringstream input("\"spectrum name\" [" + path + " " + path + "]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                     iris::pbrt_frontend::Parameter::REFLECTOR,
                     spectrum_manager, texture_manager);
  EXPECT_EQ(2u, parameter.GetReflectors(2u, 2u).size());
  EXPECT_NE(nullptr, parameter.GetReflectors(2u, 2u).at(0).Get());
  EXPECT_NE(nullptr, parameter.GetReflectors(2u, 2u).at(1).Get());
}

TEST(Parameter, ReflectorTextureWrongType) {
  std::stringstream input("\"integer name\" [1]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  EXPECT_EXIT(
      parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                         iris::pbrt_frontend::Parameter::REFLECTOR_TEXTURE,
                         spectrum_manager, texture_manager),
      testing::ExitedWithCode(EXIT_FAILURE),
      "ERROR: Wrong type for parameter list: name");
}

TEST(Parameter, ReflectorFloatTextureOutOfRangeNegative) {
  std::stringstream input("\"float name\" [-2]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  EXPECT_EXIT(
      parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                         iris::pbrt_frontend::Parameter::REFLECTOR_TEXTURE,
                         spectrum_manager, texture_manager),
      testing::ExitedWithCode(EXIT_FAILURE),
      "ERROR: Out of range value in parameter list: name");
}

TEST(Parameter, ReflectorFloatTextureOutOfRangePositive) {
  std::stringstream input("\"float name\" [2]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  EXPECT_EXIT(
      parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                         iris::pbrt_frontend::Parameter::REFLECTOR_TEXTURE,
                         spectrum_manager, texture_manager),
      testing::ExitedWithCode(EXIT_FAILURE),
      "ERROR: Out of range value in parameter list: name");
}

TEST(Parameter, ReflectorTextureOutOfRange) {
  std::stringstream input("\"spectrum name\" [1 2]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  EXPECT_EXIT(
      parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                         iris::pbrt_frontend::Parameter::REFLECTOR_TEXTURE,
                         spectrum_manager, texture_manager),
      testing::ExitedWithCode(EXIT_FAILURE),
      "ERROR: Out of range value in parameter list: name");
}

TEST(Parameter, ReflectorTextureOutOfRangeX) {
  std::stringstream input("\"rgb name\" [2 1 1]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  EXPECT_EXIT(
      parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                         iris::pbrt_frontend::Parameter::REFLECTOR_TEXTURE,
                         spectrum_manager, texture_manager),
      testing::ExitedWithCode(EXIT_FAILURE),
      "ERROR: Out of range value in parameter list: name");
}

TEST(Parameter, ReflectorTextureOutOfRangeY) {
  std::stringstream input("\"rgb name\" [1 2 1]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  EXPECT_EXIT(
      parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                         iris::pbrt_frontend::Parameter::REFLECTOR_TEXTURE,
                         spectrum_manager, texture_manager),
      testing::ExitedWithCode(EXIT_FAILURE),
      "ERROR: Out of range value in parameter list: name");
}

TEST(Parameter, ReflectorTextureOutOfRangeZ) {
  std::stringstream input("\"rgb name\" [1 1 2]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  EXPECT_EXIT(
      parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                         iris::pbrt_frontend::Parameter::REFLECTOR_TEXTURE,
                         spectrum_manager, texture_manager),
      testing::ExitedWithCode(EXIT_FAILURE),
      "ERROR: Out of range value in parameter list: name");
}

TEST(Parameter, ReflectorTextureTooFew) {
  std::stringstream input(
      "\"spectrum name\" [1 1] \"texture name\" [\"texture0\" \"texture1\"]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                     iris::pbrt_frontend::Parameter::REFLECTOR_TEXTURE,
                     spectrum_manager, texture_manager);

  EXPECT_EQ(1u, parameter.GetReflectorTextures(1u, 1u).size());
  auto texture = parameter.GetReflectorTextures(1u, 1u).at(0);

  texture_manager.Put("texture0", texture);
  texture_manager.Put("texture1", texture);

  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));
  parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                     iris::pbrt_frontend::Parameter::REFLECTOR_TEXTURE,
                     spectrum_manager, texture_manager);
  EXPECT_EXIT(parameter.GetReflectorTextures(3u, 3u),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Too few values in parameter list: name");
}

TEST(Parameter, ReflectorTextureTooMany) {
  std::stringstream input(
      "\"spectrum name\" [1 1] \"texture name\" [\"texture0\" \"texture1\"]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                     iris::pbrt_frontend::Parameter::REFLECTOR_TEXTURE,
                     spectrum_manager, texture_manager);

  EXPECT_EQ(1u, parameter.GetReflectorTextures(1u, 1u).size());
  auto texture = parameter.GetReflectorTextures(1u, 1u).at(0);

  texture_manager.Put("texture0", texture);
  texture_manager.Put("texture1", texture);

  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));
  parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                     iris::pbrt_frontend::Parameter::REFLECTOR_TEXTURE,
                     spectrum_manager, texture_manager);
  EXPECT_EXIT(parameter.GetReflectorTextures(1u, 1u),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Too many values in parameter list: name");
}

TEST(Parameter, ReflectorTextureFileNotFound) {
  std::stringstream input("\"spectrum name\" [\"notafile\"]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  EXPECT_EXIT(
      parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                         iris::pbrt_frontend::Parameter::REFLECTOR_TEXTURE,
                         spectrum_manager, texture_manager),
      testing::ExitedWithCode(EXIT_FAILURE),
      "ERROR: Could not find file specified in parameter list: name");
}

TEST(Parameter, ReflectorTextureEmissiveSpd) {
  std::string path = RunfilePath("emissive.spd");
  std::stringstream input("\"spectrum name\" [" + path + "]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  EXPECT_EXIT(
      parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                         iris::pbrt_frontend::Parameter::REFLECTOR_TEXTURE,
                         spectrum_manager, texture_manager),
      testing::ExitedWithCode(EXIT_FAILURE),
      "ERROR: SPD file parsing failed with error: The input contained a sample "
      "with a spectral power greater than one");
}

TEST(Parameter, ReflectorTextureReflectiveSpd) {
  std::string path = RunfilePath("reflective.spd");
  std::stringstream input("\"spectrum name\" [" + path + " " + path + "]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                     iris::pbrt_frontend::Parameter::REFLECTOR_TEXTURE,
                     spectrum_manager, texture_manager);
  EXPECT_EQ(2u, parameter.GetReflectorTextures(2u, 2u).size());
  EXPECT_NE(nullptr, parameter.GetReflectorTextures(2u, 2u).at(0).Get());
  EXPECT_NE(nullptr, parameter.GetReflectorTextures(2u, 2u).at(1).Get());
}

TEST(Parameter, ReflectorFloatTexture) {
  std::stringstream input("\"float name\" [1]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                     iris::pbrt_frontend::Parameter::REFLECTOR_TEXTURE,
                     spectrum_manager, texture_manager);

  EXPECT_EQ(1u, parameter.GetReflectorTextures(1u, 1u).size());
  EXPECT_NE(nullptr, parameter.GetReflectorTextures(1u, 1u).at(0).Get());
}

TEST(Parameter, ReflectorColorTexture) {
  std::stringstream input("\"rgb name\" [1 1 1]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                     iris::pbrt_frontend::Parameter::REFLECTOR_TEXTURE,
                     spectrum_manager, texture_manager);

  EXPECT_EQ(1u, parameter.GetReflectorTextures(1u, 1u).size());
  EXPECT_NE(nullptr, parameter.GetReflectorTextures(1u, 1u).at(0).Get());
}

TEST(Parameter, ReflectorTexture) {
  std::stringstream input(
      "\"spectrum name\" [1 1] \"texture name\" [\"texture0\"]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                     iris::pbrt_frontend::Parameter::REFLECTOR_TEXTURE,
                     spectrum_manager, texture_manager);

  EXPECT_EQ(1u, parameter.GetReflectorTextures(1u, 1u).size());
  auto texture = parameter.GetReflectorTextures(1u, 1u).at(0);

  texture_manager.Put("texture0", texture);

  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));
  parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                     iris::pbrt_frontend::Parameter::REFLECTOR_TEXTURE,
                     spectrum_manager, texture_manager);

  EXPECT_EQ(1u, parameter.GetReflectorTextures(1u, 1u).size());
  EXPECT_NE(nullptr, parameter.GetReflectorTextures(1u, 1u).at(0).Get());
}

TEST(Parameter, SpectrumWrongType) {
  std::stringstream input("\"integer name\" [1]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  EXPECT_EXIT(
      parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                         iris::pbrt_frontend::Parameter::SPECTRUM,
                         spectrum_manager, texture_manager),
      testing::ExitedWithCode(EXIT_FAILURE),
      "ERROR: Wrong type for parameter list: name");
}

TEST(Parameter, SpectrumTooFew) {
  std::stringstream input("\"rgb name\" [1 1 1 1 1 1]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                     iris::pbrt_frontend::Parameter::SPECTRUM, spectrum_manager,
                     texture_manager);

  EXPECT_EXIT(parameter.GetSpectra(3u, 3u),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Too few values in parameter list: name");
}

TEST(Parameter, SpectrumTooMany) {
  std::stringstream input("\"rgb name\" [1 1 1 1 1 1]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                     iris::pbrt_frontend::Parameter::SPECTRUM, spectrum_manager,
                     texture_manager);

  EXPECT_EXIT(parameter.GetSpectra(1u, 1u),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Too many values in parameter list: name");
}

TEST(Parameter, SpectrumTooManyColor) {
  std::stringstream input("\"rgb name\" [1 1 1]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                     iris::pbrt_frontend::Parameter::SPECTRUM, spectrum_manager,
                     texture_manager);

  ASSERT_EQ(1u, parameter.GetSpectra(1u, 1u).size());
  EXPECT_NE(nullptr, parameter.GetSpectra(1u, 1u).at(0).Get());
}

TEST(Parameter, SpectrumTooManySampled) {
  std::stringstream input("\"spectrum name\" [2 2]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                     iris::pbrt_frontend::Parameter::SPECTRUM, spectrum_manager,
                     texture_manager);

  ASSERT_EQ(1u, parameter.GetSpectra(1u, 1u).size());
  EXPECT_NE(nullptr, parameter.GetSpectra(1u, 1u).at(0).Get());
}

TEST(Parameter, SpectrumFileNotFound) {
  std::stringstream input("\"spectrum name\" [\"notafile\"]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  EXPECT_EXIT(
      parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                         iris::pbrt_frontend::Parameter::SPECTRUM,
                         spectrum_manager, texture_manager),
      testing::ExitedWithCode(EXIT_FAILURE),
      "ERROR: Could not find file specified in parameter list: name");
}

TEST(Parameter, SpectrumEmissiveSpd) {
  std::string path = RunfilePath("emissive.spd");
  std::stringstream input("\"spectrum name\" [" + path + " " + path + "]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                     iris::pbrt_frontend::Parameter::SPECTRUM, spectrum_manager,
                     texture_manager);
  EXPECT_EQ(2u, parameter.GetSpectra(2u, 2u).size());
  EXPECT_NE(nullptr, parameter.GetSpectra(2u, 2u).at(0).Get());
  EXPECT_NE(nullptr, parameter.GetSpectra(2u, 2u).at(1).Get());
}

TEST(Parameter, StringWrongType) {
  std::stringstream input("\"float name\" [1]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  EXPECT_EXIT(
      parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                         iris::pbrt_frontend::Parameter::STRING,
                         spectrum_manager, texture_manager),
      testing::ExitedWithCode(EXIT_FAILURE),
      "ERROR: Wrong type for parameter list: name");
}

TEST(Parameter, StringTooFew) {
  std::stringstream input("\"string name\" [\"str\"]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                     iris::pbrt_frontend::Parameter::STRING, spectrum_manager,
                     texture_manager);

  EXPECT_EXIT(parameter.GetStringValues(0u, 3u),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Too few values in parameter list: name");
}

TEST(Parameter, StringTooMany) {
  std::stringstream input("\"string name\" [\"str\" \"str\" \"str\"]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                     iris::pbrt_frontend::Parameter::STRING, spectrum_manager,
                     texture_manager);

  EXPECT_EXIT(parameter.GetStringValues(1u, 1u),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Too many values in parameter list: name");
}

TEST(Parameter, String) {
  std::stringstream input("\"string name\" [\"str\"]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                     iris::pbrt_frontend::Parameter::STRING, spectrum_manager,
                     texture_manager);

  ASSERT_EQ(1u, parameter.GetStringValues(1u, 1u).size());
  EXPECT_EQ("str", parameter.GetStringValues(1u, 1u).at(0));
}

TEST(Parameter, Vector3WrongType) {
  std::stringstream input("\"float name\" [1]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  EXPECT_EXIT(
      parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                         iris::pbrt_frontend::Parameter::VECTOR3,
                         spectrum_manager, texture_manager),
      testing::ExitedWithCode(EXIT_FAILURE),
      "ERROR: Wrong type for parameter list: name");
}

TEST(Parameter, Vector3TooFew) {
  std::stringstream input("\"vector3 name\" [1 1 1]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                     iris::pbrt_frontend::Parameter::VECTOR3, spectrum_manager,
                     texture_manager);

  EXPECT_EXIT(parameter.GetVector3Values(0u, 3u),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Too few values in parameter list: name");
}

TEST(Parameter, Vector3TooMany) {
  std::stringstream input("\"vector3 name\" [1 1 1 1 1 1 1 1 1]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                     iris::pbrt_frontend::Parameter::VECTOR3, spectrum_manager,
                     texture_manager);

  EXPECT_EXIT(parameter.GetVector3Values(1u, 1u),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Too many values in parameter list: name");
}

TEST(Parameter, Vector3) {
  std::stringstream input("\"vector3 name\" [1 1 1]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);
  iris::pbrt_frontend::ParameterList parameter_list;
  ASSERT_TRUE(parameter_list.ParseFrom(tokenizer));

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::Parameter parameter;
  parameter.LoadFrom(parameter_list, std::filesystem::current_path(),
                     iris::pbrt_frontend::Parameter::VECTOR3, spectrum_manager,
                     texture_manager);

  ASSERT_EQ(1u, parameter.GetVector3Values(1u, 1u).size());
  EXPECT_EQ(iris::Vector(1.0, 1.0, 1.0),
            parameter.GetVector3Values(1u, 1u).at(0));
}