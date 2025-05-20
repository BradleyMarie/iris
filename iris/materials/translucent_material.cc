#include "iris/materials/translucent_material.h"

#include <cassert>

#include "iris/bxdf.h"
#include "iris/bxdf_allocator.h"
#include "iris/bxdfs/composite_bxdf.h"
#include "iris/bxdfs/lambertian_bxdf.h"
#include "iris/bxdfs/microfacet_bxdf.h"
#include "iris/bxdfs/microfacet_distributions/trowbridge_reitz_distribution.h"
#include "iris/float.h"
#include "iris/material.h"
#include "iris/reference_counted.h"
#include "iris/spectral_allocator.h"
#include "iris/texture_coordinates.h"
#include "iris/textures/texture2d.h"

namespace iris {
namespace materials {
namespace {

using ::iris::bxdfs::FresnelDielectric;
using ::iris::bxdfs::MakeCompositeBxdf;
using ::iris::bxdfs::MakeLambertianBrdf;
using ::iris::bxdfs::MakeLambertianBtdf;
using ::iris::bxdfs::MakeMicrofacetBrdf;
using ::iris::bxdfs::MakeMicrofacetBtdf;
using ::iris::bxdfs::microfacet_distributions::TrowbridgeReitzDistribution;

class TranslucentMaterial final : public Material {
 public:
  TranslucentMaterial(
      ReferenceCounted<textures::PointerTexture2D<Reflector, SpectralAllocator>>
          reflectance,
      ReferenceCounted<textures::PointerTexture2D<Reflector, SpectralAllocator>>
          transmittance,
      ReferenceCounted<textures::PointerTexture2D<Reflector, SpectralAllocator>>
          diffuse,
      ReferenceCounted<textures::PointerTexture2D<Reflector, SpectralAllocator>>
          specular,
      ReferenceCounted<textures::ValueTexture2D<visual>> eta_incident,
      ReferenceCounted<textures::ValueTexture2D<visual>> eta_transmitted,
      ReferenceCounted<textures::ValueTexture2D<visual>> roughness,
      bool remap_roughness)
      : reflectance_(std::move(reflectance)),
        transmittance_(std::move(transmittance)),
        diffuse_(std::move(diffuse)),
        specular_(std::move(specular)),
        eta_incident_(std::move(eta_incident)),
        eta_transmitted_(std::move(eta_transmitted)),
        roughness_(std::move(roughness)),
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
  ReferenceCounted<textures::ValueTexture2D<visual>> eta_incident_;
  ReferenceCounted<textures::ValueTexture2D<visual>> eta_transmitted_;
  ReferenceCounted<textures::ValueTexture2D<visual>> roughness_;
  bool remap_roughness_;
};

const Bxdf* TranslucentMaterial::Evaluate(
    const TextureCoordinates& texture_coordinates,
    SpectralAllocator& spectral_allocator,
    BxdfAllocator& bxdf_allocator) const {
  const Reflector* reflectance = nullptr;
  if (reflectance_) {
    reflectance =
        reflectance_->Evaluate(texture_coordinates, spectral_allocator);
  }

  const Reflector* transmittance = nullptr;
  if (transmittance_) {
    transmittance =
        transmittance_->Evaluate(texture_coordinates, spectral_allocator);
  }

  const Bxdf* lambertian_brdf = nullptr;
  const Bxdf* lambertian_btdf = nullptr;
  if (diffuse_) {
    const Reflector* diffuse =
        diffuse_->Evaluate(texture_coordinates, spectral_allocator);
    lambertian_brdf = MakeLambertianBrdf(
        bxdf_allocator, spectral_allocator.Scale(reflectance, diffuse));
    lambertian_btdf = MakeLambertianBtdf(
        bxdf_allocator, spectral_allocator.Scale(transmittance, diffuse));
  }

  const Bxdf* microfacet_brdf = nullptr;
  const Bxdf* microfacet_btdf = nullptr;
  if (specular_) {
    const Reflector* specular =
        specular_->Evaluate(texture_coordinates, spectral_allocator);

    // Consider clamping these values
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

    if (remap_roughness_) {
      roughness = TrowbridgeReitzDistribution::RoughnessToAlpha(roughness);
    }

    microfacet_brdf = MakeMicrofacetBrdf(
        bxdf_allocator, spectral_allocator.Scale(reflectance, specular),
        TrowbridgeReitzDistribution(roughness, roughness),
        FresnelDielectric(eta_incident, eta_transmitted));

    microfacet_btdf = MakeMicrofacetBtdf(
        bxdf_allocator, spectral_allocator.Scale(transmittance, specular),
        eta_incident, eta_transmitted,
        TrowbridgeReitzDistribution(roughness, roughness),
        FresnelDielectric(eta_incident, eta_transmitted));
  }

  return MakeCompositeBxdf(bxdf_allocator, lambertian_brdf, lambertian_btdf,
                           microfacet_brdf, microfacet_btdf);
}

}  // namespace

ReferenceCounted<Material> MakeTranslucentMaterial(
    ReferenceCounted<textures::PointerTexture2D<Reflector, SpectralAllocator>>
        reflectance,
    ReferenceCounted<textures::PointerTexture2D<Reflector, SpectralAllocator>>
        transmittance,
    ReferenceCounted<textures::PointerTexture2D<Reflector, SpectralAllocator>>
        diffuse,
    ReferenceCounted<textures::PointerTexture2D<Reflector, SpectralAllocator>>
        specular,
    ReferenceCounted<textures::ValueTexture2D<visual>> eta_incident,
    ReferenceCounted<textures::ValueTexture2D<visual>> eta_transmitted,
    ReferenceCounted<textures::ValueTexture2D<visual>> roughness,
    bool remap_roughness) {
  if (!reflectance && !transmittance) {
    return ReferenceCounted<Material>();
  }

  if (!diffuse && !specular) {
    return ReferenceCounted<Material>();
  }

  return MakeReferenceCounted<TranslucentMaterial>(
      std::move(reflectance), std::move(transmittance), std::move(diffuse),
      std::move(specular), std::move(eta_incident), std::move(eta_transmitted),
      std::move(roughness), remap_roughness);
}

}  // namespace materials
}  // namespace iris
