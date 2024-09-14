#include "iris/materials/mirror_material.h"

#include "iris/bxdfs/specular_bxdf.h"

namespace iris {
namespace materials {

using ::iris::bxdfs::FresnelNoOp;
using ::iris::bxdfs::SpecularBrdf;

const Bxdf* MirrorMaterial::Evaluate(
    const TextureCoordinates& texture_coordinates,
    SpectralAllocator& spectral_allocator,
    BxdfAllocator& bxdf_allocator) const {
  const Reflector* reflector =
      reflectance_->Evaluate(texture_coordinates, spectral_allocator);
  if (reflector == nullptr) {
    return nullptr;
  }

  return &bxdf_allocator.Allocate<SpecularBrdf<FresnelNoOp>>(*reflector,
                                                             FresnelNoOp());
}

}  // namespace materials
}  // namespace iris