#include "iris/materials/metal_material.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/spectra/mock_spectrum.h"
#include "iris/testing/bxdf_allocator.h"
#include "iris/testing/spectral_allocator.h"
#include "iris/textures/constant_texture.h"

namespace iris {
namespace materials {
namespace {

using ::iris::spectra::MockSpectrum;
using ::iris::testing::GetBxdfAllocator;
using ::iris::testing::GetSpectralAllocator;
using ::iris::textures::ConstantValueTexture2D;
using ::iris::textures::ValueTexture2D;
using ::testing::_;
using ::testing::Return;

TEST(MetalMaterialTest, NullMaterial) {
  ReferenceCounted<Spectrum> spectrum = MakeReferenceCounted<MockSpectrum>();
  ReferenceCounted<ValueTexture2D<visual>> float_texture =
      MakeReferenceCounted<ConstantValueTexture2D<visual>>(1.0);
  EXPECT_TRUE(MakeMetalMaterial(float_texture, spectrum, spectrum,
                                float_texture, float_texture, false));
  EXPECT_FALSE(MakeMetalMaterial(ReferenceCounted<ValueTexture2D<visual>>(),
                                 spectrum, spectrum, float_texture,
                                 float_texture, false));
  EXPECT_FALSE(MakeMetalMaterial(float_texture, ReferenceCounted<Spectrum>(),
                                 spectrum, float_texture, float_texture,
                                 false));
}

TEST(MetalMaterialTest, Evaluate) {
  ReferenceCounted<MockSpectrum> uniform_spectrum =
      MakeReferenceCounted<MockSpectrum>();
  EXPECT_CALL(*uniform_spectrum, Intensity(_)).WillRepeatedly(Return(1.0));
  ReferenceCounted<ValueTexture2D<visual>> float_texture =
      MakeReferenceCounted<ConstantValueTexture2D<visual>>(0.0);
  ReferenceCounted<Material> material = MakeMetalMaterial(
      MakeReferenceCounted<ConstantValueTexture2D<visual>>(1.0),
      uniform_spectrum, uniform_spectrum, float_texture, float_texture, false);

  const Bxdf* result =
      material->Evaluate(TextureCoordinates{{0.0, 0.0}, std::nullopt},
                         GetSpectralAllocator(), GetBxdfAllocator());
  ASSERT_TRUE(result);
}

}  // namespace
}  // namespace materials
}  // namespace iris
