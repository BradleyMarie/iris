#include "iris/materials/uber_material.h"

#include <algorithm>
#include <cassert>

#include "iris/bxdf.h"
#include "iris/bxdf_allocator.h"
#include "iris/bxdfs/composite_bxdf.h"
#include "iris/bxdfs/lambertian_bxdf.h"
#include "iris/bxdfs/microfacet_bxdf.h"
#include "iris/bxdfs/microfacet_distributions/trowbridge_reitz_distribution.h"
#include "iris/bxdfs/specular_bxdf.h"
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
using ::iris::bxdfs::FresnelNoOp;
using ::iris::bxdfs::MakeCompositeBxdf;
using ::iris::bxdfs::MakeLambertianBrdf;
using ::iris::bxdfs::MakeSpecularBrdf;
using ::iris::bxdfs::MakeSpecularBtdf;
using ::iris::bxdfs::MicrofacetBrdf;
using ::iris::bxdfs::microfacet_distributions::TrowbridgeReitzDistribution;
using ::iris::reflectors::CreateUniformReflector;

static const ReferenceCounted<Reflector> kWhite =
    CreateUniformReflector(static_cast<visual>(1.0));

class UberMaterial final : public Material {
 public:
  UberMaterial(
      ReferenceCounted<textures::PointerTexture2D<Reflector, SpectralAllocator>>
          reflectance,
      ReferenceCounted<textures::PointerTexture2D<Reflector, SpectralAllocator>>
          transmittance,
      ReferenceCounted<textures::PointerTexture2D<Reflector, SpectralAllocator>>
          diffuse,
      ReferenceCounted<textures::PointerTexture2D<Reflector, SpectralAllocator>>
          specular,
      ReferenceCounted<textures::ValueTexture2D<visual>> opacity,
      ReferenceCounted<textures::ValueTexture2D<visual>> eta_incident,
      ReferenceCounted<textures::ValueTexture2D<visual>> eta_transmitted,
      ReferenceCounted<textures::ValueTexture2D<visual>> roughness_u,
      ReferenceCounted<textures::ValueTexture2D<visual>> roughness_v,
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
        remap_roughness_(remap_roughness) {
    assert(eta_incident_);
    assert(eta_transmitted_);
  }

  const Bxdf* Evaluate(const TextureCoordinates& texture_coordinates,
                       SpectralAllocator& spectral_allocator,
                       BxdfAllocator& bxdf_allocator) const override;

 private:
  ReferenceCounted<textures::PointerTexture2D<Reflector, SpectralAllocator>>
      reflectance_;
  ReferenceCounted<textures::PointerTexture2D<Reflector, SpectralAllocator>>
      transmittance_;
  ReferenceCounted<textures::PointerTexture2D<Reflector, SpectralAllocator>>
      diffuse_;
  ReferenceCounted<textures::PointerTexture2D<Reflector, SpectralAllocator>>
      specular_;
  ReferenceCounted<textures::ValueTexture2D<visual>> opacity_;
  ReferenceCounted<textures::ValueTexture2D<visual>> eta_incident_;
  ReferenceCounted<textures::ValueTexture2D<visual>> eta_transmitted_;
  ReferenceCounted<textures::ValueTexture2D<visual>> roughness_u_;
  ReferenceCounted<textures::ValueTexture2D<visual>> roughness_v_;
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

  visual transparency = static_cast<visual>(1.0) - opacity;

  const Bxdf* transparent_btdf = nullptr;
  if (transparency > static_cast<visual>(0.0)) {
    transparent_btdf = MakeSpecularBtdf(
        bxdf_allocator, spectral_allocator.Scale(kWhite.Get(), transparency),
        static_cast<geometric_t>(1.0), static_cast<geometric_t>(1.0),
        FresnelNoOp());
  }

  const Bxdf* lambertian_brdf = nullptr;
  const Bxdf* microfacet_brdf = nullptr;
  const Bxdf* specular_brdf = nullptr;
  const Bxdf* specular_btdf = nullptr;
  if (opacity > static_cast<visual>(0.0)) {
    visual eta_incident = static_cast<visual>(0.0);
    if (eta_incident_) {
      eta_incident = eta_incident_->Evaluate(texture_coordinates);
    }

    visual eta_transmitted = static_cast<visual>(0.0);
    if (eta_transmitted_) {
      eta_transmitted = eta_transmitted_->Evaluate(texture_coordinates);
    }

    if (diffuse_) {
      lambertian_brdf = MakeLambertianBrdf(
          bxdf_allocator,
          spectral_allocator.Scale(
              diffuse_->Evaluate(texture_coordinates, spectral_allocator),
              opacity));
    }

    if (specular_) {
      const Reflector* specular =
          specular_->Evaluate(texture_coordinates, spectral_allocator);

      if (specular != nullptr) {
        visual roughness_u = static_cast<visual>(0.0);
        if (roughness_u_) {
          roughness_u = roughness_u_->Evaluate(texture_coordinates);

          if (remap_roughness_) {
            roughness_u =
                TrowbridgeReitzDistribution::RoughnessToAlpha(roughness_u);
          }
        }

        visual roughness_v = static_cast<visual>(0.0);
        if (roughness_v_) {
          roughness_v = roughness_v_->Evaluate(texture_coordinates);

          if (remap_roughness_) {
            roughness_v =
                TrowbridgeReitzDistribution::RoughnessToAlpha(roughness_v);
          }
        }

        if (remap_roughness_) {
          roughness_u =
              TrowbridgeReitzDistribution::RoughnessToAlpha(roughness_u);
          roughness_v =
              TrowbridgeReitzDistribution::RoughnessToAlpha(roughness_v);
        }

        microfacet_brdf = &bxdf_allocator.Allocate<
            MicrofacetBrdf<TrowbridgeReitzDistribution, FresnelDielectric>>(
            *spectral_allocator.Scale(specular, opacity),
            TrowbridgeReitzDistribution(roughness_u, roughness_v),
            FresnelDielectric(eta_incident, eta_transmitted));
      }
    }

    if (reflectance_) {
      specular_brdf = MakeSpecularBrdf(
          bxdf_allocator,
          spectral_allocator.Scale(
              reflectance_->Evaluate(texture_coordinates, spectral_allocator),
              opacity),
          FresnelDielectric(eta_incident, eta_transmitted));
    }

    if (transmittance_) {
      specular_btdf = MakeSpecularBtdf(
          bxdf_allocator,
          spectral_allocator.Scale(
              transmittance_->Evaluate(texture_coordinates, spectral_allocator),
              opacity),
          eta_incident, eta_transmitted,
          FresnelDielectric(eta_incident, eta_transmitted));
    }
  }

  return MakeCompositeBxdf(bxdf_allocator, transparent_btdf, lambertian_brdf,
                           microfacet_brdf, specular_brdf, specular_btdf);
}

}  // namespace

