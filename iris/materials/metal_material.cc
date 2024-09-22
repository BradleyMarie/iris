#include "iris/materials/metal_material.h"

#include "iris/bxdfs/microfacet_bxdf.h"
#include "iris/bxdfs/microfacet_distributions/trowbridge_reitz_distribution.h"
#include "iris/reflectors/uniform_reflector.h"

namespace iris {
namespace materials {
namespace {

using ::iris::bxdfs::FresnelConductor;
using ::iris::bxdfs::MicrofacetBrdf;
using ::iris::bxdfs::microfacet_distributions::TrowbridgeReitzDistribution;
using ::iris::reflectors::CreateUniformReflector;

static const ReferenceCounted<Reflector> kWhite =
    CreateUniformReflector(static_cast<visual>(1.0));

}  // namespace

const Bxdf* MetalMaterial::Evaluate(
    const TextureCoordinates& texture_coordinates,
    SpectralAllocator& spectral_allocator,
    BxdfAllocator& bxdf_allocator) const {
  visual roughness_u = roughness_u_->Evaluate(texture_coordinates);
  visual roughness_v = roughness_v_->Evaluate(texture_coordinates);
  if (remap_roughness_) {
    roughness_u = TrowbridgeReitzDistribution::RoughnessToAlpha(roughness_u);
    roughness_v = TrowbridgeReitzDistribution::RoughnessToAlpha(roughness_v);
  }

  return &bxdf_allocator.Allocate<
      MicrofacetBrdf<TrowbridgeReitzDistribution, FresnelConductor>>(
      *kWhite, TrowbridgeReitzDistribution(roughness_u, roughness_v),
      FresnelConductor(eta_incident_.Get(), eta_transmitted_.Get(), k_.Get()));
}

}  // namespace materials
}  // namespace iris