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
  const Reflector* reflector = nullptr;
  if (reflectance_) {
    reflector = reflectance_->Evaluate(texture_coordinates, spectral_allocator);
  }

  const Reflector* transmittance = nullptr;
  if (transmittance_) {
    transmittance =
        transmittance_->Evaluate(texture_coordinates, spectral_allocator);
  }

  if (reflector == nullptr && transmittance == nullptr) {
    return nullptr;
  }

  // Consider clamping these values
  visual eta_incident = static_cast<visual>(0.0);
  if (eta_incident_) {
    eta_incident = eta_incident_->Evaluate(texture_coordinates);
  }

  visual eta_transmitted = static_cast<visual>(0.0);
  if (eta_transmitted_) {
    eta_transmitted = eta_transmitted_->Evaluate(texture_coordinates);
  }

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

ReferenceCounted<Material> MakeGlassMaterial(
    ReferenceCounted<textures::PointerTexture2D<Reflector, SpectralAllocator>>
        reflectance,
    ReferenceCounted<textures::PointerTexture2D<Reflector, SpectralAllocator>>
        transmittance,
    ReferenceCounted<textures::ValueTexture2D<visual>> eta_incident,
    ReferenceCounted<textures::ValueTexture2D<visual>> eta_transmitted) {
  if (!reflectance && !transmittance) {
    return ReferenceCounted<Material>();
  }

  return MakeReferenceCounted<GlassMaterial>(
      std::move(reflectance), std::move(transmittance), std::move(eta_incident),
      std::move(eta_transmitted));
}

}  // namespace materials
}  // namespace iris
