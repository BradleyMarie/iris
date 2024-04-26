#include "frontends/pbrt/parser.h"

#include <cstdlib>

#include "frontends/pbrt/spectrum_managers/test_spectrum_manager.h"
#include "googletest/include/gtest/gtest.h"
#include "iris/color_matchers/mock_color_matcher.h"
#include "iris/random/mersenne_twister_random.h"
#include "tools/cpp/runfiles/runfiles.h"

using bazel::tools::cpp::runfiles::Runfiles;

std::string RawRunfilePath(const std::string& path) {
  std::unique_ptr<Runfiles> runfiles(Runfiles::CreateForTest());
  const char* base_path = "__main__/frontends/pbrt/test_data/";
  return runfiles->Rlocation(base_path + path);
}

std::string RunfilePath(const std::string& path) {
  return std::string("\"") + RawRunfilePath(path) + std::string("\"");
}

TEST(Parser, Empty) {
  std::stringstream input("");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::Parser parser(
      std::make_unique<
          iris::pbrt_frontend::spectrum_managers::TestSpectrumManager>());
  EXPECT_FALSE(parser.ParseFrom(tokenizer, std::filesystem::current_path()));
}

TEST(Parser, NoEnd) {
  std::stringstream input("WorldBegin");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::Parser parser(
      std::make_unique<
          iris::pbrt_frontend::spectrum_managers::TestSpectrumManager>());
  EXPECT_EXIT(parser.ParseFrom(tokenizer, std::filesystem::current_path()),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Final directive should be WorldEnd");
}

TEST(Parser, InvalidDirective) {
  std::stringstream input("NotADirective");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::Parser parser(
      std::make_unique<
          iris::pbrt_frontend::spectrum_managers::TestSpectrumManager>());
  EXPECT_EXIT(parser.ParseFrom(tokenizer, std::filesystem::current_path()),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Invalid directive: NotADirective");
}

TEST(AreaLightSource, BeforeWorldEnd) {
  std::stringstream input("AreaLightSource \"diffuse\"");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::Parser parser(
      std::make_unique<
          iris::pbrt_frontend::spectrum_managers::TestSpectrumManager>());
  EXPECT_EXIT(parser.ParseFrom(tokenizer, std::filesystem::current_path()),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Directive cannot be specified before WorldBegin: "
              "AreaLightSource");
}

TEST(AreaLightSource, TooFewArguments) {
  std::stringstream input("WorldBegin AreaLightSource");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::Parser parser(
      std::make_unique<
          iris::pbrt_frontend::spectrum_managers::TestSpectrumManager>());
  EXPECT_EXIT(parser.ParseFrom(tokenizer, std::filesystem::current_path()),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Too few parameters to directive: AreaLightSource");
}

TEST(AttributeBegin, BeforeWorldBegin) {
  std::stringstream input("AttributeBegin");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::Parser parser(
      std::make_unique<
          iris::pbrt_frontend::spectrum_managers::TestSpectrumManager>());
  EXPECT_EXIT(
      parser.ParseFrom(tokenizer, std::filesystem::current_path()),
      testing::ExitedWithCode(EXIT_FAILURE),
      "ERROR: Directive cannot be specified before WorldBegin: AttributeBegin");
}

TEST(AttributeBegin, Mismatched) {
  std::stringstream input("WorldBegin AttributeBegin TransformEnd");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::Parser parser(
      std::make_unique<
          iris::pbrt_frontend::spectrum_managers::TestSpectrumManager>());
  EXPECT_EXIT(parser.ParseFrom(tokenizer, std::filesystem::current_path()),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Mismatched AttributeBegin and AttributeEnd directives");
}

TEST(AttributeEnd, BeforeWorldBegin) {
  std::stringstream input("AttributeEnd");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::Parser parser(
      std::make_unique<
          iris::pbrt_frontend::spectrum_managers::TestSpectrumManager>());
  EXPECT_EXIT(
      parser.ParseFrom(tokenizer, std::filesystem::current_path()),
      testing::ExitedWithCode(EXIT_FAILURE),
      "ERROR: Directive cannot be specified before WorldBegin: AttributeEnd");
}

TEST(AttributeEnd, Mismatched) {
  std::stringstream input("WorldBegin AttributeEnd");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::Parser parser(
      std::make_unique<
          iris::pbrt_frontend::spectrum_managers::TestSpectrumManager>());
  EXPECT_EXIT(parser.ParseFrom(tokenizer, std::filesystem::current_path()),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Missing AttributeBegin directive");
}

TEST(Camera, AfterWorldBegin) {
  std::stringstream input("WorldBegin Camera");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::Parser parser(
      std::make_unique<
          iris::pbrt_frontend::spectrum_managers::TestSpectrumManager>());
  EXPECT_EXIT(parser.ParseFrom(tokenizer, std::filesystem::current_path()),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Directive cannot be specified between WorldBegin and "
              "WorldEnd: Camera");
}

TEST(Camera, Duplicate) {
  std::stringstream input("Camera \"perspective\" Camera \"perspective\"");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::Parser parser(
      std::make_unique<
          iris::pbrt_frontend::spectrum_managers::TestSpectrumManager>());
  EXPECT_EXIT(parser.ParseFrom(tokenizer, std::filesystem::current_path()),
              testing::ExitedWithCode(EXIT_FAILURE),
              "Directive specified twice for a render: Camera");
}

TEST(Camera, TooFewArguments) {
  std::stringstream input("Camera");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::Parser parser(
      std::make_unique<
          iris::pbrt_frontend::spectrum_managers::TestSpectrumManager>());
  EXPECT_EXIT(parser.ParseFrom(tokenizer, std::filesystem::current_path()),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Too few parameters to directive: Camera");
}

TEST(Film, AfterWorldBegin) {
  std::stringstream input("WorldBegin Film");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::Parser parser(
      std::make_unique<
          iris::pbrt_frontend::spectrum_managers::TestSpectrumManager>());
  EXPECT_EXIT(parser.ParseFrom(tokenizer, std::filesystem::current_path()),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Directive cannot be specified between WorldBegin and "
              "WorldEnd: Film");
}

TEST(Film, Duplicate) {
  std::stringstream input("Film \"image\" Film \"image\"");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::Parser parser(
      std::make_unique<
          iris::pbrt_frontend::spectrum_managers::TestSpectrumManager>());
  EXPECT_EXIT(parser.ParseFrom(tokenizer, std::filesystem::current_path()),
              testing::ExitedWithCode(EXIT_FAILURE),
              "Directive specified twice for a render: Film");
}

TEST(Film, TooFewArguments) {
  std::stringstream input("Film");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::Parser parser(
      std::make_unique<
          iris::pbrt_frontend::spectrum_managers::TestSpectrumManager>());
  EXPECT_EXIT(parser.ParseFrom(tokenizer, std::filesystem::current_path()),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Too few parameters to directive: Film");
}

TEST(Include, MissingToken) {
  std::stringstream input("Include");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::Parser parser(
      std::make_unique<
          iris::pbrt_frontend::spectrum_managers::TestSpectrumManager>());
  EXPECT_EXIT(parser.ParseFrom(tokenizer, std::filesystem::current_path()),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Too few parameters to directive: Include");
}

TEST(Include, NotQuoted) {
  std::stringstream input("Include 2.0");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::Parser parser(
      std::make_unique<
          iris::pbrt_frontend::spectrum_managers::TestSpectrumManager>());
  EXPECT_EXIT(parser.ParseFrom(tokenizer, std::filesystem::current_path()),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Parameter to Include must be a string");
}

TEST(Include, Circular) {
  std::filesystem::path file_path(
      RawRunfilePath("include_circular_first.pbrt"));
  std::filesystem::path search_path =
      std::filesystem::weakly_canonical(file_path).parent_path();

  std::stringstream input("Include \"include_circular_first.pbrt\"");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::Parser parser(
      std::make_unique<
          iris::pbrt_frontend::spectrum_managers::TestSpectrumManager>());
  EXPECT_EXIT(
      parser.ParseFrom(tokenizer, search_path),
      testing::ExitedWithCode(EXIT_FAILURE),
      "ERROR: Detected cyclic Include of file: include_circular_first.pbrt");
}

TEST(Include, CircularSelf) {
  std::filesystem::path file_path(
      RawRunfilePath("include_circular_first.pbrt"));
  std::filesystem::path search_path =
      std::filesystem::weakly_canonical(file_path).parent_path();

  std::stringstream input("Include \"include_circular_self.pbrt\"");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::Parser parser(
      std::make_unique<
          iris::pbrt_frontend::spectrum_managers::TestSpectrumManager>());
  EXPECT_EXIT(
      parser.ParseFrom(tokenizer, search_path),
      testing::ExitedWithCode(EXIT_FAILURE),
      "ERROR: Detected cyclic Include of file: include_circular_self.pbrt");
}

TEST(Include, Empty) {
  std::stringstream input("Include " + RunfilePath("include_empty.pbrt"));
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::Parser parser(
      std::make_unique<
          iris::pbrt_frontend::spectrum_managers::TestSpectrumManager>());
  EXPECT_EXIT(parser.ParseFrom(tokenizer, std::filesystem::current_path()),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Final directive should be WorldEnd");
}

TEST(Integrator, AfterWorldBegin) {
  std::stringstream input("WorldBegin Integrator");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::Parser parser(
      std::make_unique<
          iris::pbrt_frontend::spectrum_managers::TestSpectrumManager>());
  EXPECT_EXIT(parser.ParseFrom(tokenizer, std::filesystem::current_path()),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Directive cannot be specified between WorldBegin and "
              "WorldEnd: Integrator");
}

TEST(Integrator, Duplicate) {
  std::stringstream input("Integrator \"path\" Integrator \"path\"");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::Parser parser(
      std::make_unique<
          iris::pbrt_frontend::spectrum_managers::TestSpectrumManager>());
  EXPECT_EXIT(parser.ParseFrom(tokenizer, std::filesystem::current_path()),
              testing::ExitedWithCode(EXIT_FAILURE),
              "Directive specified twice for a render: Integrator");
}

TEST(Integrator, TooFewArguments) {
  std::stringstream input("Integrator");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::Parser parser(
      std::make_unique<
          iris::pbrt_frontend::spectrum_managers::TestSpectrumManager>());
  EXPECT_EXIT(parser.ParseFrom(tokenizer, std::filesystem::current_path()),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Too few parameters to directive: Integrator");
}

TEST(LightSource, BeforeWorldBegin) {
  std::stringstream input("LightSource");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::Parser parser(
      std::make_unique<
          iris::pbrt_frontend::spectrum_managers::TestSpectrumManager>());
  EXPECT_EXIT(
      parser.ParseFrom(tokenizer, std::filesystem::current_path()),
      testing::ExitedWithCode(EXIT_FAILURE),
      "ERROR: Directive cannot be specified before WorldBegin: LightSource");
}

TEST(MakeNamedMaterial, TooFewArguments) {
  std::stringstream input("MakeNamedMaterial");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::Parser parser(
      std::make_unique<
          iris::pbrt_frontend::spectrum_managers::TestSpectrumManager>());
  EXPECT_EXIT(parser.ParseFrom(tokenizer, std::filesystem::current_path()),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Too few parameters to directive: MakeNamedMaterial");
}

TEST(Material, BeforeWorldEnd) {
  std::stringstream input("Material \"matte\"");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::Parser parser(
      std::make_unique<
          iris::pbrt_frontend::spectrum_managers::TestSpectrumManager>());
  EXPECT_EXIT(
      parser.ParseFrom(tokenizer, std::filesystem::current_path()),
      testing::ExitedWithCode(EXIT_FAILURE),
      "ERROR: Directive cannot be specified before WorldBegin: Material");
}

TEST(Material, TooFewArguments) {
  std::stringstream input("WorldBegin Material");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::Parser parser(
      std::make_unique<
          iris::pbrt_frontend::spectrum_managers::TestSpectrumManager>());
  EXPECT_EXIT(parser.ParseFrom(tokenizer, std::filesystem::current_path()),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Too few parameters to directive: Material");
}

TEST(NamedMaterial, BeforeWorldEnd) {
  std::stringstream input("NamedMaterial \"name\"");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::Parser parser(
      std::make_unique<
          iris::pbrt_frontend::spectrum_managers::TestSpectrumManager>());
  EXPECT_EXIT(
      parser.ParseFrom(tokenizer, std::filesystem::current_path()),
      testing::ExitedWithCode(EXIT_FAILURE),
      "ERROR: Directive cannot be specified before WorldBegin: NamedMaterial");
}

TEST(NamedMaterial, TooFewArguments) {
  std::stringstream input("WorldBegin NamedMaterial");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::Parser parser(
      std::make_unique<
          iris::pbrt_frontend::spectrum_managers::TestSpectrumManager>());
  EXPECT_EXIT(parser.ParseFrom(tokenizer, std::filesystem::current_path()),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Too few parameters to directive: NamedMaterial");
}

TEST(NamedMaterial, NotAString) {
  std::stringstream input("WorldBegin NamedMaterial 1");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::Parser parser(
      std::make_unique<
          iris::pbrt_frontend::spectrum_managers::TestSpectrumManager>());
  EXPECT_EXIT(parser.ParseFrom(tokenizer, std::filesystem::current_path()),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Parameter to NamedMaterial must be a string");
}

TEST(ObjectBegin, BeforeWorldBegin) {
  std::stringstream input("ObjectBegin");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::Parser parser(
      std::make_unique<
          iris::pbrt_frontend::spectrum_managers::TestSpectrumManager>());
  EXPECT_EXIT(
      parser.ParseFrom(tokenizer, std::filesystem::current_path()),
      testing::ExitedWithCode(EXIT_FAILURE),
      "ERROR: Directive cannot be specified before WorldBegin: ObjectBegin");
}

TEST(ObjectBegin, Mismatched) {
  std::stringstream input("WorldBegin ObjectBegin \"abc\" ObjectBegin \"abc\"");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::Parser parser(
      std::make_unique<
          iris::pbrt_frontend::spectrum_managers::TestSpectrumManager>());
  EXPECT_EXIT(parser.ParseFrom(tokenizer, std::filesystem::current_path()),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Mismatched ObjectBegin and ObjectEnd directives");
}

TEST(ObjectBegin, TooFew) {
  std::stringstream input("WorldBegin ObjectBegin");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::Parser parser(
      std::make_unique<
          iris::pbrt_frontend::spectrum_managers::TestSpectrumManager>());
  EXPECT_EXIT(parser.ParseFrom(tokenizer, std::filesystem::current_path()),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Too few parameters to directive: ObjectBegin");
}

TEST(ObjectBegin, NotAString) {
  std::stringstream input("WorldBegin ObjectBegin 1");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::Parser parser(
      std::make_unique<
          iris::pbrt_frontend::spectrum_managers::TestSpectrumManager>());
  EXPECT_EXIT(parser.ParseFrom(tokenizer, std::filesystem::current_path()),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Parameter to ObjectBegin must be a string");
}

TEST(ObjectEnd, BeforeWorldBegin) {
  std::stringstream input("ObjectEnd");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::Parser parser(
      std::make_unique<
          iris::pbrt_frontend::spectrum_managers::TestSpectrumManager>());
  EXPECT_EXIT(
      parser.ParseFrom(tokenizer, std::filesystem::current_path()),
      testing::ExitedWithCode(EXIT_FAILURE),
      "ERROR: Directive cannot be specified before WorldBegin: ObjectEnd");
}

TEST(ObjectEnd, Mismatched) {
  std::stringstream input("WorldBegin ObjectEnd");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::Parser parser(
      std::make_unique<
          iris::pbrt_frontend::spectrum_managers::TestSpectrumManager>());
  EXPECT_EXIT(parser.ParseFrom(tokenizer, std::filesystem::current_path()),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Mismatched ObjectBegin and ObjectEnd directives");
}

TEST(ObjectInstance, BeforeWorldBegin) {
  std::stringstream input("ObjectInstance");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::Parser parser(
      std::make_unique<
          iris::pbrt_frontend::spectrum_managers::TestSpectrumManager>());
  EXPECT_EXIT(parser.ParseFrom(tokenizer, std::filesystem::current_path()),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Directive cannot be specified before WorldBegin: "
              "ObjectInstance");
}

TEST(ObjectInstance, InsideObject) {
  std::stringstream input("WorldBegin ObjectBegin \"abc\" ObjectInstance");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::Parser parser(
      std::make_unique<
          iris::pbrt_frontend::spectrum_managers::TestSpectrumManager>());
  EXPECT_EXIT(parser.ParseFrom(tokenizer, std::filesystem::current_path()),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: ObjectInstance cannot be specified between ObjectBegin "
              "and ObjectEnd");
}

TEST(ObjectInstance, TooFew) {
  std::stringstream input("WorldBegin ObjectInstance");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::Parser parser(
      std::make_unique<
          iris::pbrt_frontend::spectrum_managers::TestSpectrumManager>());
  EXPECT_EXIT(parser.ParseFrom(tokenizer, std::filesystem::current_path()),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Too few parameters to directive: ObjectInstance");
}

TEST(ObjectInstance, NotAString) {
  std::stringstream input("WorldBegin ObjectInstance 1");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::Parser parser(
      std::make_unique<
          iris::pbrt_frontend::spectrum_managers::TestSpectrumManager>());
  EXPECT_EXIT(parser.ParseFrom(tokenizer, std::filesystem::current_path()),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Parameter to ObjectInstance must be a string");
}

TEST(ObjectInstance, MissingObject) {
  std::stringstream input("WorldBegin ObjectInstance \"1\"");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::Parser parser(
      std::make_unique<
          iris::pbrt_frontend::spectrum_managers::TestSpectrumManager>());
  EXPECT_EXIT(parser.ParseFrom(tokenizer, std::filesystem::current_path()),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: ObjectInstance referred to an unknown object: 1");
}

TEST(Matrix, Parses) {
  std::stringstream input("Identity");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::Parser parser(
      std::make_unique<
          iris::pbrt_frontend::spectrum_managers::TestSpectrumManager>());
  EXPECT_EXIT(parser.ParseFrom(tokenizer, std::filesystem::current_path()),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Final directive should be WorldEnd");
}

TEST(PixelFilter, AfterWorldBegin) {
  std::stringstream input("WorldBegin PixelFilter");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::Parser parser(
      std::make_unique<
          iris::pbrt_frontend::spectrum_managers::TestSpectrumManager>());
  EXPECT_EXIT(parser.ParseFrom(tokenizer, std::filesystem::current_path()),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Directive cannot be specified between WorldBegin and "
              "WorldEnd: PixelFilter");
}

TEST(PixelFilter, MissingToken) {
  std::stringstream input("PixelFilter");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::Parser parser(
      std::make_unique<
          iris::pbrt_frontend::spectrum_managers::TestSpectrumManager>());
  EXPECT_EXIT(parser.ParseFrom(tokenizer, std::filesystem::current_path()),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Too few parameters to directive: PixelFilter");
}

TEST(PixelFilter, NotQuoted) {
  std::stringstream input("PixelFilter 2.0");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::Parser parser(
      std::make_unique<
          iris::pbrt_frontend::spectrum_managers::TestSpectrumManager>());
  EXPECT_EXIT(parser.ParseFrom(tokenizer, std::filesystem::current_path()),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Parameter to PixelFilter must be a string");
}

TEST(PixelFilter, Duplicate) {
  std::stringstream input("PixelFilter \"box\" PixelFilter \"box\"");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::Parser parser(
      std::make_unique<
          iris::pbrt_frontend::spectrum_managers::TestSpectrumManager>());
  EXPECT_EXIT(parser.ParseFrom(tokenizer, std::filesystem::current_path()),
              testing::ExitedWithCode(EXIT_FAILURE),
              "Directive specified twice for a render: PixelFilter");
}

TEST(ReverseOrientation, Succeeds) {
  std::stringstream input("ReverseOrientation Shape");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::Parser parser(
      std::make_unique<
          iris::pbrt_frontend::spectrum_managers::TestSpectrumManager>());
  EXPECT_EXIT(parser.ParseFrom(tokenizer, std::filesystem::current_path()),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Directive cannot be specified before WorldBegin: Shape");
}

TEST(Sampler, AfterWorldBegin) {
  std::stringstream input("WorldBegin Sampler");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::Parser parser(
      std::make_unique<
          iris::pbrt_frontend::spectrum_managers::TestSpectrumManager>());
  EXPECT_EXIT(parser.ParseFrom(tokenizer, std::filesystem::current_path()),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Directive cannot be specified between WorldBegin and "
              "WorldEnd: Sampler");
}

TEST(Sampler, Duplicate) {
  std::stringstream input("Sampler \"random\" Sampler \"random\"");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::Parser parser(
      std::make_unique<
          iris::pbrt_frontend::spectrum_managers::TestSpectrumManager>());
  EXPECT_EXIT(parser.ParseFrom(tokenizer, std::filesystem::current_path()),
              testing::ExitedWithCode(EXIT_FAILURE),
              "Directive specified twice for a render: Sampler");
}

TEST(Sampler, TooFewArguments) {
  std::stringstream input("Sampler");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::Parser parser(
      std::make_unique<
          iris::pbrt_frontend::spectrum_managers::TestSpectrumManager>());
  EXPECT_EXIT(parser.ParseFrom(tokenizer, std::filesystem::current_path()),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Too few parameters to directive: Sampler");
}

TEST(Shape, BeforeWorldBegin) {
  std::stringstream input("Shape");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::Parser parser(
      std::make_unique<
          iris::pbrt_frontend::spectrum_managers::TestSpectrumManager>());
  EXPECT_EXIT(parser.ParseFrom(tokenizer, std::filesystem::current_path()),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Directive cannot be specified before WorldBegin: Shape");
}

TEST(Texture, BeforeWorldBegin) {
  std::stringstream input("Texture WorldBegin");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::Parser parser(
      std::make_unique<
          iris::pbrt_frontend::spectrum_managers::TestSpectrumManager>());
  EXPECT_EXIT(
      parser.ParseFrom(tokenizer, std::filesystem::current_path()),
      testing::ExitedWithCode(EXIT_FAILURE),
      "ERROR: Directive cannot be specified before WorldBegin: Texture");
}

TEST(Texture, TooFewArguments) {
  std::stringstream input("WorldBegin Texture");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::Parser parser(
      std::make_unique<
          iris::pbrt_frontend::spectrum_managers::TestSpectrumManager>());
  EXPECT_EXIT(parser.ParseFrom(tokenizer, std::filesystem::current_path()),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Too few parameters to directive: Texture");
}

TEST(WorldBegin, Duplicate) {
  std::stringstream input("WorldBegin WorldBegin");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::Parser parser(
      std::make_unique<
          iris::pbrt_frontend::spectrum_managers::TestSpectrumManager>());
  EXPECT_EXIT(parser.ParseFrom(tokenizer, std::filesystem::current_path()),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Invalid WorldBegin directive");
}

TEST(WorldEnd, NoWorldBegin) {
  std::stringstream input("WorldEnd");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::Parser parser(
      std::make_unique<
          iris::pbrt_frontend::spectrum_managers::TestSpectrumManager>());
  EXPECT_EXIT(parser.ParseFrom(tokenizer, std::filesystem::current_path()),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Invalid WorldEnd directive");
}

TEST(Render, EmptyScene) {
  std::stringstream input("WorldBegin WorldEnd");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::Parser parser(
      std::make_unique<
          iris::pbrt_frontend::spectrum_managers::TestSpectrumManager>());
  auto result = parser.ParseFrom(tokenizer, std::filesystem::current_path());
  ASSERT_TRUE(result);

  EXPECT_FALSE(result->skip_pixel_callback({0, 0}, {480, 640}));
  EXPECT_FALSE(result->skip_pixel_callback({479, 639}, {480, 640}));
  EXPECT_TRUE(result->skip_pixel_callback({480, 640}, {480, 640}));
  EXPECT_EQ("pbrt.exr", result->output_filename);

  iris::color_matchers::MockColorMatcher color_matcher;
  EXPECT_CALL(color_matcher, ColorSpace())
      .WillRepeatedly(testing::Return(iris::Color::LINEAR_SRGB));

  iris::random::MersenneTwisterRandom rng;

  auto framebuffer = result->renderable.Render(color_matcher, rng);
  auto dimensions = framebuffer.Size();
  EXPECT_EQ(480u, dimensions.first);
  EXPECT_EQ(640u, dimensions.second);

  for (size_t y = 0; y < dimensions.first; y++) {
    for (size_t x = 0; x < dimensions.second; x++) {
      auto color = framebuffer.Get(y, x);
      EXPECT_EQ(0.0, color.r);
      EXPECT_EQ(0.0, color.g);
      EXPECT_EQ(0.0, color.b);
      EXPECT_EQ(iris::Color::LINEAR_SRGB, color.space);
    }
  }
}