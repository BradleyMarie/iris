#ifndef _IRIS_MATERIALS_MATTE_MATERIAL_
#define _IRIS_MATERIALS_MATTE_MATERIAL_

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

class MatteMaterial final : public Material {
 public:
  MatteMaterial(
      ReferenceCounted<textures::PointerTexture2D<Reflector, SpectralAllocator>>
          reflectance,
      ReferenceCounted<textures::ValueTexture2D<visual>> sigma)
      : reflectance_(std::move(reflectance)), sigma_(std::move(sigma)) {
    assert(reflectance_);
  }

  const Bxdf* Evaluate(const TextureCoordinates& texture_coordinates,
                       SpectralAllocator& spectral_allocator,
                       BxdfAllocator& bxdf_allocator) const override;

 private:
  ReferenceCounted<textures::PointerTexture2D<Reflector, SpectralAllocator>>
      reflectance_;
  ReferenceCounted<textures::ValueTexture2D<visual>> sigma_;
};

ReferenceCounted<Material> MakeMatteMaterial(
    ReferenceCounted<textures::PointerTexture2D<Reflector, SpectralAllocator>>
        reflectance,
    ReferenceCounted<textures::ValueTexture2D<visual>> sigma);

}  // namespace materials
}  // namespace iris

#endif  // _IRIS_MATERIALS_MATTE_MATERIAL_
