#include "iris/materials/mirror_material.h"

#include "iris/bxdfs/specular_bxdf.h"

namespace iris {
namespace materials {

const Bxdf* MirrorMaterial::Evaluate(
    const TextureCoordinates& texture_coordinates,
    SpectralAllocator& spectral_allocator,
    BxdfAllocator& bxdf_allocator) const {
  auto* reflector =
      reflectance_->Evaluate(texture_coordinates, spectral_allocator);
  if (reflector == nullptr) {
    return nullptr;
  }

  return &bxdf_allocator.Allocate<bxdfs::SpecularBrdf<bxdfs::FresnelNoOp>>(
      *reflector, bxdfs::FresnelNoOp());
}

}  // namespace materials
}  // namespace iris