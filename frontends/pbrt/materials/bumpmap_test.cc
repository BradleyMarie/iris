#include "frontends/pbrt/materials/bumpmap.h"

#include "frontends/pbrt/spectrum_managers/test_spectrum_manager.h"
#include "frontends/pbrt/texture_manager.h"
#include "googletest/include/gtest/gtest.h"
#include "iris/normal_map.h"
#include "iris/reference_counted.h"
#include "iris/texture_coordinates.h"
#include "iris/textures/float_texture.h"
#include "pbrt_proto/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace materials {
namespace {

using ::iris::pbrt_frontend::spectrum_managers::TestSpectrumManager;
using ::iris::textures::FloatTexture;
using ::pbrt_proto::FloatTextureParameter;

class TestTexture : public FloatTexture {
 public:
  visual_t Evaluate(const TextureCoordinates& coords) const override {
    return 0.0;
  }
};

TEST(MakeBumpMap, Empty) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);

  FloatTextureParameter parameter;

  std::array<ReferenceCounted<NormalMap>, 2> bump_maps =
      MakeBumpMap(parameter, texture_manager);
  EXPECT_FALSE(bump_maps[0]);
  EXPECT_FALSE(bump_maps[1]);
}

TEST(MakeBumpMap, FloatValue) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);

  FloatTextureParameter parameter;
  parameter.set_float_value(1.0);

  std::array<ReferenceCounted<NormalMap>, 2> bump_maps =
      MakeBumpMap(parameter, texture_manager);
  EXPECT_FALSE(bump_maps[0]);
  EXPECT_FALSE(bump_maps[1]);
}

TEST(AlphaAdapter, NamedTexture) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  texture_manager.Put("a", MakeReferenceCounted<TestTexture>());

  FloatTextureParameter parameter;
  parameter.set_float_texture_name("a");

  std::array<ReferenceCounted<NormalMap>, 2> bump_maps =
      MakeBumpMap(parameter, texture_manager);
  EXPECT_TRUE(bump_maps[0]);
  EXPECT_TRUE(bump_maps[1]);
}

}  // namespace
}  // namespace materials
}  // namespace pbrt_frontend
}  // namespace iris
