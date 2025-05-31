#include "iris/materials/substrate_material.h"

#include <iostream>
#include <utility>

#include "iris/bxdf.h"
#include "iris/bxdf_allocator.h"
#include "iris/bxdfs/ashikhmin_shirley_brdf.h"
#include "iris/bxdfs/microfacet_distributions/trowbridge_reitz_distribution.h"
#include "iris/float.h"
#include "iris/material.h"
#include "iris/reference_counted.h"
#include "iris/spectral_allocator.h"
#include "iris/texture_coordinates.h"
#include "iris/textures/texture2d.h"

namespace iris {
namespace materials {
namespace {

using ::iris::bxdfs::MakeAshikhminShirleyBrdf;
using ::iris::bxdfs::microfacet_distributions::TrowbridgeReitzDistribution;
using ::iris::textures::PointerTexture2D;
using ::iris::textures::ValueTexture2D;

class SubstrateMaterial final : public Material {
 public:
  SubstrateMaterial(
      ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> diffuse,
      ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> specular,
      ReferenceCounted<ValueTexture2D<visual>> roughness_u,
      ReferenceCounted<ValueTexture2D<visual>> roughness_v,
      bool remap_roughness)
      : diffuse_(std::move(diffuse)),
        specular_(std::move(specular)),
        roughness_u_(std::move(roughness_u)),
        roughness_v_(std::move(roughness_v)),
        remap_roughness_(remap_roughness) {}

  const Bxdf* Evaluate(const TextureCoordinates& texture_coordinates,
                       SpectralAllocator& spectral_allocator,
                       BxdfAllocator& bxdf_allocator) const override;

 private:
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> diffuse_;
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> specular_;
  ReferenceCounted<ValueTexture2D<visual>> roughness_u_;
  ReferenceCounted<ValueTexture2D<visual>> roughness_v_;
  bool remap_roughness_;
};

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

  visual roughness_u = static_cast<visual>(0.0);
  if (roughness_u_) {
    roughness_u = roughness_u_->Evaluate(texture_coordinates);
  }

  visual roughness_v = static_cast<visual>(0.0);
  if (roughness_v_) {
    roughness_v = roughness_v_->Evaluate(texture_coordinates);
  }

  if (remap_roughness_) {
    roughness_u = TrowbridgeReitzDistribution::RoughnessToAlpha(roughness_u);
    roughness_v = TrowbridgeReitzDistribution::RoughnessToAlpha(roughness_v);
  }

  return MakeAshikhminShirleyBrdf(
      bxdf_allocator, diffuse, specular,
      TrowbridgeReitzDistribution(roughness_u, roughness_v));
}

}  // namespace

ReferenceCounted<Material> MakeSubstrateMaterial(
    ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> diffuse,
    ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> specular,
    ReferenceCounted<ValueTexture2D<visual>> roughness_u,
    ReferenceCounted<ValueTexture2D<visual>> roughness_v,
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
