#ifndef _IRIS_MATERIALS_SUBSTRATE_MATERIAL_
#define _IRIS_MATERIALS_SUBSTRATE_MATERIAL_

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

class SubstrateMaterial final : public Material {
 public:
  SubstrateMaterial(
      ReferenceCounted<textures::PointerTexture2D<Reflector, SpectralAllocator>>
          diffuse,
      ReferenceCounted<textures::PointerTexture2D<Reflector, SpectralAllocator>>
          specular,
      ReferenceCounted<textures::ValueTexture2D<visual>> roughness_u,
      ReferenceCounted<textures::ValueTexture2D<visual>> roughness_v,
      bool remap_roughness)
      : diffuse_(std::move(diffuse)),
        specular_(std::move(specular)),
        roughness_u_(std::move(roughness_u)),
        roughness_v_(std::move(roughness_v)),
        remap_roughness_(remap_roughness) {
    assert(diffuse_);
    assert(specular_);
    assert(roughness_u_);
    assert(roughness_v_);
  }

  const Bxdf* Evaluate(const TextureCoordinates& texture_coordinates,
                       SpectralAllocator& spectral_allocator,
                       BxdfAllocator& bxdf_allocator) const override;

 private:
  ReferenceCounted<textures::PointerTexture2D<Reflector, SpectralAllocator>>
      diffuse_;
  ReferenceCounted<textures::PointerTexture2D<Reflector, SpectralAllocator>>
      specular_;
  ReferenceCounted<textures::ValueTexture2D<visual>> roughness_u_;
  ReferenceCounted<textures::ValueTexture2D<visual>> roughness_v_;
  bool remap_roughness_;
};

}  // namespace materials
}  // namespace iris

#endif  // _IRIS_MATERIALS_SUBSTRATE_MATERIAL_