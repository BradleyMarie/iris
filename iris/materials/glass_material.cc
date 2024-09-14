#include "iris/materials/glass_material.h"

#include "iris/bxdfs/specular_bxdf.h"

namespace iris {
namespace materials {

using ::iris::bxdfs::FresnelDielectric;
using ::iris::bxdfs::SpecularBrdf;
using ::iris::bxdfs::SpecularBtdf;
using ::iris::bxdfs::SpecularBxdf;

const Bxdf* GlassMaterial::Evaluate(
    const TextureCoordinates& texture_coordinates,
    SpectralAllocator& spectral_allocator,
    BxdfAllocator& bxdf_allocator) const {
  const Reflector* reflector =
      reflectance_->Evaluate(texture_coordinates, spectral_allocator);
  const Reflector* transmittance =
      transmittance_->Evaluate(texture_coordinates, spectral_allocator);

  if (reflector == nullptr && transmittance == nullptr) {
    return nullptr;
  }

  // Consider clamping these values
  visual eta_incident = eta_incident_->Evaluate(texture_coordinates);
  visual eta_transmitted = eta_transmitted_->Evaluate(texture_coordinates);

  if (transmittance == nullptr) {
    return &bxdf_allocator.Allocate<SpecularBrdf<FresnelDielectric>>(
        *reflector, FresnelDielectric(eta_incident, eta_transmitted));
  }

  if (reflector == nullptr) {
    return &bxdf_allocator.Allocate<SpecularBtdf<FresnelDielectric>>(
        *transmittance, eta_incident, eta_transmitted,
        FresnelDielectric(eta_incident, eta_transmitted));
  }

  return &bxdf_allocator.Allocate<SpecularBxdf>(*reflector, *transmittance,
                                                eta_incident, eta_transmitted);
}

}  // namespace materials
}  // namespace iris