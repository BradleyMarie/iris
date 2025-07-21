#include "iris/textures/constant_texture.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/reference_counted.h"
#include "iris/reflectors/mock_reflector.h"
#include "iris/spectra/mock_spectrum.h"
#include "iris/testing/spectral_allocator.h"
#include "iris/texture_coordinates.h"

namespace iris {
namespace textures {
namespace {

using ::iris::reflectors::MockReflector;
using ::iris::spectra::MockSpectrum;
using ::iris::testing::GetSpectralAllocator;

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

TEST(ConstantFloatTexture, Test) {
  EXPECT_FALSE(MakeConstantTexture(0.0));
  EXPECT_EQ(1.0, MakeConstantTexture(1.0)->Evaluate(
                     TextureCoordinates{{0.0, 0.0}, std::nullopt}));
}

TEST(ConstantReflectorTexture, Test) {
  EXPECT_FALSE(MakeConstantTexture(ReferenceCounted<Reflector>()));
  ReferenceCounted<Reflector> value = MakeReferenceCounted<MockReflector>();
  EXPECT_EQ(value.Get(), MakeConstantTexture(value)->Evaluate(
                             TextureCoordinates{{0.0, 0.0}, std::nullopt},
                             GetSpectralAllocator()));
}

}  // namespace
}  // namespace textures
}  // namespace iris
