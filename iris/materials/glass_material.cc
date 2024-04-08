#include "iris/materials/glass_material.h"

#include "iris/bxdfs/specular_bxdf.h"

namespace iris {
namespace materials {

const Bxdf* GlassMaterial::Evaluate(
    const TextureCoordinates& texture_coordinates,
    SpectralAllocator& spectral_allocator,
    BxdfAllocator& bxdf_allocator) const {
  auto* reflector =
      reflectance_->Evaluate(texture_coordinates, spectral_allocator);
  auto* transmittance =
      transmittance_->Evaluate(texture_coordinates, spectral_allocator);

  if (reflector == nullptr && transmittance == nullptr) {
    return nullptr;
  }

  // Consider clamping these values
  visual eta_incident = eta_incident_->Evaluate(texture_coordinates);
  visual eta_transmitted = eta_transmitted_->Evaluate(texture_coordinates);

  if (transmittance == nullptr) {
    return &bxdf_allocator
                .Allocate<bxdfs::SpecularBrdf<bxdfs::FresnelDielectric>>(
                    *reflector,
                    bxdfs::FresnelDielectric(eta_incident, eta_transmitted));
  }

  if (reflector == nullptr) {
    return &bxdf_allocator
                .Allocate<bxdfs::SpecularBtdf<bxdfs::FresnelDielectric>>(
                    *transmittance, eta_incident, eta_transmitted,
                    bxdfs::FresnelDielectric(eta_incident, eta_transmitted));
  }

  return &bxdf_allocator.Allocate<bxdfs::SpecularBxdf>(
      *reflector, *transmittance, eta_incident, eta_transmitted);
}

}  // namespace materials
}  // namespace iris