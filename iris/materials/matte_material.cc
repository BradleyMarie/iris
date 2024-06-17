#include "iris/materials/matte_material.h"

#include "iris/bxdfs/lambertian_bxdf.h"
#include "iris/bxdfs/oren_nayar_bxdf.h"

namespace iris {
namespace materials {

const Bxdf* MatteMaterial::Evaluate(
    const TextureCoordinates& texture_coordinates,
    SpectralAllocator& spectral_allocator,
    BxdfAllocator& bxdf_allocator) const {
  auto* reflector =
      reflectance_->Evaluate(texture_coordinates, spectral_allocator);
  if (reflector == nullptr) {
    return nullptr;
  }

  visual_t sigma = sigma_->Evaluate(texture_coordinates);
  if (sigma > static_cast<visual_t>(0.0)) {
    return &bxdf_allocator.Allocate<bxdfs::OrenNayarBrdf>(*reflector, sigma);
  }

  return &bxdf_allocator.Allocate<bxdfs::LambertianBrdf>(*reflector);
}

}  // namespace materials
}  // namespace iris