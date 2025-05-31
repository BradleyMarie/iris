#include "iris/materials/glass_material.h"

#include <utility>

#include "iris/bxdf.h"
#include "iris/bxdf_allocator.h"
#include "iris/bxdfs/specular_bxdf.h"
#include "iris/float.h"
#include "iris/material.h"
#include "iris/reference_counted.h"
#include "iris/spectral_allocator.h"
#include "iris/texture_coordinates.h"
#include "iris/textures/texture2d.h"

namespace iris {
namespace materials {
namespace {

using ::iris::bxdfs::FresnelDielectric;
using ::iris::bxdfs::MakeSpecularBxdf;
using ::iris::textures::PointerTexture2D;
using ::iris::textures::ValueTexture2D;

class GlassMaterial final : public Material {
 public:
  GlassMaterial(ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
                    reflectance,
                ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
                    transmittance,
                ReferenceCounted<ValueTexture2D<visual>> eta_incident,
                ReferenceCounted<ValueTexture2D<visual>> eta_transmitted)
      : reflectance_(std::move(reflectance)),
        transmittance_(std::move(transmittance)),
        eta_incident_(std::move(eta_incident)),
        eta_transmitted_(std::move(eta_transmitted)) {}

  const Bxdf* Evaluate(const TextureCoordinates& texture_coordinates,
                       SpectralAllocator& spectral_allocator,
                       BxdfAllocator& bxdf_allocator) const override;

 private:
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> reflectance_;
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
      transmittance_;
  ReferenceCounted<ValueTexture2D<visual>> eta_incident_;
  ReferenceCounted<ValueTexture2D<visual>> eta_transmitted_;
};

const Bxdf* GlassMaterial::Evaluate(
    const TextureCoordinates& texture_coordinates,
    SpectralAllocator& spectral_allocator,
    BxdfAllocator& bxdf_allocator) const {
  const Reflector* reflectance = nullptr;
  if (reflectance_) {
    reflectance =
        reflectance_->Evaluate(texture_coordinates, spectral_allocator);
  }

  const Reflector* transmittance = nullptr;
  if (transmittance_) {
    transmittance =
        transmittance_->Evaluate(texture_coordinates, spectral_allocator);
  }

  visual eta_incident = static_cast<visual>(0.0);
  if (eta_incident_) {
    eta_incident = eta_incident_->Evaluate(texture_coordinates);
  }

  visual eta_transmitted = static_cast<visual>(0.0);
  if (eta_transmitted_) {
    eta_transmitted = eta_transmitted_->Evaluate(texture_coordinates);
  }

  return MakeSpecularBxdf(bxdf_allocator, reflectance, transmittance,
                          eta_incident, eta_transmitted);
}

}  // namespace

ReferenceCounted<Material> MakeGlassMaterial(
    ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
        reflectance,
    ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
        transmittance,
    ReferenceCounted<ValueTexture2D<visual>> eta_incident,
    ReferenceCounted<ValueTexture2D<visual>> eta_transmitted) {
  if (!eta_incident || !eta_transmitted) {
    eta_incident.Reset();
    eta_transmitted.Reset();
    transmittance.Reset();
  }

  if (!reflectance && !transmittance) {
    return ReferenceCounted<Material>();
  }

  return MakeReferenceCounted<GlassMaterial>(
      std::move(reflectance), std::move(transmittance), std::move(eta_incident),
      std::move(eta_transmitted));
}

}  // namespace materials
}  // namespace iris
