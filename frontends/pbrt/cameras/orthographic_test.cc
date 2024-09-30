#include "frontends/pbrt/cameras/orthographic.h"

#include "frontends/pbrt/build_objects.h"
#include "frontends/pbrt/spectrum_managers/test_spectrum_manager.h"
#include "googletest/include/gtest/gtest.h"

namespace iris {
namespace pbrt_frontend {
namespace cameras {
namespace {

using ::iris::pbrt_frontend::spectrum_managers::TestSpectrumManager;
using ::testing::ExitedWithCode;

static const MatrixManager::Transformation g_transformation = {
    Matrix::Identity(), Matrix::Identity()};

TEST(Orthographic, Empty) {
  std::stringstream input("");
  Tokenizer tokenizer(input);

  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager;
  auto result = BuildObject(*g_orthographic_builder, tokenizer,
                            std::filesystem::current_path(), spectrum_manager,
                            texture_manager, g_transformation);
  EXPECT_TRUE(result);
}

TEST(Orthographic, AspectRatioNegative) {
  std::stringstream input("\"float frameaspectratio\" -1.0");
  Tokenizer tokenizer(input);

  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager;
  EXPECT_EXIT(BuildObject(*g_orthographic_builder, tokenizer,
                          std::filesystem::current_path(), spectrum_manager,
                          texture_manager, g_transformation),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: frameaspectratio");
}

TEST(Orthographic, AspectRatioZero) {
  std::stringstream input("\"float frameaspectratio\" 0.0");
  Tokenizer tokenizer(input);

  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager;
  EXPECT_EXIT(BuildObject(*g_orthographic_builder, tokenizer,
                          std::filesystem::current_path(), spectrum_manager,
                          texture_manager, g_transformation),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: frameaspectratio");
}

TEST(Orthographic, WithXAspectRatio) {
  std::stringstream input("\"float frameaspectratio\" 2.0");
  Tokenizer tokenizer(input);

  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager;
  auto result = BuildObject(*g_orthographic_builder, tokenizer,
                            std::filesystem::current_path(), spectrum_manager,
                            texture_manager, g_transformation);
  ASSERT_TRUE(result);

  auto camera = result(std::make_pair(1u, 1u));
  auto top_left = camera->Compute({0.0, 0.0}, {0.0, 0.0}, std::nullopt);
  EXPECT_EQ(Point(-2.0, 1.0, 0.0), top_left.origin);
  auto bottom_right = camera->Compute({1.0, 1.0}, {1.0, 1.0}, std::nullopt);
  EXPECT_EQ(Point(2.0, -1.0, 0.0), bottom_right.origin);
}

TEST(Orthographic, WithYAspectRatio) {
  std::stringstream input("\"float frameaspectratio\" 0.5");
  Tokenizer tokenizer(input);

  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager;
  auto result = BuildObject(*g_orthographic_builder, tokenizer,
                            std::filesystem::current_path(), spectrum_manager,
                            texture_manager, g_transformation);
  ASSERT_TRUE(result);

  auto camera = result(std::make_pair(1u, 1u));
  auto top_left = camera->Compute({0.0, 0.0}, {0.0, 0.0}, std::nullopt);
  EXPECT_EQ(Point(-1.0, 2.0, 0.0), top_left.origin);
  auto bottom_right = camera->Compute({1.0, 1.0}, {1.0, 1.0}, std::nullopt);
  EXPECT_EQ(Point(1.0, -2.0, 0.0), bottom_right.origin);
}

TEST(Orthographic, FromFrameX) {
  std::stringstream input("");
  Tokenizer tokenizer(input);

  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager;
  auto result = BuildObject(*g_orthographic_builder, tokenizer,
                            std::filesystem::current_path(), spectrum_manager,
                            texture_manager, g_transformation);
  ASSERT_TRUE(result);

  auto camera = result(std::make_pair(1u, 2u));
  auto top_left = camera->Compute({0.0, 0.0}, {0.0, 0.0}, std::nullopt);
  EXPECT_EQ(Point(-2.0, 1.0, 0.0), top_left.origin);
  auto bottom_right = camera->Compute({1.0, 1.0}, {1.0, 1.0}, std::nullopt);
  EXPECT_EQ(Point(2.0, -1.0, 0.0), bottom_right.origin);
}

TEST(Orthographic, FromFrameY) {
  std::stringstream input("");
  Tokenizer tokenizer(input);

  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager;
  auto result = BuildObject(*g_orthographic_builder, tokenizer,
                            std::filesystem::current_path(), spectrum_manager,
                            texture_manager, g_transformation);
  ASSERT_TRUE(result);

  auto camera = result(std::make_pair(2u, 1u));
  auto top_left = camera->Compute({0.0, 0.0}, {0.0, 0.0}, std::nullopt);
  EXPECT_EQ(Point(-1.0, 2.0, 0.0), top_left.origin);
  auto bottom_right = camera->Compute({1.0, 1.0}, {1.0, 1.0}, std::nullopt);
  EXPECT_EQ(Point(1.0, -2.0, 0.0), bottom_right.origin);
}

TEST(Perspective, WithShutterOpen) {
  std::stringstream input("\"float shutteropen\" 0.5");
  Tokenizer tokenizer(input);

  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager;
  auto result = BuildObject(*g_orthographic_builder, tokenizer,
                            std::filesystem::current_path(), spectrum_manager,
                            texture_manager, g_transformation);
  ASSERT_TRUE(result);
}

TEST(Perspective, WithShutterClose) {
  std::stringstream input("\"float shutterclose\" 2.0");
  Tokenizer tokenizer(input);

  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager;
  auto result = BuildObject(*g_orthographic_builder, tokenizer,
                            std::filesystem::current_path(), spectrum_manager,
                            texture_manager, g_transformation);
  ASSERT_TRUE(result);
}

}  // namespace
}  // namespace cameras
}  // namespace pbrt_frontend
}  // namespace iris