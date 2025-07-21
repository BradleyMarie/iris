#include "iris/materials/mirror_material.h"

#include <utility>

#include "iris/bxdf.h"
#include "iris/bxdf_allocator.h"
#include "iris/bxdfs/mirror_brdf.h"
#include "iris/float.h"
#include "iris/material.h"
#include "iris/reference_counted.h"
#include "iris/spectral_allocator.h"
#include "iris/texture_coordinates.h"
#include "iris/textures/reflector_texture.h"

namespace iris {
namespace materials {
namespace {

using ::iris::bxdfs::MakeMirrorBrdf;
using ::iris::textures::ReflectorTexture;

class MirrorMaterial final : public Material {
 public:
  MirrorMaterial(ReferenceCounted<ReflectorTexture> reflectance)
      : reflectance_(std::move(reflectance)) {}

  const Bxdf* Evaluate(const TextureCoordinates& texture_coordinates,
                       SpectralAllocator& spectral_allocator,
                       BxdfAllocator& bxdf_allocator) const override;

 private:
  ReferenceCounted<ReflectorTexture> reflectance_;
};

const Bxdf* MirrorMaterial::Evaluate(
    const TextureCoordinates& texture_coordinates,
    SpectralAllocator& spectral_allocator,
    BxdfAllocator& bxdf_allocator) const {
  return MakeMirrorBrdf(
      bxdf_allocator,
      reflectance_->Evaluate(texture_coordinates, spectral_allocator));
}

}  // namespace

ReferenceCounted<Material> MakeMirrorMaterial(
    ReferenceCounted<ReflectorTexture> reflectance) {
  if (!reflectance) {
    return ReferenceCounted<Material>();
  }

  return MakeReferenceCounted<MirrorMaterial>(std::move(reflectance));
}

}  // namespace materials
}  // namespace iris
