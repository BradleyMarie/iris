#include "iris/materials/translucent_material.h"

#include "iris/bxdfs/composite_bxdf.h"
#include "iris/bxdfs/lambertian_bxdf.h"
#include "iris/bxdfs/microfacet_bxdf.h"

namespace iris {
namespace materials {

const Bxdf* TranslucentMaterial::Evaluate(
    const TextureCoordinates& texture_coordinates,
    SpectralAllocator& spectral_allocator,
    BxdfAllocator& bxdf_allocator) const {
  auto* reflectance =
      reflectance_->Evaluate(texture_coordinates, spectral_allocator);
  auto* transmittance =
      transmittance_->Evaluate(texture_coordinates, spectral_allocator);

  if (reflectance == nullptr && transmittance == nullptr) {
    return nullptr;
  }

  auto* diffuse = diffuse_->Evaluate(texture_coordinates, spectral_allocator);

  const Bxdf* lambertian_brdf = nullptr;
  const Bxdf* lambertian_btdf = nullptr;
  if (diffuse != nullptr) {
    if (reflectance) {
      lambertian_brdf = &bxdf_allocator.Allocate<bxdfs::LambertianBrdf>(
          *spectral_allocator.Scale(reflectance, diffuse));
    }

    if (transmittance) {
      lambertian_btdf = &bxdf_allocator.Allocate<bxdfs::LambertianBtdf>(
          *spectral_allocator.Scale(transmittance, diffuse));
    }
  }

  auto* specular = specular_->Evaluate(texture_coordinates, spectral_allocator);

  // Consider clamping these values
  visual eta_incident = eta_incident_->Evaluate(texture_coordinates);
  visual eta_transmitted = eta_transmitted_->Evaluate(texture_coordinates);

  const Bxdf* microfacet_brdf = nullptr;
  const Bxdf* microfacet_btdf = nullptr;
  if (specular != nullptr) {
    visual_t roughness = roughness_->Evaluate(texture_coordinates);
    if (remap_roughness_) {
      roughness =
          bxdfs::TrowbridgeReitzDistribution::RoughnessToAlpha(roughness);
    }

    if (reflectance) {
      microfacet_brdf = &bxdf_allocator.Allocate<bxdfs::MicrofacetBrdf<
          bxdfs::TrowbridgeReitzDistribution, bxdfs::FresnelDielectric>>(
          *spectral_allocator.Scale(reflectance, specular),
          bxdfs::TrowbridgeReitzDistribution(roughness, roughness),
          bxdfs::FresnelDielectric(eta_incident, eta_transmitted));
    }

    if (transmittance) {
      microfacet_btdf = &bxdf_allocator.Allocate<bxdfs::MicrofacetBtdf<
          bxdfs::TrowbridgeReitzDistribution, bxdfs::FresnelDielectric>>(
          *spectral_allocator.Scale(transmittance, specular), eta_incident,
          eta_transmitted,
          bxdfs::TrowbridgeReitzDistribution(roughness, roughness),
          bxdfs::FresnelDielectric(eta_incident, eta_transmitted));
    }
  }

  return bxdfs::MakeComposite(bxdf_allocator, lambertian_brdf, lambertian_btdf,
                              microfacet_brdf, microfacet_btdf);
}

}  // namespace materials
}  // namespace iris