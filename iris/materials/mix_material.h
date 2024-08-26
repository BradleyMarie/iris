#ifndef _IRIS_MATERIALS_MIX_MATERIAL_
#define _IRIS_MATERIALS_MIX_MATERIAL_

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

class MixMaterial final : public Material {
 public:
  MixMaterial(ReferenceCounted<Material> material0,
              ReferenceCounted<Material> material1,
              ReferenceCounted<textures::ValueTexture2D<visual>> interpolation)
      : material0_(std::move(material0)),
        material1_(std::move(material1)),
        interpolation_(std::move(interpolation)) {
    assert(material0_);
    assert(material1_);
    assert(interpolation_);
  }

  const Bxdf* Evaluate(const TextureCoordinates& texture_coordinates,
                       SpectralAllocator& spectral_allocator,
                       BxdfAllocator& bxdf_allocator) const override;

 private:
  ReferenceCounted<Material> material0_;
  ReferenceCounted<Material> material1_;
  ReferenceCounted<textures::ValueTexture2D<visual>> interpolation_;
};

}  // namespace materials
}  // namespace iris

#endif  // _IRIS_MATERIALS_TRANSLUCENT_MATERIAL_