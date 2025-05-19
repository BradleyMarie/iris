#include "iris/materials/mix_material.h"

#include "iris/bxdf.h"
#include "iris/bxdf_allocator.h"
#include "iris/bxdfs/attenuated_bxdf.h"
#include "iris/bxdfs/composite_bxdf.h"
#include "iris/float.h"
#include "iris/material.h"
#include "iris/reference_counted.h"
#include "iris/spectral_allocator.h"
#include "iris/texture_coordinates.h"
#include "iris/textures/texture2d.h"

namespace iris {
namespace materials {
namespace {

class MixMaterial final : public Material {
 public:
  MixMaterial(ReferenceCounted<Material> material0,
              ReferenceCounted<Material> material1,
              ReferenceCounted<textures::ValueTexture2D<visual>> interpolation)
      : material0_(std::move(material0)),
        material1_(std::move(material1)),
        interpolation_(std::move(interpolation)) {}

  const Bxdf* Evaluate(const TextureCoordinates& texture_coordinates,
                       SpectralAllocator& spectral_allocator,
                       BxdfAllocator& bxdf_allocator) const override;

 private:
  ReferenceCounted<Material> material0_;
  ReferenceCounted<Material> material1_;
  ReferenceCounted<textures::ValueTexture2D<visual>> interpolation_;
};

const Bxdf* MixMaterial::Evaluate(const TextureCoordinates& texture_coordinates,
                                  SpectralAllocator& spectral_allocator,
                                  BxdfAllocator& bxdf_allocator) const {
  visual attenuation = static_cast<visual>(0.0);
  if (interpolation_) {
    attenuation = interpolation_->Evaluate(texture_coordinates);
  }

  const Bxdf* bxdf0 = nullptr;
  if (material0_) {
    bxdf0 = material0_->Evaluate(texture_coordinates, spectral_allocator,
                                 bxdf_allocator);
    bxdf0 = bxdfs::CreateAttenuatedBxdf(bxdf_allocator, bxdf0, attenuation);
  }

  const Bxdf* bxdf1 = nullptr;
  if (material1_) {
    bxdf1 = material1_->Evaluate(texture_coordinates, spectral_allocator,
                                 bxdf_allocator);
    bxdf1 = bxdfs::CreateAttenuatedBxdf(
        bxdf_allocator, bxdf1, static_cast<visual_t>(1.0) - attenuation);
  }

  return bxdfs::MakeCompositeBxdf(bxdf_allocator, bxdf0, bxdf1);
}

}  // namespace

ReferenceCounted<Material> MakeMixMaterial(
    ReferenceCounted<Material> material0, ReferenceCounted<Material> material1,
    ReferenceCounted<textures::ValueTexture2D<visual>> interpolation) {
  return MakeReferenceCounted<MixMaterial>(
      std::move(material0), std::move(material1), std::move(interpolation));
}

}  // namespace materials
}  // namespace iris
