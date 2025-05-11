#ifndef _IRIS_MATERIALS_METAL_MATERIAL_
#define _IRIS_MATERIALS_METAL_MATERIAL_

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

class MetalMaterial final : public Material {
 public:
  MetalMaterial(ReferenceCounted<Spectrum> k,
                ReferenceCounted<Spectrum> eta_incident,
                ReferenceCounted<Spectrum> eta_transmitted,
                ReferenceCounted<textures::ValueTexture2D<visual>> roughness_u,
                ReferenceCounted<textures::ValueTexture2D<visual>> roughness_v,
                bool remap_roughness)
      : k_(std::move(k)),
        eta_incident_(std::move(eta_incident)),
        eta_transmitted_(std::move(eta_transmitted)),
        roughness_u_(std::move(roughness_u)),
        roughness_v_(std::move(roughness_v)),
        remap_roughness_(remap_roughness) {}

  const Bxdf* Evaluate(const TextureCoordinates& texture_coordinates,
                       SpectralAllocator& spectral_allocator,
                       BxdfAllocator& bxdf_allocator) const override;

 private:
  ReferenceCounted<Spectrum> k_;
  ReferenceCounted<Spectrum> eta_incident_;
  ReferenceCounted<Spectrum> eta_transmitted_;
  ReferenceCounted<textures::ValueTexture2D<visual>> roughness_u_;
  ReferenceCounted<textures::ValueTexture2D<visual>> roughness_v_;
  bool remap_roughness_;
};

}  // namespace materials
}  // namespace iris

#endif  // _IRIS_MATERIALS_METAL_MATERIAL_
