#include "iris/materials/disney_material.h"

#include <utility>

#include "iris/bxdf.h"
#include "iris/bxdf_allocator.h"
#include "iris/bxdfs/composite_bxdf.h"
#include "iris/bxdfs/disney_bxdf.h"
#include "iris/bxdfs/lambertian_bxdf.h"
#include "iris/float.h"
#include "iris/material.h"
#include "iris/reference_counted.h"
#include "iris/spectral_allocator.h"
#include "iris/texture_coordinates.h"
#include "iris/textures/float_texture.h"
#include "iris/textures/reflector_texture.h"
#include "iris/textures/spectrum_texture.h"

namespace iris {
namespace materials {
namespace {

using ::iris::bxdfs::MakeCompositeBxdf;
using ::iris::bxdfs::MakeDisneyClearcoatBrdf;
using ::iris::bxdfs::MakeDisneyDiffuseBrdf;
using ::iris::bxdfs::MakeDisneyDiffuseRetroBrdf;
using ::iris::bxdfs::MakeDisneySheenBrdf;
using ::iris::bxdfs::MakeDisneySpecularBrdf;
using ::iris::bxdfs::MakeDisneySpecularBtdf;
using ::iris::bxdfs::MakeDisneySubsurfaceBrdf;
using ::iris::bxdfs::MakeDisneyThinSpecularBtdf;
using ::iris::bxdfs::MakeLambertianBtdf;
using ::iris::textures::FloatTexture;
using ::iris::textures::ReflectorTexture;

class DisneyMaterial final : public Material {
 public:
  DisneyMaterial(ReferenceCounted<textures::ReflectorTexture> color,
                 ReferenceCounted<textures::FloatTexture> metallic,
                 ReferenceCounted<textures::FloatTexture> eta_front,
                 ReferenceCounted<textures::FloatTexture> eta_back,
                 ReferenceCounted<textures::FloatTexture> roughness,
                 ReferenceCounted<textures::FloatTexture> specular_tint,
                 ReferenceCounted<textures::FloatTexture> anisotropic,
                 ReferenceCounted<textures::FloatTexture> sheen,
                 ReferenceCounted<textures::FloatTexture> sheen_tint,
                 ReferenceCounted<textures::FloatTexture> clearcoat,
                 ReferenceCounted<textures::FloatTexture> clearcoat_gloss,
                 ReferenceCounted<textures::FloatTexture> specular_transmission,
                 ReferenceCounted<textures::FloatTexture> flatness,
                 ReferenceCounted<textures::FloatTexture> diffuse_transmission,
                 ReferenceCounted<textures::SpectrumTexture> scatter_distance,
                 bool thin)
      : color_(std::move(color)),
        metallic_(std::move(metallic)),
        eta_front_(std::move(eta_front)),
        eta_back_(std::move(eta_back)),
        roughness_(std::move(roughness)),
        specular_tint_(std::move(specular_tint)),
        anisotropic_(std::move(anisotropic)),
        sheen_(std::move(sheen)),
        sheen_tint_(std::move(sheen_tint)),
        clearcoat_(std::move(clearcoat)),
        clearcoat_gloss_(std::move(clearcoat_gloss)),
        specular_transmission_(std::move(specular_transmission)),
        flatness_(std::move(flatness)),
        diffuse_transmission_(std::move(diffuse_transmission)),
        scatter_distance_(std::move(scatter_distance)),
        thin_(thin) {}

  const Bxdf* Evaluate(const TextureCoordinates& texture_coordinates,
                       SpectralAllocator& spectral_allocator,
                       BxdfAllocator& bxdf_allocator) const override;

