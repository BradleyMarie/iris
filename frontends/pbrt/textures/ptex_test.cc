#include "frontends/pbrt/textures/ptex.h"

#include <cstdlib>

#include "frontends/pbrt/image_manager.h"
#include "frontends/pbrt/spectrum_managers/test_spectrum_manager.h"
#include "frontends/pbrt/texture_manager.h"
#include "googletest/include/gtest/gtest.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace textures {
namespace {

using ::iris::pbrt_frontend::spectrum_managers::TestSpectrumManager;
using ::pbrt_proto::v3::FloatTexture;
using ::pbrt_proto::v3::SpectrumTexture;
using ::testing::ExitedWithCode;

TEST(Ptex, FloatTexture) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(std::filesystem::current_path(), spectrum_manager);

  FloatTexture::Ptex ptex;
  EXPECT_EXIT(MakePtex(ptex, image_manager, texture_manager),
              ExitedWithCode(EXIT_FAILURE), "ERROR: Failed to open ptex file");
}

TEST(Ptex, SpectrumTexture) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(std::filesystem::current_path(), spectrum_manager);

  SpectrumTexture::Ptex ptex;
  EXPECT_EXIT(MakePtex(ptex, image_manager, texture_manager),
              ExitedWithCode(EXIT_FAILURE), "ERROR: Failed to open ptex file");
}

}  // namespace
}  // namespace textures
}  // namespace pbrt_frontend
}  // namespace iris
