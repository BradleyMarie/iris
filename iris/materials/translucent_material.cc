#include "iris/materials/translucent_material.h"

#include "iris/bxdfs/composite_bxdf.h"
#include "iris/bxdfs/lambertian_bxdf.h"
#include "iris/bxdfs/microfacet_bxdf.h"
#include "iris/bxdfs/microfacet_distributions/trowbridge_reitz_distribution.h"

namespace iris {
namespace materials {

using ::iris::bxdfs::FresnelDielectric;
using ::iris::bxdfs::LambertianBrdf;
using ::iris::bxdfs::LambertianBtdf;
using ::iris::bxdfs::MakeCompositeBxdf;
using ::iris::bxdfs::MicrofacetBrdf;
using ::iris::bxdfs::MicrofacetBtdf;
using ::iris::bxdfs::microfacet_distributions::TrowbridgeReitzDistribution;

const Bxdf* TranslucentMaterial::Evaluate(
    const TextureCoordinates& texture_coordinates,
    SpectralAllocator& spectral_allocator,
    BxdfAllocator& bxdf_allocator) const {
  const Reflector* reflectance =
      reflectance_->Evaluate(texture_coordinates, spectral_allocator);
  const Reflector* transmittance =
      transmittance_->Evaluate(texture_coordinates, spectral_allocator);

  if (reflectance == nullptr && transmittance == nullptr) {
    return nullptr;
  }

  const Bxdf* lambertian_brdf = nullptr;
  const Bxdf* lambertian_btdf = nullptr;
  if (const Reflector* diffuse =
          diffuse_->Evaluate(texture_coordinates, spectral_allocator);
      diffuse != nullptr) {
    if (reflectance != nullptr) {
      lambertian_brdf = &bxdf_allocator.Allocate<LambertianBrdf>(
          *spectral_allocator.Scale(reflectance, diffuse));
    }

    if (transmittance != nullptr) {
      lambertian_btdf = &bxdf_allocator.Allocate<LambertianBtdf>(
          *spectral_allocator.Scale(transmittance, diffuse));
    }
  }

  const Bxdf* microfacet_brdf = nullptr;
  const Bxdf* microfacet_btdf = nullptr;
  if (const Reflector* specular =
          specular_->Evaluate(texture_coordinates, spectral_allocator);
      specular != nullptr) {
    // Consider clamping these values
    visual eta_incident = eta_incident_->Evaluate(texture_coordinates);
    visual eta_transmitted = eta_transmitted_->Evaluate(texture_coordinates);

    visual roughness = roughness_->Evaluate(texture_coordinates);
    if (remap_roughness_) {
      roughness = TrowbridgeReitzDistribution::RoughnessToAlpha(roughness);
    }

    if (reflectance != nullptr) {
      microfacet_brdf = &bxdf_allocator.Allocate<
          MicrofacetBrdf<TrowbridgeReitzDistribution, FresnelDielectric>>(
          *spectral_allocator.Scale(reflectance, specular),
          TrowbridgeReitzDistribution(roughness, roughness),
          FresnelDielectric(eta_incident, eta_transmitted));
    }

    if (transmittance != nullptr) {
      microfacet_btdf = &bxdf_allocator.Allocate<
          MicrofacetBtdf<TrowbridgeReitzDistribution, FresnelDielectric>>(
          *spectral_allocator.Scale(transmittance, specular), eta_incident,
          eta_transmitted, TrowbridgeReitzDistribution(roughness, roughness),
          FresnelDielectric(eta_incident, eta_transmitted));
    }
  }

  return MakeCompositeBxdf(bxdf_allocator, lambertian_brdf, lambertian_btdf,
                           microfacet_brdf, microfacet_btdf);
}

}  // namespace materials
}  // namespace iris