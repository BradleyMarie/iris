#include "iris/materials/matte_material.h"

#include <cassert>

#include "iris/bxdf.h"
#include "iris/bxdf_allocator.h"
#include "iris/bxdfs/lambertian_bxdf.h"
#include "iris/bxdfs/oren_nayar_bxdf.h"
#include "iris/float.h"
#include "iris/material.h"
#include "iris/reference_counted.h"
#include "iris/spectral_allocator.h"
#include "iris/texture_coordinates.h"
#include "iris/textures/texture2d.h"

namespace iris {
namespace materials {
namespace {

using ::iris::bxdfs::LambertianBrdf;
using ::iris::bxdfs::OrenNayarBrdf;

class MatteMaterial final : public Material {
 public:
  MatteMaterial(
      ReferenceCounted<textures::PointerTexture2D<Reflector, SpectralAllocator>>
          reflectance,
      ReferenceCounted<textures::ValueTexture2D<visual>> sigma)
      : reflectance_(std::move(reflectance)), sigma_(std::move(sigma)) {
    assert(reflectance_);
  }

  const Bxdf* Evaluate(const TextureCoordinates& texture_coordinates,
                       SpectralAllocator& spectral_allocator,
                       BxdfAllocator& bxdf_allocator) const override;

 private:
  ReferenceCounted<textures::PointerTexture2D<Reflector, SpectralAllocator>>
      reflectance_;
  ReferenceCounted<textures::ValueTexture2D<visual>> sigma_;
};

const Bxdf* MatteMaterial::Evaluate(
    const TextureCoordinates& texture_coordinates,
    SpectralAllocator& spectral_allocator,
    BxdfAllocator& bxdf_allocator) const {
  const Reflector* reflector =
      reflectance_->Evaluate(texture_coordinates, spectral_allocator);
  if (reflector == nullptr) {
    return nullptr;
  }

  visual sigma = static_cast<visual>(0.0);
  if (sigma_) {
    sigma = sigma_->Evaluate(texture_coordinates);
  }

  if (sigma <= static_cast<visual>(0.0)) {
    return &bxdf_allocator.Allocate<LambertianBrdf>(*reflector);
  }

  return &bxdf_allocator.Allocate<OrenNayarBrdf>(*reflector, sigma);
}

}  // namespace

ReferenceCounted<Material> MakeMatteMaterial(
    ReferenceCounted<textures::PointerTexture2D<Reflector, SpectralAllocator>>
        reflectance,
    ReferenceCounted<textures::ValueTexture2D<visual>> sigma) {
  if (!reflectance) {
    return ReferenceCounted<Material>();
  }

  return MakeReferenceCounted<MatteMaterial>(std::move(reflectance),
                                             std::move(sigma));
}

}  // namespace materials
}  // namespace iris
