#include "iris/materials/plastic_material.h"

#include "iris/bxdfs/composite_bxdf.h"
#include "iris/bxdfs/lambertian_bxdf.h"
#include "iris/bxdfs/microfacet_bxdf.h"
#include "iris/bxdfs/microfacet_distributions/trowbridge_reitz_distribution.h"

namespace iris {
namespace materials {

using ::iris::bxdfs::FresnelDielectric;
using ::iris::bxdfs::LambertianBrdf;
using ::iris::bxdfs::MakeCompositeBxdf;
using ::iris::bxdfs::MicrofacetBrdf;
using ::iris::bxdfs::microfacet_distributions::TrowbridgeReitzDistribution;

const Bxdf* PlasticMaterial::Evaluate(
    const TextureCoordinates& texture_coordinates,
    SpectralAllocator& spectral_allocator,
    BxdfAllocator& bxdf_allocator) const {
  const Bxdf* lambertian_brdf = nullptr;
  if (const Reflector* diffuse =
          diffuse_->Evaluate(texture_coordinates, spectral_allocator);
      diffuse != nullptr) {
    lambertian_brdf = &bxdf_allocator.Allocate<LambertianBrdf>(*diffuse);
  }

  const Bxdf* microfacet_brdf = nullptr;
  if (const Reflector* specular =
          specular_->Evaluate(texture_coordinates, spectral_allocator);
      specular != nullptr) {
    visual roughness = static_cast<visual>(0.0);
    if (roughness_) {
      roughness = roughness_->Evaluate(texture_coordinates);
      if (remap_roughness_) {
        roughness = TrowbridgeReitzDistribution::RoughnessToAlpha(roughness);
      }
    }

    visual eta_incident = eta_incident_->Evaluate(texture_coordinates);
    visual eta_transmitted = eta_transmitted_->Evaluate(texture_coordinates);

    microfacet_brdf = &bxdf_allocator.Allocate<
        MicrofacetBrdf<TrowbridgeReitzDistribution, FresnelDielectric>>(
        *specular, TrowbridgeReitzDistribution(roughness, roughness),
        FresnelDielectric(eta_incident, eta_transmitted));
  }

  return MakeCompositeBxdf(bxdf_allocator, lambertian_brdf, microfacet_brdf);
}

ReferenceCounted<Material> MakePlasticMaterial(
    ReferenceCounted<textures::PointerTexture2D<Reflector, SpectralAllocator>>
        diffuse,
    ReferenceCounted<textures::PointerTexture2D<Reflector, SpectralAllocator>>
        specular,
    ReferenceCounted<textures::ValueTexture2D<visual>> eta_incident,
    ReferenceCounted<textures::ValueTexture2D<visual>> eta_transmitted,
    ReferenceCounted<textures::ValueTexture2D<visual>> roughness,
    bool remap_roughness) {
  if (!diffuse && !specular) {
    return ReferenceCounted<Material>();
  }

  return MakeReferenceCounted<PlasticMaterial>(
      std::move(diffuse), std::move(specular), std::move(eta_incident),
      std::move(eta_transmitted), std::move(roughness), remap_roughness);
}

}  // namespace materials
}  // namespace iris
