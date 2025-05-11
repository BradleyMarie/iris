#ifndef _IRIS_MATERIALS_TRANSLUCENT_MATERIAL_
#define _IRIS_MATERIALS_TRANSLUCENT_MATERIAL_

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

class TranslucentMaterial final : public Material {
 public:
  TranslucentMaterial(
      ReferenceCounted<textures::PointerTexture2D<Reflector, SpectralAllocator>>
          reflectance,
      ReferenceCounted<textures::PointerTexture2D<Reflector, SpectralAllocator>>
          transmittance,
      ReferenceCounted<textures::PointerTexture2D<Reflector, SpectralAllocator>>
          diffuse,
      ReferenceCounted<textures::PointerTexture2D<Reflector, SpectralAllocator>>
          specular,
      ReferenceCounted<textures::ValueTexture2D<visual>> eta_incident,
      ReferenceCounted<textures::ValueTexture2D<visual>> eta_transmitted,
      ReferenceCounted<textures::ValueTexture2D<visual>> roughness,
      bool remap_roughness)
      : reflectance_(std::move(reflectance)),
        transmittance_(std::move(transmittance)),
        diffuse_(std::move(diffuse)),
        specular_(std::move(specular)),
        eta_incident_(std::move(eta_incident)),
        eta_transmitted_(std::move(eta_transmitted)),
        roughness_(std::move(roughness)),
        remap_roughness_(remap_roughness) {}

  const Bxdf* Evaluate(const TextureCoordinates& texture_coordinates,
                       SpectralAllocator& spectral_allocator,
                       BxdfAllocator& bxdf_allocator) const override;

 private:
  ReferenceCounted<textures::PointerTexture2D<Reflector, SpectralAllocator>>
      reflectance_;
  ReferenceCounted<textures::PointerTexture2D<Reflector, SpectralAllocator>>
      transmittance_;
  ReferenceCounted<textures::PointerTexture2D<Reflector, SpectralAllocator>>
      diffuse_;
  ReferenceCounted<textures::PointerTexture2D<Reflector, SpectralAllocator>>
      specular_;
  ReferenceCounted<textures::ValueTexture2D<visual>> eta_incident_;
  ReferenceCounted<textures::ValueTexture2D<visual>> eta_transmitted_;
  ReferenceCounted<textures::ValueTexture2D<visual>> roughness_;
  bool remap_roughness_;
};

ReferenceCounted<Material> MakeTranslucentMaterial(
    ReferenceCounted<textures::PointerTexture2D<Reflector, SpectralAllocator>>
        reflectance,
    ReferenceCounted<textures::PointerTexture2D<Reflector, SpectralAllocator>>
        transmittance,
    ReferenceCounted<textures::PointerTexture2D<Reflector, SpectralAllocator>>
        diffuse,
    ReferenceCounted<textures::PointerTexture2D<Reflector, SpectralAllocator>>
        specular,
    ReferenceCounted<textures::ValueTexture2D<visual>> eta_incident,
    ReferenceCounted<textures::ValueTexture2D<visual>> eta_transmitted,
    ReferenceCounted<textures::ValueTexture2D<visual>> roughness,
    bool remap_roughness);

}  // namespace materials
}  // namespace iris

#endif  // _IRIS_MATERIALS_TRANSLUCENT_MATERIAL_
