#include "iris/materials/plastic_material.h"

#include "iris/bxdfs/composite_bxdf.h"
#include "iris/bxdfs/lambertian_bxdf.h"
#include "iris/bxdfs/microfacet_bxdf.h"

namespace iris {
namespace materials {

const Bxdf* PlasticMaterial::Evaluate(
    const TextureCoordinates& texture_coordinates,
    SpectralAllocator& spectral_allocator,
    BxdfAllocator& bxdf_allocator) const {
  auto* diffuse = diffuse_->Evaluate(texture_coordinates, spectral_allocator);

  const Bxdf* lambertian_brdf = nullptr;
  if (diffuse != nullptr) {
    lambertian_brdf = &bxdf_allocator.Allocate<bxdfs::LambertianBrdf>(*diffuse);
  }

  auto* specular = specular_->Evaluate(texture_coordinates, spectral_allocator);

  const Bxdf* microfacet_brdf = nullptr;
  if (specular != nullptr) {
    visual_t roughness = roughness_->Evaluate(texture_coordinates);
    if (remap_roughness_) {
      roughness =
          bxdfs::TrowbridgeReitzDistribution::RoughnessToAlpha(roughness);
    }

    visual_t eta_incident = eta_incident_->Evaluate(texture_coordinates);
    visual_t eta_transmitted = eta_transmitted_->Evaluate(texture_coordinates);

    microfacet_brdf = &bxdf_allocator.Allocate<bxdfs::MicrofacetBrdf<
        bxdfs::TrowbridgeReitzDistribution, bxdfs::FresnelDielectric>>(
        *specular, bxdfs::TrowbridgeReitzDistribution(roughness, roughness),
        bxdfs::FresnelDielectric(eta_incident, eta_transmitted));
  }

  return bxdfs::MakeComposite(bxdf_allocator, lambertian_brdf, microfacet_brdf);
}

}  // namespace materials
}  // namespace iris