#include "iris/materials/substrate_material.h"

#include "iris/bxdfs/ashikhmin_shirley_brdf.h"
#include "iris/bxdfs/microfacet_distributions/trowbridge_reitz_distribution.h"

namespace iris {
namespace materials {

using ::iris::bxdfs::AshikhminShirleyBrdf;
using ::iris::bxdfs::microfacet_distributions::TrowbridgeReitzDistribution;

const Bxdf* SubstrateMaterial::Evaluate(
    const TextureCoordinates& texture_coordinates,
    SpectralAllocator& spectral_allocator,
    BxdfAllocator& bxdf_allocator) const {
  const Reflector* diffuse =
      diffuse_->Evaluate(texture_coordinates, spectral_allocator);
  const Reflector* specular =
      specular_->Evaluate(texture_coordinates, spectral_allocator);
  if (diffuse == nullptr && specular == nullptr) {
    return nullptr;
  }

  visual roughness_u = roughness_u_->Evaluate(texture_coordinates);
  visual roughness_v = roughness_v_->Evaluate(texture_coordinates);
  if (remap_roughness_) {
    roughness_u = TrowbridgeReitzDistribution::RoughnessToAlpha(roughness_u);
    roughness_v = TrowbridgeReitzDistribution::RoughnessToAlpha(roughness_v);
  }

  return &bxdf_allocator
              .Allocate<AshikhminShirleyBrdf<TrowbridgeReitzDistribution>>(
                  *diffuse, *specular,
                  TrowbridgeReitzDistribution(roughness_u, roughness_v));
}

}  // namespace materials
}  // namespace iris