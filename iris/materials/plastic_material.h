#ifndef _IRIS_MATERIALS_PLASTIC_MATERIAL_
#define _IRIS_MATERIALS_PLASTIC_MATERIAL_

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

class PlasticMaterial final : public Material {
 public:
  PlasticMaterial(
      iris::ReferenceCounted<
          textures::PointerTexture2D<Reflector, SpectralAllocator>>
          diffuse,
      iris::ReferenceCounted<
          textures::PointerTexture2D<Reflector, SpectralAllocator>>
          specular,
      iris::ReferenceCounted<textures::ValueTexture2D<visual>> eta_incident,
      iris::ReferenceCounted<textures::ValueTexture2D<visual>> eta_transmitted,
      iris::ReferenceCounted<textures::ValueTexture2D<visual>> roughness,
      bool remap_roughness)
      : diffuse_(std::move(diffuse)),
        specular_(std::move(specular)),
        eta_incident_(std::move(eta_incident)),
        eta_transmitted_(std::move(eta_transmitted)),
        roughness_(std::move(roughness)),
        remap_roughness_(remap_roughness) {
    assert(diffuse_);
    assert(specular_);
    assert(eta_incident_);
    assert(eta_transmitted_);
    assert(roughness_);
  }

  const Bxdf* Evaluate(const TextureCoordinates& texture_coordinates,
                       SpectralAllocator& spectral_allocator,
                       BxdfAllocator& bxdf_allocator) const override;

 private:
  iris::ReferenceCounted<
      textures::PointerTexture2D<Reflector, SpectralAllocator>>
      diffuse_;
  iris::ReferenceCounted<
      textures::PointerTexture2D<Reflector, SpectralAllocator>>
      specular_;
  iris::ReferenceCounted<textures::ValueTexture2D<visual>> eta_incident_;
  iris::ReferenceCounted<textures::ValueTexture2D<visual>> eta_transmitted_;
  iris::ReferenceCounted<textures::ValueTexture2D<visual>> roughness_;
  bool remap_roughness_;
};

}  // namespace materials
}  // namespace iris

#endif  // _IRIS_MATERIALS_PLASTIC_MATERIAL_