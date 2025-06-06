#include "iris/textures/constant_texture.h"

#include <optional>

#include "googletest/include/gtest/gtest.h"
#include "iris/reference_counted.h"
#include "iris/spectra/mock_spectrum.h"
#include "iris/texture_coordinates.h"

namespace iris {
namespace textures {
namespace {

using ::iris::spectra::MockSpectrum;

TEST(ConstantValueTexture2DTest, Test) {
  ConstantValueTexture2D<float> texture(1.0);
  EXPECT_EQ(1.0,
            texture.Evaluate(TextureCoordinates{{0.0, 0.0}, std::nullopt}));
}

TEST(PointerValueTexture2DTest, Test) {
  ReferenceCounted<Spectrum> value = MakeReferenceCounted<MockSpectrum>();
  ConstantPointerTexture2D<Spectrum> texture(value);
  EXPECT_EQ(value.Get(),
            texture.Evaluate(TextureCoordinates{{0.0, 0.0}, std::nullopt}));
}

}  // namespace
}  // namespace textures
}  // namespace iris
