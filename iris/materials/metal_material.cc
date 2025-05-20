#include "iris/materials/metal_material.h"

#include <cassert>

#include "iris/bxdf.h"
#include "iris/bxdf_allocator.h"
#include "iris/bxdfs/microfacet_bxdf.h"
#include "iris/bxdfs/microfacet_distributions/trowbridge_reitz_distribution.h"
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

using ::iris::bxdfs::FresnelConductor;
using ::iris::bxdfs::MakeMicrofacetBrdf;
using ::iris::bxdfs::microfacet_distributions::TrowbridgeReitzDistribution;
using ::iris::reflectors::CreateUniformReflector;

static const ReferenceCounted<Reflector> kWhite =
    CreateUniformReflector(static_cast<visual>(1.0));

class MetalMaterial final : public Material {
 public:
  MetalMaterial(ReferenceCounted<Spectrum> k,
                ReferenceCounted<Spectrum> eta_incident,
                ReferenceCounted<Spectrum> eta_transmitted,
                ReferenceCounted<textures::ValueTexture2D<visual>> roughness_u,
                ReferenceCounted<textures::ValueTexture2D<visual>> roughness_v,
                bool remap_roughness)
      : k_(std::move(k)),
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
  ReferenceCounted<Spectrum> k_;
  ReferenceCounted<Spectrum> eta_incident_;
  ReferenceCounted<Spectrum> eta_transmitted_;
  ReferenceCounted<textures::ValueTexture2D<visual>> roughness_u_;
  ReferenceCounted<textures::ValueTexture2D<visual>> roughness_v_;
  bool remap_roughness_;
};

const Bxdf* MetalMaterial::Evaluate(
    const TextureCoordinates& texture_coordinates,
    SpectralAllocator& spectral_allocator,
    BxdfAllocator& bxdf_allocator) const {
  visual roughness_u = static_cast<visual>(0.0);
  if (roughness_u_) {
    roughness_u_->Evaluate(texture_coordinates);

    if (remap_roughness_) {
      roughness_u = TrowbridgeReitzDistribution::RoughnessToAlpha(roughness_u);
    }
  }

  visual roughness_v = static_cast<visual>(0.0);
  if (roughness_v_) {
    roughness_v_->Evaluate(texture_coordinates);

    if (remap_roughness_) {
      roughness_v = TrowbridgeReitzDistribution::RoughnessToAlpha(roughness_v);
    }
  }

  return MakeMicrofacetBrdf(
      bxdf_allocator, kWhite.Get(),
      TrowbridgeReitzDistribution(roughness_u, roughness_v),
      FresnelConductor(eta_incident_.Get(), eta_transmitted_.Get(), k_.Get()));
}

}  // namespace

ReferenceCounted<Material> MakeMetalMaterial(
    ReferenceCounted<Spectrum> k, ReferenceCounted<Spectrum> eta_incident,
    ReferenceCounted<Spectrum> eta_transmitted,
    ReferenceCounted<textures::ValueTexture2D<visual>> roughness_u,
    ReferenceCounted<textures::ValueTexture2D<visual>> roughness_v,
    bool remap_roughness) {
  if (!k) {
    return ReferenceCounted<Material>();
  }

  return MakeReferenceCounted<MetalMaterial>(
      std::move(k), std::move(eta_incident), std::move(eta_transmitted),
      std::move(roughness_u), std::move(roughness_v), remap_roughness);
}

}  // namespace materials
}  // namespace iris
