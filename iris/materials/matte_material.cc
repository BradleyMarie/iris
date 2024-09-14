#include "iris/materials/matte_material.h"

#include "iris/bxdfs/lambertian_bxdf.h"
#include "iris/bxdfs/oren_nayar_bxdf.h"

namespace iris {
namespace materials {

using ::iris::bxdfs::LambertianBrdf;
using ::iris::bxdfs::OrenNayarBrdf;

const Bxdf* MatteMaterial::Evaluate(
    const TextureCoordinates& texture_coordinates,
    SpectralAllocator& spectral_allocator,
    BxdfAllocator& bxdf_allocator) const {
  const Reflector* reflector =
      reflectance_->Evaluate(texture_coordinates, spectral_allocator);
  if (reflector == nullptr) {
    return nullptr;
  }

  visual sigma = sigma_->Evaluate(texture_coordinates);
  if (sigma <= static_cast<visual>(0.0)) {
    return &bxdf_allocator.Allocate<LambertianBrdf>(*reflector);
  }

  return &bxdf_allocator.Allocate<OrenNayarBrdf>(*reflector, sigma);
}

}  // namespace materials
}  // namespace iris