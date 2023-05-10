#include "frontends/pbrt/cameras/perspective.h"

#include "frontends/pbrt/build_objects.h"
#include "frontends/pbrt/spectrum_managers/test_spectrum_manager.h"
#include "googletest/include/gtest/gtest.h"

static const iris::pbrt_frontend::MatrixManager::Transformation
    g_transformation = {iris::Matrix::Identity(), iris::Matrix::Identity()};

TEST(Perspective, Empty) {
  std::stringstream input("");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  auto result = iris::pbrt_frontend::BuildObject(
      *iris::pbrt_frontend::cameras::g_perspective_builder, tokenizer,
      spectrum_manager, texture_manager, g_transformation);
  EXPECT_TRUE(result);
}

TEST(Perspective, FovTooLow) {
  std::stringstream input("\"float fov\" 0.0");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  EXPECT_EXIT(
      iris::pbrt_frontend::BuildObject(
          *iris::pbrt_frontend::cameras::g_perspective_builder, tokenizer,
          spectrum_manager, texture_manager, g_transformation),
      testing::ExitedWithCode(EXIT_FAILURE),
      "ERROR: Out of range value for parameter: fov");
}

TEST(Perspective, FovTooHigh) {
  std::stringstream input("\"float fov\" 180.0");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  EXPECT_EXIT(
      iris::pbrt_frontend::BuildObject(
          *iris::pbrt_frontend::cameras::g_perspective_builder, tokenizer,
          spectrum_manager, texture_manager, g_transformation),
      testing::ExitedWithCode(EXIT_FAILURE),
      "ERROR: Out of range value for parameter: fov");
}

TEST(Perspective, HalfTooLow) {
  std::stringstream input("\"float halffov\" 0.0");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  EXPECT_EXIT(
      iris::pbrt_frontend::BuildObject(
          *iris::pbrt_frontend::cameras::g_perspective_builder, tokenizer,
          spectrum_manager, texture_manager, g_transformation),
      testing::ExitedWithCode(EXIT_FAILURE),
      "ERROR: Out of range value for parameter: halffov");
}

TEST(Perspective, HalfTooHigh) {
  std::stringstream input("\"float halffov\" 90.0");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  EXPECT_EXIT(
      iris::pbrt_frontend::BuildObject(
          *iris::pbrt_frontend::cameras::g_perspective_builder, tokenizer,
          spectrum_manager, texture_manager, g_transformation),
      testing::ExitedWithCode(EXIT_FAILURE),
      "ERROR: Out of range value for parameter: halffov");
}

TEST(Perspective, FovAndHalfFov) {
  std::stringstream input("\"float fov\" 45.0 \"float halffov\" 45.0");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  EXPECT_EXIT(
      iris::pbrt_frontend::BuildObject(
          *iris::pbrt_frontend::cameras::g_perspective_builder, tokenizer,
          spectrum_manager, texture_manager, g_transformation),
      testing::ExitedWithCode(EXIT_FAILURE),
      "ERROR: Cannot specify parameters together: fov, halffov");
}

TEST(Perspective, AspectRatioNegative) {
  std::stringstream input("\"float frameaspectratio\" -1.0");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  EXPECT_EXIT(
      iris::pbrt_frontend::BuildObject(
          *iris::pbrt_frontend::cameras::g_perspective_builder, tokenizer,
          spectrum_manager, texture_manager, g_transformation),
      testing::ExitedWithCode(EXIT_FAILURE),
      "ERROR: Out of range value for parameter: frameaspectratio");
}

TEST(Perspective, AspectRatioZero) {
  std::stringstream input("\"float frameaspectratio\" 0.0");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  EXPECT_EXIT(
      iris::pbrt_frontend::BuildObject(
          *iris::pbrt_frontend::cameras::g_perspective_builder, tokenizer,
          spectrum_manager, texture_manager, g_transformation),
      testing::ExitedWithCode(EXIT_FAILURE),
      "ERROR: Out of range value for parameter: frameaspectratio");
}

TEST(Perspective, WithXAspectRatio) {
  std::stringstream input("\"float frameaspectratio\" 2.0");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  auto result = iris::pbrt_frontend::BuildObject(
      *iris::pbrt_frontend::cameras::g_perspective_builder, tokenizer,
      spectrum_manager, texture_manager, g_transformation);
  ASSERT_TRUE(result);

  auto camera = result(std::make_pair(1u, 1u));
  auto top_right = camera->Compute({1.0, 0.0}, nullptr);
  EXPECT_EQ(iris::Point(0.0, 0.0, 0.0), top_right.origin);
  EXPECT_NEAR(top_right.direction.x, top_right.direction.y * 2.0, 0.001);
}

TEST(Perspective, WithYAspectRatio) {
  std::stringstream input("\"float frameaspectratio\" 0.5");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  auto result = iris::pbrt_frontend::BuildObject(
      *iris::pbrt_frontend::cameras::g_perspective_builder, tokenizer,
      spectrum_manager, texture_manager, g_transformation);
  ASSERT_TRUE(result);

  auto camera = result(std::make_pair(1u, 1u));
  auto top_right = camera->Compute({1.0, 0.0}, nullptr);
  EXPECT_EQ(iris::Point(0.0, 0.0, 0.0), top_right.origin);
  EXPECT_NEAR(top_right.direction.x * 2.0, top_right.direction.y, 0.001);
}

TEST(Perspective, FromFrameX) {
  std::stringstream input("");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  auto result = iris::pbrt_frontend::BuildObject(
      *iris::pbrt_frontend::cameras::g_perspective_builder, tokenizer,
      spectrum_manager, texture_manager, g_transformation);
  ASSERT_TRUE(result);

  auto camera = result(std::make_pair(1u, 2u));
  auto top_right = camera->Compute({1.0, 0.0}, nullptr);
  EXPECT_EQ(iris::Point(0.0, 0.0, 0.0), top_right.origin);
  EXPECT_NEAR(top_right.direction.x, top_right.direction.y * 2.0, 0.001);
}

TEST(Perspective, FromFrameY) {
  std::stringstream input("");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  auto result = iris::pbrt_frontend::BuildObject(
      *iris::pbrt_frontend::cameras::g_perspective_builder, tokenizer,
      spectrum_manager, texture_manager, g_transformation);
  ASSERT_TRUE(result);

  auto camera = result(std::make_pair(2u, 1u));
  auto top_right = camera->Compute({1.0, 0.0}, nullptr);
  EXPECT_EQ(iris::Point(0.0, 0.0, 0.0), top_right.origin);
  EXPECT_NEAR(top_right.direction.x * 2.0, top_right.direction.y, 0.001);
}

TEST(Perspective, WithHalfFov) {
  std::stringstream input("\"float halffov\" 30.0");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  auto result = iris::pbrt_frontend::BuildObject(
      *iris::pbrt_frontend::cameras::g_perspective_builder, tokenizer,
      spectrum_manager, texture_manager, g_transformation);
  ASSERT_TRUE(result);
}

TEST(Perspective, WithFov) {
  std::stringstream input("\"float fov\" 70.0");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  auto result = iris::pbrt_frontend::BuildObject(
      *iris::pbrt_frontend::cameras::g_perspective_builder, tokenizer,
      spectrum_manager, texture_manager, g_transformation);
  ASSERT_TRUE(result);
}