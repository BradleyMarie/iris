#include "frontends/pbrt/shapes/alpha_mask.h"

#include "frontends/pbrt/spectrum_managers/test_spectrum_manager.h"
#include "frontends/pbrt/texture_manager.h"
#include "googletest/include/gtest/gtest.h"
#include "iris/reference_counted.h"
#include "iris/textures/texture2d.h"

namespace iris {
namespace pbrt_frontend {
namespace {

using ::iris::pbrt_frontend::spectrum_managers::TestSpectrumManager;
using ::iris::textures::ValueTexture2D;
using ::pbrt_proto::v3::FloatTextureParameter;

class AlphaHits : public ValueTexture2D<visual> {
 public:
  visual Evaluate(const TextureCoordinates& coords) const override {
    return 0.001;
  }
};

class AlphaMisses : public ValueTexture2D<visual> {
 public:
  visual Evaluate(const TextureCoordinates& coords) const override {
    return 0.0;
  }
};

TEST(AlphaAdapter, NonZeroValued) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);

  FloatTextureParameter parameter;
  parameter.set_float_value(1.0);

  ReferenceCounted<textures::ValueTexture2D<bool>> result;
  EXPECT_TRUE(MakeAlphaMask(texture_manager, parameter, result));
}

TEST(AlphaAdapter, ZeroValued) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);

  FloatTextureParameter parameter;
  parameter.set_float_value(0.0);

  ReferenceCounted<textures::ValueTexture2D<bool>> result;
  EXPECT_FALSE(MakeAlphaMask(texture_manager, parameter, result));
}

TEST(AlphaAdapter, Empty) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  texture_manager.Put("a", ReferenceCounted<ValueTexture2D<visual>>());

  FloatTextureParameter parameter;

  ReferenceCounted<textures::ValueTexture2D<bool>> result;
  EXPECT_FALSE(MakeAlphaMask(texture_manager, parameter, result));
}

TEST(AlphaAdapter, NullTexture) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  texture_manager.Put("a", ReferenceCounted<ValueTexture2D<visual>>());

  FloatTextureParameter parameter;
  parameter.set_float_texture_name("a");

  ReferenceCounted<textures::ValueTexture2D<bool>> result;
  EXPECT_FALSE(MakeAlphaMask(texture_manager, parameter, result));
}

TEST(AlphaAdapter, Hits) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  texture_manager.Put("a", MakeReferenceCounted<AlphaHits>());

  FloatTextureParameter parameter;
  parameter.set_float_texture_name("a");

  ReferenceCounted<textures::ValueTexture2D<bool>> result;
  ASSERT_TRUE(MakeAlphaMask(texture_manager, parameter, result));
  EXPECT_TRUE(result->Evaluate({0.0, 0.0}));
}

TEST(AlphaAdapter, Misses) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  texture_manager.Put("a", MakeReferenceCounted<AlphaMisses>());

  FloatTextureParameter parameter;
  parameter.set_float_texture_name("a");

  ReferenceCounted<textures::ValueTexture2D<bool>> result;
  ASSERT_TRUE(MakeAlphaMask(texture_manager, parameter, result));
  EXPECT_FALSE(result->Evaluate({0.0, 0.0}));
}

}  // namespace
}  // namespace pbrt_frontend
}  // namespace iris
