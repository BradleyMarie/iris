#include "frontends/pbrt/cameras/perspective.h"

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

TEST(Perspective, Empty) {
  std::stringstream input("");
  Tokenizer tokenizer(input);

  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager;
  auto result = BuildObject(*g_perspective_builder, tokenizer,
                            std::filesystem::current_path(), spectrum_manager,
                            texture_manager, g_transformation);
  EXPECT_TRUE(result);
}

TEST(Perspective, FocalDistanceTooLow) {
  std::stringstream input("\"float focaldistance\" -0.1");
  Tokenizer tokenizer(input);

  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager;
  EXPECT_EXIT(BuildObject(*g_perspective_builder, tokenizer,
                          std::filesystem::current_path(), spectrum_manager,
                          texture_manager, g_transformation),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: focaldistance");
}

TEST(Perspective, FocalDistanceZero) {
  std::stringstream input("\"float focaldistance\" 0.0");
  Tokenizer tokenizer(input);

  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager;
  EXPECT_EXIT(BuildObject(*g_perspective_builder, tokenizer,
                          std::filesystem::current_path(), spectrum_manager,
                          texture_manager, g_transformation),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: focaldistance");
}

TEST(Perspective, FocalDistancePositive) {
  std::stringstream input("\"float focaldistance\" 0.1");
  Tokenizer tokenizer(input);

  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager;
  EXPECT_TRUE(BuildObject(*g_perspective_builder, tokenizer,
                          std::filesystem::current_path(), spectrum_manager,
                          texture_manager, g_transformation));
}

TEST(Perspective, FovTooLow) {
  std::stringstream input("\"float fov\" 0.0");
  Tokenizer tokenizer(input);

  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager;
  EXPECT_EXIT(BuildObject(*g_perspective_builder, tokenizer,
                          std::filesystem::current_path(), spectrum_manager,
                          texture_manager, g_transformation),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: fov");
}

TEST(Perspective, FovTooHigh) {
  std::stringstream input("\"float fov\" 180.0");
  Tokenizer tokenizer(input);

  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager;
  EXPECT_EXIT(BuildObject(*g_perspective_builder, tokenizer,
                          std::filesystem::current_path(), spectrum_manager,
                          texture_manager, g_transformation),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: fov");
}

TEST(Perspective, HalfTooLow) {
  std::stringstream input("\"float halffov\" 0.0");
  Tokenizer tokenizer(input);

  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager;
  EXPECT_EXIT(BuildObject(*g_perspective_builder, tokenizer,
                          std::filesystem::current_path(), spectrum_manager,
                          texture_manager, g_transformation),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: halffov");
}

TEST(Perspective, HalfTooHigh) {
  std::stringstream input("\"float halffov\" 90.0");
  Tokenizer tokenizer(input);

  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager;
  EXPECT_EXIT(BuildObject(*g_perspective_builder, tokenizer,
                          std::filesystem::current_path(), spectrum_manager,
                          texture_manager, g_transformation),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: halffov");
}

TEST(Perspective, LensRadiusTooLow) {
  std::stringstream input("\"float lensradius\" -0.1");
  Tokenizer tokenizer(input);

  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager;
  EXPECT_EXIT(BuildObject(*g_perspective_builder, tokenizer,
                          std::filesystem::current_path(), spectrum_manager,
                          texture_manager, g_transformation),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: lensradius");
}

TEST(Perspective, LensRadiusTooZero) {
  std::stringstream input("\"float lensradius\" 0.0");
  Tokenizer tokenizer(input);

  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager;
  EXPECT_TRUE(BuildObject(*g_perspective_builder, tokenizer,
                          std::filesystem::current_path(), spectrum_manager,
                          texture_manager, g_transformation));
}

TEST(Perspective, LensRadiusTooPositive) {
  std::stringstream input("\"float lensradius\" 0.1");
  Tokenizer tokenizer(input);

  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager;
  EXPECT_TRUE(BuildObject(*g_perspective_builder, tokenizer,
                          std::filesystem::current_path(), spectrum_manager,
                          texture_manager, g_transformation));
}

TEST(Perspective, FovAndHalfFov) {
  std::stringstream input("\"float fov\" 45.0 \"float halffov\" 45.0");
  Tokenizer tokenizer(input);

  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager;
  EXPECT_EXIT(BuildObject(*g_perspective_builder, tokenizer,
                          std::filesystem::current_path(), spectrum_manager,
                          texture_manager, g_transformation),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Cannot specify parameters together: fov, halffov");
}

TEST(Perspective, AspectRatioNegative) {
  std::stringstream input("\"float frameaspectratio\" -1.0");
  Tokenizer tokenizer(input);

  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager;
  EXPECT_EXIT(BuildObject(*g_perspective_builder, tokenizer,
                          std::filesystem::current_path(), spectrum_manager,
                          texture_manager, g_transformation),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: frameaspectratio");
}

TEST(Perspective, AspectRatioZero) {
  std::stringstream input("\"float frameaspectratio\" 0.0");
  Tokenizer tokenizer(input);

  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager;
  EXPECT_EXIT(BuildObject(*g_perspective_builder, tokenizer,
                          std::filesystem::current_path(), spectrum_manager,
                          texture_manager, g_transformation),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: frameaspectratio");
}

TEST(Perspective, WithXAspectRatio) {
  std::stringstream input("\"float frameaspectratio\" 2.0");
  Tokenizer tokenizer(input);

  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager;
  auto result = BuildObject(*g_perspective_builder, tokenizer,
                            std::filesystem::current_path(), spectrum_manager,
                            texture_manager, g_transformation);
  ASSERT_TRUE(result);

  auto camera = result(std::make_pair(1u, 1u));
  auto top_right = camera->Compute({1.0, 0.0}, {1.0, 0.0}, std::nullopt);
  EXPECT_EQ(Point(0.0, 0.0, 0.0), top_right.origin);
  EXPECT_NEAR(top_right.direction.x, top_right.direction.y * 2.0, 0.001);
}

TEST(Perspective, WithYAspectRatio) {
  std::stringstream input("\"float frameaspectratio\" 0.5");
  Tokenizer tokenizer(input);

  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager;
  auto result = BuildObject(*g_perspective_builder, tokenizer,
                            std::filesystem::current_path(), spectrum_manager,
                            texture_manager, g_transformation);
  ASSERT_TRUE(result);

  auto camera = result(std::make_pair(1u, 1u));
  auto top_right = camera->Compute({1.0, 0.0}, {1.0, 0.0}, std::nullopt);
  EXPECT_EQ(Point(0.0, 0.0, 0.0), top_right.origin);
  EXPECT_NEAR(top_right.direction.x * 2.0, top_right.direction.y, 0.001);
}

TEST(Perspective, FromFrameX) {
  std::stringstream input("");
  Tokenizer tokenizer(input);

  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager;
  auto result = BuildObject(*g_perspective_builder, tokenizer,
                            std::filesystem::current_path(), spectrum_manager,
                            texture_manager, g_transformation);
  ASSERT_TRUE(result);

  auto camera = result(std::make_pair(1u, 2u));
  auto top_right = camera->Compute({1.0, 0.0}, {1.0, 0.0}, std::nullopt);
  EXPECT_EQ(Point(0.0, 0.0, 0.0), top_right.origin);
  EXPECT_NEAR(top_right.direction.x, top_right.direction.y * 2.0, 0.001);
}

TEST(Perspective, FromFrameY) {
  std::stringstream input("");
  Tokenizer tokenizer(input);

  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager;
  auto result = BuildObject(*g_perspective_builder, tokenizer,
                            std::filesystem::current_path(), spectrum_manager,
                            texture_manager, g_transformation);
  ASSERT_TRUE(result);

  auto camera = result(std::make_pair(2u, 1u));
  auto top_right = camera->Compute({1.0, 0.0}, {1.0, 0.0}, std::nullopt);
  EXPECT_EQ(Point(0.0, 0.0, 0.0), top_right.origin);
  EXPECT_NEAR(top_right.direction.x * 2.0, top_right.direction.y, 0.001);
}

TEST(Perspective, WithHalfFov) {
  std::stringstream input("\"float halffov\" 30.0");
  Tokenizer tokenizer(input);

  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager;
  auto result = BuildObject(*g_perspective_builder, tokenizer,
                            std::filesystem::current_path(), spectrum_manager,
                            texture_manager, g_transformation);
  ASSERT_TRUE(result);
}

TEST(Perspective, WithFov) {
  std::stringstream input("\"float fov\" 70.0");
  Tokenizer tokenizer(input);

  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager;
  auto result = BuildObject(*g_perspective_builder, tokenizer,
                            std::filesystem::current_path(), spectrum_manager,
                            texture_manager, g_transformation);
  ASSERT_TRUE(result);
}

TEST(Perspective, WithShutterOpen) {
  std::stringstream input("\"float shutteropen\" 0.5");
  Tokenizer tokenizer(input);

  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager;
  auto result = BuildObject(*g_perspective_builder, tokenizer,
                            std::filesystem::current_path(), spectrum_manager,
                            texture_manager, g_transformation);
  ASSERT_TRUE(result);
}

TEST(Perspective, WithShutterClose) {
  std::stringstream input("\"float shutterclose\" 2.0");
  Tokenizer tokenizer(input);

  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager;
  auto result = BuildObject(*g_perspective_builder, tokenizer,
                            std::filesystem::current_path(), spectrum_manager,
                            texture_manager, g_transformation);
  ASSERT_TRUE(result);
}

}  // namespace
}  // namespace cameras
}  // namespace pbrt_frontend
}  // namespace iris