#ifndef _IRIS_MATERIALS_MIRROR_MATERIAL_
#define _IRIS_MATERIALS_MIRROR_MATERIAL_

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

class MirrorMaterial final : public Material {
 public:
  MirrorMaterial(iris::ReferenceCounted<
                 textures::PointerTexture2D<Reflector, SpectralAllocator>>
                     reflectance)
      : reflectance_(std::move(reflectance)) {
    assert(reflectance_);
  }

  const Bxdf* Evaluate(const TextureCoordinates& texture_coordinates,
                       SpectralAllocator& spectral_allocator,
                       BxdfAllocator& bxdf_allocator) const override;

 private:
  iris::ReferenceCounted<
      textures::PointerTexture2D<Reflector, SpectralAllocator>>
      reflectance_;
};

}  // namespace materials
}  // namespace iris

#endif  // _IRIS_MATERIALS_MIRROR_MATERIAL_