#include "frontends/pbrt/textures/ptex.h"

#include <cstdlib>

#include "frontends/pbrt/image_manager.h"
#include "frontends/pbrt/spectrum_managers/test_spectrum_manager.h"
#include "frontends/pbrt/texture_manager.h"
#include "googletest/include/gtest/gtest.h"
#include "pbrt_proto/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace textures {
namespace {

using ::iris::pbrt_frontend::spectrum_managers::TestSpectrumManager;
using ::pbrt_proto::PtexFloatTexture;
using ::pbrt_proto::PtexSpectrumTexture;
using ::testing::ExitedWithCode;

TEST(Ptex, FloatTexture) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(std::filesystem::current_path(), spectrum_manager);

  PtexFloatTexture ptex;
  EXPECT_EXIT(MakePtex(ptex, image_manager, texture_manager),
              ExitedWithCode(EXIT_FAILURE), "ERROR: Failed to open ptex file");
}

TEST(Ptex, SpectrumTexture) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  ImageManager image_manager(std::filesystem::current_path(), spectrum_manager);

  PtexSpectrumTexture ptex;
  EXPECT_EXIT(MakePtex(ptex, image_manager, texture_manager),
              ExitedWithCode(EXIT_FAILURE), "ERROR: Failed to open ptex file");
}

}  // namespace
}  // namespace textures
}  // namespace pbrt_frontend
}  // namespace iris
