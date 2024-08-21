#ifndef _IRIS_MATERIALS_UBER_MATERIAL_
#define _IRIS_MATERIALS_UBER_MATERIAL_

#include <cassert>
#include <memory>

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

class UberMaterial final : public Material {
 public:
  UberMaterial(
      iris::ReferenceCounted<
          textures::PointerTexture2D<Reflector, SpectralAllocator>>
          reflectance,
      iris::ReferenceCounted<
          textures::PointerTexture2D<Reflector, SpectralAllocator>>
          transmittance,
      iris::ReferenceCounted<
          textures::PointerTexture2D<Reflector, SpectralAllocator>>
          diffuse,
      iris::ReferenceCounted<
          textures::PointerTexture2D<Reflector, SpectralAllocator>>
          specular,
      iris::ReferenceCounted<textures::ValueTexture2D<visual>> opacity,
      iris::ReferenceCounted<textures::ValueTexture2D<visual>> eta_incident,
      iris::ReferenceCounted<textures::ValueTexture2D<visual>> eta_transmitted,
      iris::ReferenceCounted<textures::ValueTexture2D<visual>> roughness_u,
      iris::ReferenceCounted<textures::ValueTexture2D<visual>> roughness_v,
      bool remap_roughness)
      : reflectance_(std::move(reflectance)),
        transmittance_(std::move(transmittance)),
        diffuse_(std::move(diffuse)),
        specular_(std::move(specular)),
        opacity_(std::move(opacity)),
        eta_incident_(std::move(eta_incident)),
        eta_transmitted_(std::move(eta_transmitted)),
        roughness_u_(std::move(roughness_u)),
        roughness_v_(std::move(roughness_v)),
        remap_roughness_(remap_roughness) {
    assert(reflectance_);
    assert(transmittance_);
    assert(diffuse_);
    assert(specular_);
    assert(opacity_);
    assert(eta_incident_);
    assert(eta_transmitted_);
    assert(roughness_u_);
    assert(roughness_v_);
  }

  const Bxdf* Evaluate(const TextureCoordinates& texture_coordinates,
                       SpectralAllocator& spectral_allocator,
                       BxdfAllocator& bxdf_allocator) const override;

 private:
  iris::ReferenceCounted<
      textures::PointerTexture2D<Reflector, SpectralAllocator>>
      reflectance_;
  iris::ReferenceCounted<
      textures::PointerTexture2D<Reflector, SpectralAllocator>>
      transmittance_;
  iris::ReferenceCounted<
      textures::PointerTexture2D<Reflector, SpectralAllocator>>
      diffuse_;
  iris::ReferenceCounted<
      textures::PointerTexture2D<Reflector, SpectralAllocator>>
      specular_;
  iris::ReferenceCounted<textures::ValueTexture2D<visual>> opacity_;
  iris::ReferenceCounted<textures::ValueTexture2D<visual>> eta_incident_;
  iris::ReferenceCounted<textures::ValueTexture2D<visual>> eta_transmitted_;
  iris::ReferenceCounted<textures::ValueTexture2D<visual>> roughness_u_;
  iris::ReferenceCounted<textures::ValueTexture2D<visual>> roughness_v_;
  bool remap_roughness_;
};

}  // namespace materials
}  // namespace iris

#endif  // _IRIS_MATERIALS_TRANSLUCENT_MATERIAL_