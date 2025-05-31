#include "iris/materials/mirror_material.h"

#include <utility>

#include "iris/bxdf.h"
#include "iris/bxdf_allocator.h"
#include "iris/bxdfs/specular_bxdf.h"
#include "iris/float.h"
#include "iris/material.h"
#include "iris/reference_counted.h"
#include "iris/spectral_allocator.h"
#include "iris/texture_coordinates.h"
#include "iris/textures/texture2d.h"

namespace iris {
namespace materials {
namespace {

using ::iris::bxdfs::FresnelNoOp;
using ::iris::bxdfs::MakeSpecularBrdf;
using ::iris::textures::PointerTexture2D;

class MirrorMaterial final : public Material {
 public:
  MirrorMaterial(
      ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
          reflectance)
      : reflectance_(std::move(reflectance)) {}

  const Bxdf* Evaluate(const TextureCoordinates& texture_coordinates,
                       SpectralAllocator& spectral_allocator,
                       BxdfAllocator& bxdf_allocator) const override;

 private:
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> reflectance_;
};

const Bxdf* MirrorMaterial::Evaluate(
    const TextureCoordinates& texture_coordinates,
    SpectralAllocator& spectral_allocator,
    BxdfAllocator& bxdf_allocator) const {
  return MakeSpecularBrdf(
      bxdf_allocator,
      reflectance_->Evaluate(texture_coordinates, spectral_allocator),
      FresnelNoOp());
}

}  // namespace

ReferenceCounted<Material> MakeMirrorMaterial(
    ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
        reflectance) {
  if (!reflectance) {
    return ReferenceCounted<Material>();
  }

  return MakeReferenceCounted<MirrorMaterial>(std::move(reflectance));
}

}  // namespace materials
}  // namespace iris
