#ifndef _IRIS_MATERIALS_TRANSLUCENT_MATERIAL_
#define _IRIS_MATERIALS_TRANSLUCENT_MATERIAL_

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

class TranslucentMaterial final : public Material {
 public:
  TranslucentMaterial(
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
      iris::ReferenceCounted<textures::ValueTexture2D<visual>> roughness,
      bool remap_roughness)
      : reflectance_(std::move(reflectance)),
        transmittance_(std::move(transmittance)),
        diffuse_(std::move(diffuse)),
        specular_(std::move(specular)),
        roughness_(std::move(roughness)),
        remap_roughness_(remap_roughness) {
    assert(reflectance_);
    assert(transmittance_);
    assert(diffuse_);
    assert(specular_);
    assert(roughness_);
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
  iris::ReferenceCounted<textures::ValueTexture2D<visual>> roughness_;
  bool remap_roughness_;
};

}  // namespace materials
}  // namespace iris

#endif  // _IRIS_MATERIALS_TRANSLUCENT_MATERIAL_