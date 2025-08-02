#include "frontends/pbrt/lights/infinite.h"

#include <string>

#include "frontends/pbrt/spectrum_managers/test_spectrum_manager.h"
#include "googletest/include/gtest/gtest.h"
#include "iris/matrix.h"

namespace iris {
namespace pbrt_frontend {
namespace lights {
namespace {

using ::iris::pbrt_frontend::spectrum_managers::TestSpectrumManager;
using ::pbrt_proto::v3::LightSource;
using ::testing::ExitedWithCode;

TEST(Infinite, Empty) {
  Matrix model_to_world = Matrix::Identity();
  TestSpectrumManager spectrum_manager;

  LightSource::Infinite infinite;

  EXPECT_TRUE(MakeInfinite(infinite, std::filesystem::current_path(),
                           model_to_world, spectrum_manager));
}

TEST(Infinite, BadFiletype) {
  Matrix model_to_world = Matrix::Identity();
  TestSpectrumManager spectrum_manager;

  LightSource::Infinite infinite;
  infinite.set_mapname("frontends/pbrt/lights/test_data/image.txt");

  EXPECT_EXIT(MakeInfinite(infinite, std::filesystem::current_path(),
                           model_to_world, spectrum_manager),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Unsupported image file type: .txt");
}

TEST(Infinite, NoExtension) {
  Matrix model_to_world = Matrix::Identity();
  TestSpectrumManager spectrum_manager;

  LightSource::Infinite infinite;
  infinite.set_mapname("frontends/pbrt/lights/test_data/image");

  EXPECT_EXIT(MakeInfinite(infinite, std::filesystem::current_path(),
                           model_to_world, spectrum_manager),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Unsupported image file \\(no extension\\): image");
}

TEST(Infinite, AllSpecifiedEXR) {
  Matrix model_to_world = Matrix::Identity();
  TestSpectrumManager spectrum_manager;

  LightSource::Infinite infinite;
  infinite.set_mapname("frontends/pbrt/lights/test_data/image.exr");

  EXPECT_TRUE(MakeInfinite(infinite, std::filesystem::current_path(),
                           model_to_world, spectrum_manager));
}

TEST(Infinite, AllSpecifiedPNG) {
  Matrix model_to_world = Matrix::Identity();
  TestSpectrumManager spectrum_manager;

  LightSource::Infinite infinite;
  infinite.set_mapname("frontends/pbrt/lights/test_data/image.png");

  EXPECT_TRUE(MakeInfinite(infinite, std::filesystem::current_path(),
                           model_to_world, spectrum_manager));
}

}  // namespace
}  // namespace lights
}  // namespace pbrt_frontend
}  // namespace iris
