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
    visual roughness = roughness_->Evaluate(texture_coordinates);
    if (remap_roughness_) {
      roughness = TrowbridgeReitzDistribution::RoughnessToAlpha(roughness);
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

}  // namespace materials
}  // namespace iris