ReferenceCounted<Material> MakeUberMaterial(
    ReferenceCounted<textures::PointerTexture2D<Reflector, SpectralAllocator>>
        reflectance,
    ReferenceCounted<textures::PointerTexture2D<Reflector, SpectralAllocator>>
        transmittance,
    ReferenceCounted<textures::PointerTexture2D<Reflector, SpectralAllocator>>
        diffuse,
    ReferenceCounted<textures::PointerTexture2D<Reflector, SpectralAllocator>>
        specular,
    ReferenceCounted<textures::ValueTexture2D<visual>> opacity,
    ReferenceCounted<textures::ValueTexture2D<visual>> eta_incident,
    ReferenceCounted<textures::ValueTexture2D<visual>> eta_transmitted,
    ReferenceCounted<textures::ValueTexture2D<visual>> roughness_u,
    ReferenceCounted<textures::ValueTexture2D<visual>> roughness_v,
    bool remap_roughness) {
  if (!reflectance && !transmittance) {
    return ReferenceCounted<Material>();
  }

  if (!diffuse && !specular) {
    return ReferenceCounted<Material>();
  }

  return MakeReferenceCounted<UberMaterial>(
      std::move(reflectance), std::move(transmittance), std::move(diffuse),
      std::move(specular), std::move(opacity), std::move(eta_incident),
      std::move(eta_transmitted), std::move(roughness_u),
      std::move(roughness_v), remap_roughness);
}

}  // namespace materials
}  // namespace iris
