#include "iris/materials/metal_material.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/bxdfs/microfacet_bxdf.h"
#include "iris/bxdfs/microfacet_distributions/trowbridge_reitz_distribution.h"
#include "iris/testing/bxdf_allocator.h"
#include "iris/testing/spectral_allocator.h"
#include "iris/textures/constant_texture.h"

namespace iris {
namespace materials {
namespace {

using ::iris::bxdfs::FresnelConductor;
using ::iris::bxdfs::MicrofacetBrdf;
using ::iris::bxdfs::microfacet_distributions::TrowbridgeReitzDistribution;
using ::iris::testing::GetBxdfAllocator;
using ::iris::testing::GetSpectralAllocator;
using ::iris::textures::ConstantValueTexture2D;
using ::iris::textures::ValueTexture2D;

TEST(MetalMaterialTest, Evaluate) {
  ReferenceCounted<Spectrum> spectrum;
  ReferenceCounted<ValueTexture2D<visual>> float_texture =
      MakeReferenceCounted<ConstantValueTexture2D<visual>>(0.0);
  MetalMaterial material(spectrum, spectrum, spectrum, float_texture,
                         float_texture, false);

  const Bxdf* result =
      material.Evaluate(TextureCoordinates{{0.0, 0.0}, std::nullopt},
                        GetSpectralAllocator(), GetBxdfAllocator());
  ASSERT_TRUE(result);
  EXPECT_TRUE((dynamic_cast<const MicrofacetBrdf<TrowbridgeReitzDistribution,
                                                 FresnelConductor>*>(result)));
}

}  // namespace
}  // namespace materials
}  // namespace iris