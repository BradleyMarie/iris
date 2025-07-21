#include "iris/materials/matte_material.h"

#include <utility>

#include "iris/bxdf.h"
#include "iris/bxdf_allocator.h"
#include "iris/bxdfs/lambertian_bxdf.h"
#include "iris/bxdfs/oren_nayar_bxdf.h"
#include "iris/float.h"
#include "iris/material.h"
#include "iris/reference_counted.h"
#include "iris/spectral_allocator.h"
#include "iris/texture_coordinates.h"
#include "iris/textures/float_texture.h"
#include "iris/textures/reflector_texture.h"

namespace iris {
namespace materials {
namespace {

using ::iris::bxdfs::MakeLambertianBrdf;
using ::iris::bxdfs::MakeOrenNayarBrdf;
using ::iris::textures::FloatTexture;
using ::iris::textures::ReflectorTexture;

class MatteMaterial final : public Material {
 public:
  MatteMaterial(ReferenceCounted<ReflectorTexture> reflectance,
                ReferenceCounted<FloatTexture> sigma)
      : reflectance_(std::move(reflectance)), sigma_(std::move(sigma)) {}

  const Bxdf* Evaluate(const TextureCoordinates& texture_coordinates,
                       SpectralAllocator& spectral_allocator,
                       BxdfAllocator& bxdf_allocator) const override;

 private:
  ReferenceCounted<ReflectorTexture> reflectance_;
  ReferenceCounted<FloatTexture> sigma_;
};

const Bxdf* MatteMaterial::Evaluate(
    const TextureCoordinates& texture_coordinates,
    SpectralAllocator& spectral_allocator,
    BxdfAllocator& bxdf_allocator) const {
  const Reflector* reflectance = nullptr;
  if (reflectance_) {
    reflectance =
        reflectance_->Evaluate(texture_coordinates, spectral_allocator);
  }

  visual sigma = static_cast<visual>(0.0);
  if (sigma_) {
    sigma = sigma_->Evaluate(texture_coordinates);
  }

  return sigma <= static_cast<visual>(0.0)
             ? MakeLambertianBrdf(bxdf_allocator, reflectance)
             : MakeOrenNayarBrdf(bxdf_allocator, reflectance, sigma);
}

}  // namespace

ReferenceCounted<Material> MakeMatteMaterial(
    ReferenceCounted<ReflectorTexture> reflectance,
    ReferenceCounted<FloatTexture> sigma) {
  if (!reflectance) {
    return ReferenceCounted<Material>();
  }

  return MakeReferenceCounted<MatteMaterial>(std::move(reflectance),
                                             std::move(sigma));
}

}  // namespace materials
}  // namespace iris
