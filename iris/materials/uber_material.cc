#include "iris/materials/uber_material.h"

#include <algorithm>
#include <utility>

#include "iris/bxdf.h"
#include "iris/bxdf_allocator.h"
#include "iris/bxdfs/composite_bxdf.h"
#include "iris/bxdfs/lambertian_bxdf.h"
#include "iris/bxdfs/microfacet_bxdf.h"
#include "iris/bxdfs/specular_dielectric_bxdf.h"
#include "iris/bxdfs/transparent_btdf.h"
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
using ::iris::bxdfs::MakeSpecularDielectricBxdf;
using ::iris::bxdfs::MakeTransparentBtdf;
using ::iris::textures::FloatTexture;
using ::iris::textures::ReflectorTexture;

class UberMaterial final : public Material {
 public:
  UberMaterial(ReferenceCounted<ReflectorTexture> reflectance,
               ReferenceCounted<ReflectorTexture> transmittance,
               ReferenceCounted<ReflectorTexture> diffuse,
               ReferenceCounted<ReflectorTexture> specular,
               ReferenceCounted<ReflectorTexture> opacity,
               ReferenceCounted<FloatTexture> eta_incident,
               ReferenceCounted<FloatTexture> eta_transmitted,
               ReferenceCounted<FloatTexture> roughness_u,
               ReferenceCounted<FloatTexture> roughness_v, bool remap_roughness)
      : reflectance_(std::move(reflectance)),
        transmittance_(std::move(transmittance)),
        diffuse_(std::move(diffuse)),
        specular_(std::move(specular)),
        opacity_(std::move(opacity)),
        eta_incident_(std::move(eta_incident)),
        eta_transmitted_(std::move(eta_transmitted)),
        roughness_u_(std::move(roughness_u)),
        roughness_v_(std::move(roughness_v)),
        remap_roughness_(remap_roughness) {}

  const Bxdf* Evaluate(const TextureCoordinates& texture_coordinates,
                       SpectralAllocator& spectral_allocator,
                       BxdfAllocator& bxdf_allocator) const override;

 private:
  ReferenceCounted<ReflectorTexture> reflectance_;
  ReferenceCounted<ReflectorTexture> transmittance_;
  ReferenceCounted<ReflectorTexture> diffuse_;
  ReferenceCounted<ReflectorTexture> specular_;
  ReferenceCounted<ReflectorTexture> opacity_;
  ReferenceCounted<FloatTexture> eta_incident_;
  ReferenceCounted<FloatTexture> eta_transmitted_;
  ReferenceCounted<FloatTexture> roughness_u_;
  ReferenceCounted<FloatTexture> roughness_v_;
  bool remap_roughness_;
};

const Bxdf* UberMaterial::Evaluate(
    const TextureCoordinates& texture_coordinates,
    SpectralAllocator& spectral_allocator,
    BxdfAllocator& bxdf_allocator) const {
  const Reflector* opacity = nullptr;
  const Bxdf* transparent_btdf = nullptr;
  if (opacity_) {
    opacity = opacity_->Evaluate(texture_coordinates, spectral_allocator);
  }

  transparent_btdf =
      MakeTransparentBtdf(bxdf_allocator, spectral_allocator.Invert(opacity));

  const Bxdf* lambertian_brdf = nullptr;
  if (diffuse_) {
    lambertian_brdf = MakeLambertianBrdf(
        bxdf_allocator,
        spectral_allocator.Scale(
            diffuse_->Evaluate(texture_coordinates, spectral_allocator),
            opacity));
  }

  visual eta_incident = static_cast<visual>(0.0);
  if (eta_incident_) {
    eta_incident = eta_incident_->Evaluate(texture_coordinates);
  }

  visual eta_transmitted = static_cast<visual>(0.0);
  if (eta_transmitted_) {
    eta_transmitted = eta_transmitted_->Evaluate(texture_coordinates);
  }

  const Bxdf* microfacet_brdf = nullptr;
  if (specular_) {
    visual roughness_u = static_cast<visual>(0.0);
    if (roughness_u_) {
      roughness_u = roughness_u_->Evaluate(texture_coordinates);
    }

    visual roughness_v = static_cast<visual>(0.0);
    if (roughness_v_) {
      roughness_v = roughness_v_->Evaluate(texture_coordinates);
    }

    microfacet_brdf = MakeMicrofacetDielectricBrdf(
        bxdf_allocator,
        spectral_allocator.Scale(
            specular_->Evaluate(texture_coordinates, spectral_allocator),
            opacity),
        eta_incident, eta_transmitted, roughness_u, roughness_v,
        remap_roughness_);
  }

  const Bxdf* specular_bxdf = nullptr;
  if (reflectance_ || transmittance_) {
    const Reflector* reflectance = nullptr;
    if (reflectance_) {
      reflectance = spectral_allocator.Scale(
          reflectance_->Evaluate(texture_coordinates, spectral_allocator),
          opacity);
    }

    const Reflector* transmittance = nullptr;
    if (transmittance_) {
      transmittance = spectral_allocator.Scale(
          transmittance_->Evaluate(texture_coordinates, spectral_allocator),
          opacity);
    }

    specular_bxdf =
        MakeSpecularDielectricBxdf(bxdf_allocator, reflectance, transmittance,
                                   eta_incident, eta_transmitted);
  }

  return MakeCompositeBxdf(bxdf_allocator, transparent_btdf, lambertian_brdf,
                           microfacet_brdf, specular_bxdf);
}

}  // namespace

ReferenceCounted<Material> MakeUberMaterial(
    ReferenceCounted<ReflectorTexture> reflectance,
    ReferenceCounted<ReflectorTexture> transmittance,
    ReferenceCounted<ReflectorTexture> diffuse,
    ReferenceCounted<ReflectorTexture> specular,
    ReferenceCounted<ReflectorTexture> opacity,
    ReferenceCounted<FloatTexture> eta_incident,
    ReferenceCounted<FloatTexture> eta_transmitted,
    ReferenceCounted<FloatTexture> roughness_u,
    ReferenceCounted<FloatTexture> roughness_v, bool remap_roughness) {
  if (!eta_incident || !eta_transmitted) {
    eta_incident.Reset();
    eta_transmitted.Reset();
    specular.Reset();
    reflectance.Reset();
    transmittance.Reset();
  }

  return MakeReferenceCounted<UberMaterial>(
      std::move(reflectance), std::move(transmittance), std::move(diffuse),
      std::move(specular), std::move(opacity), std::move(eta_incident),
      std::move(eta_transmitted), std::move(roughness_u),
      std::move(roughness_v), remap_roughness);
}

}  // namespace materials
}  // namespace iris
