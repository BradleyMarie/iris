#include "iris/materials/plastic_material.h"

#include <utility>

#include "iris/bxdf.h"
#include "iris/bxdf_allocator.h"
#include "iris/bxdfs/composite_bxdf.h"
#include "iris/bxdfs/lambertian_bxdf.h"
#include "iris/bxdfs/microfacet_bxdf.h"
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

using ::iris::bxdfs::MakeCompositeBxdf;
using ::iris::bxdfs::MakeLambertianBrdf;
using ::iris::bxdfs::MakeMicrofacetDielectricBrdf;
using ::iris::textures::FloatTexture;
using ::iris::textures::ReflectorTexture;

class PlasticMaterial final : public Material {
 public:
  PlasticMaterial(ReferenceCounted<ReflectorTexture> diffuse,
                  ReferenceCounted<ReflectorTexture> specular,
                  ReferenceCounted<FloatTexture> eta_incident,
                  ReferenceCounted<FloatTexture> eta_transmitted,
                  ReferenceCounted<FloatTexture> roughness,
                  bool remap_roughness)
      : diffuse_(std::move(diffuse)),
        specular_(std::move(specular)),
        eta_incident_(std::move(eta_incident)),
        eta_transmitted_(std::move(eta_transmitted)),
        roughness_(std::move(roughness)),
        remap_roughness_(remap_roughness) {}

  const Bxdf* Evaluate(const TextureCoordinates& texture_coordinates,
                       SpectralAllocator& spectral_allocator,
                       BxdfAllocator& bxdf_allocator) const override;

 private:
  ReferenceCounted<ReflectorTexture> diffuse_;
  ReferenceCounted<ReflectorTexture> specular_;
  ReferenceCounted<FloatTexture> eta_incident_;
  ReferenceCounted<FloatTexture> eta_transmitted_;
  ReferenceCounted<FloatTexture> roughness_;
  bool remap_roughness_;
};

const Bxdf* PlasticMaterial::Evaluate(
    const TextureCoordinates& texture_coordinates,
    SpectralAllocator& spectral_allocator,
    BxdfAllocator& bxdf_allocator) const {
  const Bxdf* lambertian_brdf = nullptr;
  if (diffuse_) {
    lambertian_brdf = MakeLambertianBrdf(
        bxdf_allocator,
        diffuse_->Evaluate(texture_coordinates, spectral_allocator));
  }

  const Bxdf* microfacet_brdf = nullptr;
  if (specular_) {
    visual eta_incident = static_cast<visual>(0.0);
    if (eta_incident_) {
      eta_incident = eta_incident_->Evaluate(texture_coordinates);
    }

    visual eta_transmitted = static_cast<visual>(0.0);
    if (eta_transmitted_) {
      eta_transmitted = eta_transmitted_->Evaluate(texture_coordinates);
    }

    visual roughness = static_cast<visual>(0.0);
    if (roughness_) {
      roughness = roughness_->Evaluate(texture_coordinates);
    }

    microfacet_brdf = MakeMicrofacetDielectricBrdf(
        bxdf_allocator,
        specular_->Evaluate(texture_coordinates, spectral_allocator),
        eta_incident, eta_transmitted, roughness, roughness, remap_roughness_);
  }

  return MakeCompositeBxdf(bxdf_allocator, lambertian_brdf, microfacet_brdf);
}

}  // namespace

ReferenceCounted<Material> MakePlasticMaterial(
    ReferenceCounted<ReflectorTexture> diffuse,
    ReferenceCounted<ReflectorTexture> specular,
    ReferenceCounted<FloatTexture> eta_incident,
    ReferenceCounted<FloatTexture> eta_transmitted,
    ReferenceCounted<FloatTexture> roughness, bool remap_roughness) {
  if (!eta_incident || !eta_transmitted) {
    eta_incident.Reset();
    eta_transmitted.Reset();
    specular.Reset();
  }

  if (!diffuse && !specular) {
    return ReferenceCounted<Material>();
  }

  return MakeReferenceCounted<PlasticMaterial>(
      std::move(diffuse), std::move(specular), std::move(eta_incident),
      std::move(eta_transmitted), std::move(roughness), remap_roughness);
}

}  // namespace materials
}  // namespace iris