 private:
  ReferenceCounted<textures::ReflectorTexture> color_;
  ReferenceCounted<textures::FloatTexture> metallic_;
  ReferenceCounted<textures::FloatTexture> eta_front_;
  ReferenceCounted<textures::FloatTexture> eta_back_;
  ReferenceCounted<textures::FloatTexture> roughness_;
  ReferenceCounted<textures::FloatTexture> specular_tint_;
  ReferenceCounted<textures::FloatTexture> anisotropic_;
  ReferenceCounted<textures::FloatTexture> sheen_;
  ReferenceCounted<textures::FloatTexture> sheen_tint_;
  ReferenceCounted<textures::FloatTexture> clearcoat_;
  ReferenceCounted<textures::FloatTexture> clearcoat_gloss_;
  ReferenceCounted<textures::FloatTexture> specular_transmission_;
  ReferenceCounted<textures::FloatTexture> flatness_;
  ReferenceCounted<textures::FloatTexture> diffuse_transmission_;
  ReferenceCounted<textures::SpectrumTexture> scatter_distance_;
  bool thin_;
};

const Bxdf* DisneyMaterial::Evaluate(
    const TextureCoordinates& texture_coordinates,
    SpectralAllocator& spectral_allocator,
    BxdfAllocator& bxdf_allocator) const {
  const Reflector* color = nullptr;
  if (color_) {
    color = color_->Evaluate(texture_coordinates, spectral_allocator);
  }

  visual_t metallic = static_cast<visual_t>(0.0);
  visual_t non_metallic = static_cast<visual_t>(1.0);
  if (!thin_ && metallic_) {
    metallic = metallic_->Evaluate(texture_coordinates);
    non_metallic -= metallic;
  }

  visual_t specular = static_cast<visual_t>(0.0);
  visual_t diffuse = non_metallic;
  if (specular_transmission_) {
    specular = specular_transmission_->Evaluate(texture_coordinates);
    diffuse *= static_cast<visual_t>(1.0) - specular;
    specular *= non_metallic;
  }

  //
  // Clearcoat BRDF
  //

  visual_t clearcoat = static_cast<visual_t>(0.0);
  if (!thin_ && clearcoat_) {
    clearcoat = clearcoat_->Evaluate(texture_coordinates);
  }

  visual_t clearcoat_gloss = static_cast<visual_t>(0.0);
  if (!thin_ && clearcoat_gloss_) {
    clearcoat_gloss = clearcoat_gloss_->Evaluate(texture_coordinates);
  }

  const Bxdf* clearcoat_brdf =
      MakeDisneyClearcoatBrdf(bxdf_allocator, clearcoat, clearcoat_gloss);

  //
  // Specular BRDF
  //

  visual_t specular_tint = static_cast<visual_t>(0.0);
  if (specular_tint_) {
    specular_tint = specular_tint_->Evaluate(texture_coordinates);
  }

  visual_t eta_front = static_cast<visual_t>(0.0);
  if (eta_front_) {
    eta_front = eta_front_->Evaluate(texture_coordinates);
  }

  visual_t eta_back = static_cast<visual_t>(0.0);
  if (eta_back_) {
    eta_back = eta_back_->Evaluate(texture_coordinates);
  }

  visual_t anisotropic = static_cast<visual_t>(0.0);
  if (anisotropic_) {
    anisotropic = anisotropic_->Evaluate(texture_coordinates);
  }

  visual_t roughness = static_cast<visual_t>(0.0);
  if (roughness_) {
    roughness = roughness_->Evaluate(texture_coordinates);
  }

  const Bxdf* specular_brdf =
      MakeDisneySpecularBrdf(bxdf_allocator, color, metallic, specular_tint,
                             eta_front, eta_back, anisotropic, roughness);

  //
  // Specular BTDF
  //

  const Bxdf* specular_btdf = nullptr;
  if (thin_) {
    specular_btdf = MakeDisneyThinSpecularBtdf(
        bxdf_allocator, spectral_allocator.Scale(color, specular), eta_front,
        eta_back, anisotropic, roughness);
  } else {
    specular_btdf = MakeDisneySpecularBtdf(
        bxdf_allocator,
        spectral_allocator.Scale(spectral_allocator.Sqrt(color), specular),
        eta_front, eta_back, anisotropic, roughness);
  }

  //
  // Diffuse Sheen BRDF
  //

  visual_t sheen = static_cast<visual_t>(0.0);
  if (sheen_) {
    sheen = thin_ ? diffuse : non_metallic;
    sheen *= sheen_->Evaluate(texture_coordinates);
  }

  visual_t sheen_tint = static_cast<visual_t>(0.0);
  if (sheen_tint_) {
    sheen_tint = sheen_tint_->Evaluate(texture_coordinates);
  }

  const Bxdf* sheen_brdf =
      MakeDisneySheenBrdf(bxdf_allocator, color, sheen, sheen_tint);

  //
  // Diffuse Retroreflective BRDF
  //

  visual_t flat = static_cast<visual_t>(0.0);
  visual_t nonflat = static_cast<visual_t>(1.0);
  if (flatness_) {
    flat = flatness_->Evaluate(texture_coordinates);
    nonflat = static_cast<visual_t>(1.0) - flat;
  }

  const Bxdf* retro_brdf = MakeDisneyDiffuseRetroBrdf(
      bxdf_allocator, spectral_allocator.Scale(color, diffuse * nonflat),
      roughness);

  //
  // Diffuse BRDF
  //

  visual_t diffuse_transmission = static_cast<visual_t>(0.0);
  visual_t diffuse_reflection = diffuse;
  if (thin_ && diffuse_transmission_) {
    diffuse_transmission =
        diffuse_transmission_->Evaluate(texture_coordinates) *
        static_cast<visual_t>(0.5);
    diffuse_reflection *= static_cast<visual_t>(1.0) - diffuse_transmission;
    diffuse_transmission *= diffuse;
  }

  const Spectrum* scatter_distance = nullptr;
  if (!thin_ && scatter_distance_) {
    scatter_distance =
        scatter_distance_->Eval(texture_coordinates, spectral_allocator);
  }

  const Bxdf* diffuse_brdf = nullptr;
  if (scatter_distance) {
    // TODO
  } else {
    diffuse_brdf = MakeDisneyDiffuseBrdf(
        bxdf_allocator,
        spectral_allocator.Scale(color, diffuse_reflection * nonflat));
  }

  const Bxdf* subsurface_brdf = MakeDisneySubsurfaceBrdf(
      bxdf_allocator,
      spectral_allocator.Scale(color, diffuse_reflection * flat), roughness);

  //
  // Diffuse BTDF
  //

  const Bxdf* diffuse_btdf = MakeLambertianBtdf(
      bxdf_allocator, spectral_allocator.Scale(color, diffuse_transmission));

  //
  // Assemble Result
  //

  return MakeCompositeBxdf(bxdf_allocator, clearcoat_brdf, specular_brdf,
                           specular_btdf, sheen_brdf, retro_brdf, diffuse_brdf,
                           subsurface_brdf, diffuse_btdf);
}

}  // namespace

ReferenceCounted<Material> MakeDisneyMaterial(
    ReferenceCounted<textures::ReflectorTexture> color,
    ReferenceCounted<textures::FloatTexture> metallic,
    ReferenceCounted<textures::FloatTexture> eta_front,
    ReferenceCounted<textures::FloatTexture> eta_back,
    ReferenceCounted<textures::FloatTexture> roughness,
    ReferenceCounted<textures::FloatTexture> specular_tint,
    ReferenceCounted<textures::FloatTexture> anisotropic,
    ReferenceCounted<textures::FloatTexture> sheen,
    ReferenceCounted<textures::FloatTexture> sheen_tint,
    ReferenceCounted<textures::FloatTexture> clearcoat,
    ReferenceCounted<textures::FloatTexture> clearcoat_gloss,
    ReferenceCounted<textures::FloatTexture> specular_transmission,
    ReferenceCounted<textures::FloatTexture> flatness,
    ReferenceCounted<textures::FloatTexture> diffuse_transmission,
    ReferenceCounted<textures::SpectrumTexture> scatter_distance, bool thin) {
  if (!eta_front || !eta_back) {
    return ReferenceCounted<Material>();
  }

  return MakeReferenceCounted<DisneyMaterial>(
      std::move(color), std::move(metallic), std::move(eta_front),
      std::move(eta_back), std::move(roughness), std::move(specular_tint),
      std::move(anisotropic), std::move(sheen), std::move(sheen_tint),
      std::move(clearcoat), std::move(clearcoat_gloss),
      std::move(specular_transmission), std::move(flatness),
      std::move(diffuse_transmission), std::move(scatter_distance), thin);
}

}  // namespace materials
}  // namespace iris
