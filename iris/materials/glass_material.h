#ifndef _IRIS_MATERIALS_GLASS_MATERIAL_
#define _IRIS_MATERIALS_GLASS_MATERIAL_

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

class GlassMaterial final : public Material {
 public:
  GlassMaterial(
      iris::ReferenceCounted<
          textures::PointerTexture2D<Reflector, SpectralAllocator>>
          reflectance,
      iris::ReferenceCounted<
          textures::PointerTexture2D<Reflector, SpectralAllocator>>
          transmittance,
      iris::ReferenceCounted<textures::ValueTexture2D<visual>> eta_front,
      iris::ReferenceCounted<textures::ValueTexture2D<visual>> eta_back)
      : reflectance_(std::move(reflectance)),
        transmittance_(std::move(transmittance)),
        eta_front_(std::move(eta_front)),
        eta_back_(std::move(eta_back)) {
    assert(reflectance_);
    assert(transmittance_);
    assert(eta_front_);
    assert(eta_back_);
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
  iris::ReferenceCounted<textures::ValueTexture2D<visual>> eta_front_;
  iris::ReferenceCounted<textures::ValueTexture2D<visual>> eta_back_;
};

}  // namespace materials
}  // namespace iris

#endif  // _IRIS_MATERIALS_GLASS_MATERIAL_