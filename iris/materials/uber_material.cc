#include "iris/materials/uber_material.h"

#include <algorithm>

#include "iris/bxdfs/composite_bxdf.h"
#include "iris/bxdfs/lambertian_bxdf.h"
#include "iris/bxdfs/microfacet_bxdf.h"
#include "iris/bxdfs/microfacet_distributions/trowbridge_reitz_distribution.h"
#include "iris/bxdfs/specular_bxdf.h"
#include "iris/reflectors/uniform_reflector.h"

namespace iris {
namespace materials {
namespace {

using ::iris::bxdfs::FresnelDielectric;
using ::iris::bxdfs::FresnelNoOp;
using ::iris::bxdfs::LambertianBrdf;
using ::iris::bxdfs::MicrofacetBrdf;
using ::iris::bxdfs::SpecularBrdf;
using ::iris::bxdfs::SpecularBtdf;
using ::iris::bxdfs::microfacet_distributions::TrowbridgeReitzDistribution;
using ::iris::reflectors::CreateUniformReflector;

static const ReferenceCounted<Reflector> kWhite =
    CreateUniformReflector(static_cast<visual>(1.0));

}  // namespace

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
    transparent_btdf = &bxdf_allocator.Allocate<SpecularBtdf<FresnelNoOp>>(
        *spectral_allocator.Scale(kWhite.Get(), transparency),
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
      const Reflector* diffuse =
          diffuse_->Evaluate(texture_coordinates, spectral_allocator);

      if (diffuse != nullptr) {
        lambertian_brdf = &bxdf_allocator.Allocate<LambertianBrdf>(
            *spectral_allocator.Scale(diffuse, opacity));
      }
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
      const Reflector* reflectance =
          reflectance_->Evaluate(texture_coordinates, spectral_allocator);

      if (reflectance != nullptr) {
        specular_brdf =
            &bxdf_allocator.Allocate<SpecularBrdf<FresnelDielectric>>(
                *spectral_allocator.Scale(reflectance, opacity),
                FresnelDielectric(eta_incident, eta_transmitted));
      }
    }

    if (transmittance_) {
      const Reflector* transmittance =
          transmittance_->Evaluate(texture_coordinates, spectral_allocator);

      if (transmittance != nullptr) {
        specular_btdf =
            &bxdf_allocator.Allocate<SpecularBtdf<FresnelDielectric>>(
                *spectral_allocator.Scale(transmittance, opacity), eta_incident,
                eta_transmitted,
                FresnelDielectric(eta_incident, eta_transmitted));
      }
    }
  }

  return bxdfs::MakeCompositeBxdf(bxdf_allocator, transparent_btdf,
                                  lambertian_brdf, microfacet_brdf,
                                  specular_brdf, specular_btdf);
}

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
