#include "iris/bxdfs/microfacet_bxdf.h"

#include <cmath>

#include "iris/bxdf.h"
#include "iris/bxdf_allocator.h"
#include "iris/bxdfs/internal/fresnel.h"
#include "iris/bxdfs/internal/microfacet_bxdf.h"
#include "iris/bxdfs/internal/microfacet_distribution.h"
#include "iris/float.h"
#include "iris/reflector.h"

namespace iris {
namespace bxdfs {

using ::iris::bxdfs::internal::FresnelConductor;
using ::iris::bxdfs::internal::FresnelDielectric;
using ::iris::bxdfs::internal::MicrofacetBrdf;
using ::iris::bxdfs::internal::MicrofacetBtdf;
using ::iris::bxdfs::internal::MicrofacetDistribution;
using ::iris::bxdfs::internal::TrowbridgeReitzDistribution;

const Bxdf* MakeMicrofacetConductorBrdf(
    BxdfAllocator& bxdf_allocator, const Reflector* reflectance,
    visual_t eta_dielectric, const Spectrum* eta_conductor,
    const Spectrum* k_conductor, geometric_t roughness_x,
    geometric_t roughness_y, bool remap_roughness_to_alpha) {
  if (!reflectance) {
    return nullptr;
  }

  FresnelConductor fresnel(eta_dielectric, eta_conductor, k_conductor);
  if (!fresnel.IsValid()) {
    return nullptr;
  }

  return &bxdf_allocator.Allocate<
      MicrofacetBrdf<TrowbridgeReitzDistribution, FresnelConductor>>(
      *reflectance,
      TrowbridgeReitzDistribution(roughness_x, roughness_y,
                                  remap_roughness_to_alpha),
      fresnel);
}

const Bxdf* MakeMicrofacetDielectricBrdf(BxdfAllocator& bxdf_allocator,
                                         const Reflector* reflectance,
                                         geometric_t eta_incident,
                                         geometric_t eta_transmitted,
                                         geometric_t roughness_x,
                                         geometric_t roughness_y,
                                         bool remap_roughness_to_alpha) {
  if (!reflectance) {
    return nullptr;
  }

  FresnelDielectric fresnel(eta_incident, eta_transmitted);
  if (!fresnel.IsValid()) {
    return nullptr;
  }

  return &bxdf_allocator.Allocate<
      MicrofacetBrdf<TrowbridgeReitzDistribution, FresnelDielectric>>(
      *reflectance,
      TrowbridgeReitzDistribution(roughness_x, roughness_y,
                                  remap_roughness_to_alpha),
      fresnel);
}

const Bxdf* MakeMicrofacetDielectricBtdf(BxdfAllocator& bxdf_allocator,
                                         const Reflector* transmittance,
                                         geometric_t eta_incident,
                                         geometric_t eta_transmitted,
                                         geometric_t roughness_x,
                                         geometric_t roughness_y,
                                         bool remap_roughness_to_alpha) {
  if (!transmittance ||
      !FresnelDielectric(eta_incident, eta_transmitted).IsValid()) {
    return nullptr;
  }

  return &bxdf_allocator.Allocate<MicrofacetBtdf<TrowbridgeReitzDistribution>>(
      *transmittance,
      TrowbridgeReitzDistribution(roughness_x, roughness_y,
                                  remap_roughness_to_alpha),
      eta_incident, eta_transmitted);
}

}  // namespace bxdfs
}  // namespace iris
