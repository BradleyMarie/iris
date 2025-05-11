#ifndef _IRIS_MATERIALS_GLASS_MATERIAL_
#define _IRIS_MATERIALS_GLASS_MATERIAL_

#include <cassert>

#include "iris/bxdf.h"
#include "iris/bxdf_allocator.h"
#include "iris/float.h"
#include "iris/material.h"
#include "iris/reference_counted.h"
#include "iris/spectral_allocator.h"
#include "iris/texture_coordinates.h"
#include "iris/textures/texture2d.h"

namespace iris {
namespace materials {

class GlassMaterial final : public Material {
 public:
  GlassMaterial(
      ReferenceCounted<textures::PointerTexture2D<Reflector, SpectralAllocator>>
          reflectance,
      ReferenceCounted<textures::PointerTexture2D<Reflector, SpectralAllocator>>
          transmittance,
      ReferenceCounted<textures::ValueTexture2D<visual>> eta_incident,
      ReferenceCounted<textures::ValueTexture2D<visual>> eta_transmitted)
      : reflectance_(std::move(reflectance)),
        transmittance_(std::move(transmittance)),
        eta_incident_(std::move(eta_incident)),
        eta_transmitted_(std::move(eta_transmitted)) {}

  const Bxdf* Evaluate(const TextureCoordinates& texture_coordinates,
                       SpectralAllocator& spectral_allocator,
                       BxdfAllocator& bxdf_allocator) const override;

 private:
  ReferenceCounted<textures::PointerTexture2D<Reflector, SpectralAllocator>>
      reflectance_;
  ReferenceCounted<textures::PointerTexture2D<Reflector, SpectralAllocator>>
      transmittance_;
  ReferenceCounted<textures::ValueTexture2D<visual>> eta_incident_;
  ReferenceCounted<textures::ValueTexture2D<visual>> eta_transmitted_;
};

ReferenceCounted<Material> MakeGlassMaterial(
    ReferenceCounted<textures::PointerTexture2D<Reflector, SpectralAllocator>>
        reflectance,
    ReferenceCounted<textures::PointerTexture2D<Reflector, SpectralAllocator>>
        transmittance,
    ReferenceCounted<textures::ValueTexture2D<visual>> eta_incident,
    ReferenceCounted<textures::ValueTexture2D<visual>> eta_transmitted);

}  // namespace materials
}  // namespace iris

#endif  // _IRIS_MATERIALS_GLASS_MATERIAL_
