#include "iris/materials/uber_material.h"

#include <algorithm>
#include <utility>

#include "iris/bxdf.h"
#include "iris/bxdf_allocator.h"
#include "iris/bxdfs/composite_bxdf.h"
#include "iris/bxdfs/fresnel_dielectric_bxdf.h"
#include "iris/bxdfs/lambertian_bxdf.h"
#include "iris/bxdfs/microfacet_bxdf.h"
#include "iris/bxdfs/microfacet_distributions/trowbridge_reitz_distribution.h"
#include "iris/bxdfs/transparent_btdf.h"
#include "iris/float.h"
#include "iris/material.h"
#include "iris/reference_counted.h"
#include "iris/reflectors/uniform_reflector.h"
#include "iris/spectral_allocator.h"
#include "iris/texture_coordinates.h"
#include "iris/textures/texture2d.h"

namespace iris {
namespace materials {
namespace {

using ::iris::bxdfs::FresnelDielectric;
using ::iris::bxdfs::MakeCompositeBxdf;
using ::iris::bxdfs::MakeFresnelDielectricBxdf;
using ::iris::bxdfs::MakeLambertianBrdf;
using ::iris::bxdfs::MakeMicrofacetBrdf;
using ::iris::bxdfs::MakeTransparentBtdf;
using ::iris::bxdfs::microfacet_distributions::TrowbridgeReitzDistribution;
using ::iris::reflectors::CreateUniformReflector;
using ::iris::textures::PointerTexture2D;
using ::iris::textures::ValueTexture2D;

static const ReferenceCounted<Reflector> kWhite =
    CreateUniformReflector(static_cast<visual>(1.0));

class UberMaterial final : public Material {
 public:
  UberMaterial(
      ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
          reflectance,
      ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
          transmittance,
      ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> diffuse,
      ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> specular,
      ReferenceCounted<ValueTexture2D<visual>> opacity,
      ReferenceCounted<ValueTexture2D<visual>> eta_incident,
      ReferenceCounted<ValueTexture2D<visual>> eta_transmitted,
      ReferenceCounted<ValueTexture2D<visual>> roughness_u,
      ReferenceCounted<ValueTexture2D<visual>> roughness_v,
      bool remap_roughness)
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
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> reflectance_;
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
      transmittance_;
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> diffuse_;
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> specular_;
  ReferenceCounted<ValueTexture2D<visual>> opacity_;
  ReferenceCounted<ValueTexture2D<visual>> eta_incident_;
  ReferenceCounted<ValueTexture2D<visual>> eta_transmitted_;
  ReferenceCounted<ValueTexture2D<visual>> roughness_u_;
  ReferenceCounted<ValueTexture2D<visual>> roughness_v_;
  bool remap_roughness_;
};

const Bxdf* UberMaterial::Evaluate(
    const TextureCoordinates& texture_coordinates,
    SpectralAllocator& spectral_allocator,
    BxdfAllocator& bxdf_allocator) const {
  visual opacity = static_cast<visual>(0.0);
  if (opacity_) {
    opacity = std::clamp(opacity_->Evaluate(texture_coordinates),
                         static_cast<visual>(0.0), static_cast<visual>(1.0));
  }

  const Bxdf* transparent_btdf = nullptr;
  if (visual transparency = static_cast<visual>(1.0) - opacity;
      transparency > static_cast<visual>(0.0)) {
    transparent_btdf = MakeTransparentBtdf(
        bxdf_allocator, spectral_allocator.Scale(kWhite.Get(), transparency));
  }

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

    if (remap_roughness_) {
      roughness_u = TrowbridgeReitzDistribution::RoughnessToAlpha(roughness_u);
      roughness_v = TrowbridgeReitzDistribution::RoughnessToAlpha(roughness_v);
    }

    microfacet_brdf = MakeMicrofacetBrdf(
        bxdf_allocator,
        spectral_allocator.Scale(
            specular_->Evaluate(texture_coordinates, spectral_allocator),
            opacity),
        TrowbridgeReitzDistribution(roughness_u, roughness_v),
        FresnelDielectric(eta_incident, eta_transmitted));
  }

  const Bxdf* fresnel_bxdf = nullptr;
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

    fresnel_bxdf =
        MakeFresnelDielectricBxdf(bxdf_allocator, reflectance, transmittance,
                                  eta_incident, eta_transmitted);
  }

  return MakeCompositeBxdf(bxdf_allocator, transparent_btdf, lambertian_brdf,
                           microfacet_brdf, fresnel_bxdf);
}

}  // namespace

ReferenceCounted<Material> MakeUberMaterial(
    ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
        reflectance,
    ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
        transmittance,
    ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> diffuse,
    ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> specular,
    ReferenceCounted<ValueTexture2D<visual>> opacity,
    ReferenceCounted<ValueTexture2D<visual>> eta_incident,
    ReferenceCounted<ValueTexture2D<visual>> eta_transmitted,
    ReferenceCounted<ValueTexture2D<visual>> roughness_u,
    ReferenceCounted<ValueTexture2D<visual>> roughness_v,
    bool remap_roughness) {
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
