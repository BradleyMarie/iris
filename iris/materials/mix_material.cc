#include "iris/materials/mix_material.h"

#include "iris/bxdfs/attenuated_bxdf.h"
#include "iris/bxdfs/composite_bxdf.h"

namespace iris {
namespace materials {

const Bxdf* MixMaterial::Evaluate(const TextureCoordinates& texture_coordinates,
                                  SpectralAllocator& spectral_allocator,
                                  BxdfAllocator& bxdf_allocator) const {
  visual_t attenuation = interpolation_->Evaluate(texture_coordinates);

  const Bxdf* bxdf0 = material0_->Evaluate(texture_coordinates,
                                           spectral_allocator, bxdf_allocator);
  bxdf0 = bxdfs::CreateAttenuatedBxdf(bxdf_allocator, bxdf0, attenuation);

  const Bxdf* bxdf1 = material1_->Evaluate(texture_coordinates,
                                           spectral_allocator, bxdf_allocator);
  bxdf1 = bxdfs::CreateAttenuatedBxdf(bxdf_allocator, bxdf1,
                                      static_cast<visual_t>(1.0) - attenuation);

  return bxdfs::MakeCompositeBxdf(bxdf_allocator, bxdf0, bxdf1);
}

}  // namespace materials
}  // namespace iris