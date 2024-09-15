#include "iris/materials/substrate_material.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/bxdfs/ashikhmin_shirley_brdf.h"
#include "iris/bxdfs/microfacet_distributions/trowbridge_reitz_distribution.h"
#include "iris/reflectors/mock_reflector.h"
#include "iris/testing/bxdf_allocator.h"
#include "iris/testing/spectral_allocator.h"
#include "iris/textures/constant_texture.h"

namespace iris {
namespace materials {
namespace {

using ::iris::bxdfs::AshikhminShirleyBrdf;
using ::iris::bxdfs::microfacet_distributions::TrowbridgeReitzDistribution;
using ::iris::reflectors::MockReflector;
using ::iris::testing::GetBxdfAllocator;
using ::iris::testing::GetSpectralAllocator;
using ::iris::textures::ConstantPointerTexture2D;
using ::iris::textures::ConstantValueTexture2D;
using ::iris::textures::PointerTexture2D;
using ::iris::textures::ValueTexture2D;

TEST(SubstrateMaterialTest, EvaluateEmpty) {
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> reflectance =
      MakeReferenceCounted<
          ConstantPointerTexture2D<Reflector, SpectralAllocator>>(
          ReferenceCounted<Reflector>());
  ReferenceCounted<ValueTexture2D<visual>> roughness =
      MakeReferenceCounted<ConstantValueTexture2D<visual>>(1.0);
  SubstrateMaterial material(reflectance, reflectance, roughness, roughness,
                             true);

  ASSERT_FALSE(material.Evaluate(TextureCoordinates{{0.0, 0.0}, std::nullopt},
                                 GetSpectralAllocator(), GetBxdfAllocator()));
}

TEST(SubstrateMaterialTest, Evaluate) {
  ReferenceCounted<Reflector> reflector = MakeReferenceCounted<MockReflector>();

  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> reflectance =
      MakeReferenceCounted<
          ConstantPointerTexture2D<Reflector, SpectralAllocator>>(reflector);
  ReferenceCounted<ValueTexture2D<visual>> roughness =
      MakeReferenceCounted<ConstantValueTexture2D<visual>>(0.0);
  SubstrateMaterial material(reflectance, reflectance, roughness, roughness,
                             true);

  const Bxdf* result =
      material.Evaluate(TextureCoordinates{{0.0, 0.0}, std::nullopt},
                        GetSpectralAllocator(), GetBxdfAllocator());
  ASSERT_TRUE(result);
  EXPECT_TRUE(
      dynamic_cast<const AshikhminShirleyBrdf<TrowbridgeReitzDistribution>*>(
          result));
}

}  // namespace
}  // namespace materials
}  // namespace iris