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
  const Reflector* diffuse = nullptr;
  if (diffuse_) {
    diffuse = diffuse_->Evaluate(texture_coordinates, spectral_allocator);
  }

  const Reflector* specular = nullptr;
  if (specular_) {
    specular = specular_->Evaluate(texture_coordinates, spectral_allocator);
  }

  if (diffuse == nullptr && specular == nullptr) {
    return nullptr;
  }

  visual roughness_u = static_cast<visual>(0.0);
  if (roughness_u_) {
    roughness_u_->Evaluate(texture_coordinates);

    if (remap_roughness_) {
      roughness_u = TrowbridgeReitzDistribution::RoughnessToAlpha(roughness_u);
    }
  }

  visual roughness_v = static_cast<visual>(0.0);
  if (roughness_v_) {
    roughness_v_->Evaluate(texture_coordinates);

    if (remap_roughness_) {
      roughness_v = TrowbridgeReitzDistribution::RoughnessToAlpha(roughness_v);
    }
  }

  return &bxdf_allocator
              .Allocate<AshikhminShirleyBrdf<TrowbridgeReitzDistribution>>(
                  *diffuse, *specular,
                  TrowbridgeReitzDistribution(roughness_u, roughness_v));
}

ReferenceCounted<Material> MakeSubstrateMaterial(
    ReferenceCounted<textures::PointerTexture2D<Reflector, SpectralAllocator>>
        diffuse,
    ReferenceCounted<textures::PointerTexture2D<Reflector, SpectralAllocator>>
        specular,
    ReferenceCounted<textures::ValueTexture2D<visual>> roughness_u,
    ReferenceCounted<textures::ValueTexture2D<visual>> roughness_v,
    bool remap_roughness) {
  if (!diffuse && !specular) {
    return ReferenceCounted<Material>();
  }

  return MakeReferenceCounted<SubstrateMaterial>(
      std::move(diffuse), std::move(specular), std::move(roughness_u),
      std::move(roughness_v), remap_roughness);
}

}  // namespace materials
}  // namespace iris
