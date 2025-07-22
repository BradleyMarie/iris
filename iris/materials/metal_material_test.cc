#include "iris/materials/metal_material.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/point.h"
#include "iris/spectra/mock_spectrum.h"
#include "iris/testing/bxdf_allocator.h"
#include "iris/testing/spectral_allocator.h"
#include "iris/textures/constant_texture.h"
#include "iris/textures/float_texture.h"
#include "iris/textures/reflector_texture.h"

namespace iris {
namespace materials {
namespace {

using ::iris::spectra::MockSpectrum;
using ::iris::testing::GetBxdfAllocator;
using ::iris::testing::GetSpectralAllocator;
using ::iris::textures::FloatTexture;
using ::iris::textures::MakeConstantTexture;
using ::testing::_;
using ::testing::Return;

TEST(MetalMaterialTest, NullMaterial) {
  ReferenceCounted<Spectrum> spectrum = MakeReferenceCounted<MockSpectrum>();
  ReferenceCounted<FloatTexture> float_texture = MakeConstantTexture(1.0);
  EXPECT_TRUE(MakeMetalMaterial(float_texture, spectrum, spectrum,
                                float_texture, float_texture, false));
  EXPECT_FALSE(MakeMetalMaterial(ReferenceCounted<FloatTexture>(), spectrum,
                                 spectrum, float_texture, float_texture,
                                 false));
  EXPECT_FALSE(MakeMetalMaterial(float_texture, ReferenceCounted<Spectrum>(),
                                 spectrum, float_texture, float_texture,
                                 false));
}

TEST(MetalMaterialTest, Evaluate) {
  ReferenceCounted<MockSpectrum> uniform_spectrum =
      MakeReferenceCounted<MockSpectrum>();
  EXPECT_CALL(*uniform_spectrum, Intensity(_)).WillRepeatedly(Return(1.0));
  ReferenceCounted<FloatTexture> float_texture = MakeConstantTexture(0.0);
  ReferenceCounted<Material> material =
      MakeMetalMaterial(MakeConstantTexture(1.0), uniform_spectrum,
                        uniform_spectrum, float_texture, float_texture, false);

  const Bxdf* result = material->Evaluate(
      TextureCoordinates{Point(0.0, 0.0, 0.0), {}, {0.0, 0.0}, std::nullopt},
      GetSpectralAllocator(), GetBxdfAllocator());
  ASSERT_TRUE(result);
}

}  // namespace
}  // namespace materials
}  // namespace iris
