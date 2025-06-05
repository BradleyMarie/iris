#ifndef _IRIS_BXDFS_MICROFACET_BXDF_
#define _IRIS_BXDFS_MICROFACET_BXDF_

#include "iris/bxdf.h"
#include "iris/bxdf_allocator.h"
#include "iris/float.h"
#include "iris/reflector.h"
#include "iris/spectrum.h"

namespace iris {
namespace bxdfs {

const Bxdf* MakeMicrofacetConductorBrdf(
    BxdfAllocator& bxdf_allocator, const Reflector* reflectance,
    visual_t eta_dielectric, const Spectrum* eta_conductor,
    const Spectrum* k_conductor, geometric_t roughness_x,
    geometric_t roughness_y, bool remap_roughness_to_alpha);

const Bxdf* MakeMicrofacetDielectricBrdf(BxdfAllocator& bxdf_allocator,
                                         const Reflector* reflectance,
                                         geometric_t eta_incident,
                                         geometric_t eta_transmitted,
                                         geometric_t roughness_x,
                                         geometric_t roughness_y,
                                         bool remap_roughness_to_alpha);

const Bxdf* MakeMicrofacetDielectricBtdf(BxdfAllocator& bxdf_allocator,
                                         const Reflector* transmittance,
                                         geometric_t eta_incident,
                                         geometric_t eta_transmitted,
                                         geometric_t roughness_x,
                                         geometric_t roughness_y,
                                         bool remap_roughness_to_alpha);

}  // namespace bxdfs
}  // namespace iris

#endif  // _IRIS_BXDFS_MICROFACET_BXDF_
