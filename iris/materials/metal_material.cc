#include "iris/materials/metal_material.h"

#include <utility>

#include "iris/bxdf.h"
#include "iris/bxdf_allocator.h"
#include "iris/bxdfs/microfacet_bxdf.h"
#include "iris/float.h"
#include "iris/material.h"
#include "iris/reference_counted.h"
#include "iris/reflectors/uniform_reflector.h"
#include "iris/spectral_allocator.h"
#include "iris/texture_coordinates.h"
#include "iris/textures/float_texture.h"

namespace iris {
namespace materials {
namespace {

using ::iris::bxdfs::MakeMicrofacetConductorBrdf;
using ::iris::reflectors::CreateUniformReflector;
using ::iris::textures::FloatTexture;

static const ReferenceCounted<Reflector> kWhite =
    CreateUniformReflector(static_cast<visual>(1.0));

class MetalMaterial final : public Material {
 public:
  MetalMaterial(ReferenceCounted<FloatTexture> eta_dielectric,
                ReferenceCounted<Spectrum> eta_conductor,
                ReferenceCounted<Spectrum> k_conductor,
                ReferenceCounted<FloatTexture> roughness_u,
                ReferenceCounted<FloatTexture> roughness_v,
                bool remap_roughness)
      : eta_dielectric_(std::move(eta_dielectric)),
        eta_conductor_(std::move(eta_conductor)),
        k_conductor_(std::move(k_conductor)),
        roughness_u_(std::move(roughness_u)),
        roughness_v_(std::move(roughness_v)),
        remap_roughness_(remap_roughness) {}

  const Bxdf* Evaluate(const TextureCoordinates& texture_coordinates,
                       SpectralAllocator& spectral_allocator,
                       BxdfAllocator& bxdf_allocator) const override;

 private:
  ReferenceCounted<FloatTexture> eta_dielectric_;
  ReferenceCounted<Spectrum> eta_conductor_;
  ReferenceCounted<Spectrum> k_conductor_;
  ReferenceCounted<FloatTexture> roughness_u_;
  ReferenceCounted<FloatTexture> roughness_v_;
  bool remap_roughness_;
};

const Bxdf* MetalMaterial::Evaluate(
    const TextureCoordinates& texture_coordinates,
    SpectralAllocator& spectral_allocator,
    BxdfAllocator& bxdf_allocator) const {
  visual roughness_u = static_cast<visual>(0.0);
  if (roughness_u_) {
    roughness_u = roughness_u_->Evaluate(texture_coordinates);
  }

  visual roughness_v = static_cast<visual>(0.0);
  if (roughness_v_) {
    roughness_v = roughness_v_->Evaluate(texture_coordinates);
  }

  visual eta_dielectric = static_cast<visual>(0.0);
  if (eta_dielectric_) {
    eta_dielectric = eta_dielectric_->Evaluate(texture_coordinates);
  }

  return MakeMicrofacetConductorBrdf(
      bxdf_allocator, kWhite.Get(), eta_dielectric, eta_conductor_.Get(),
      k_conductor_.Get(), roughness_u, roughness_v, remap_roughness_);
}

}  // namespace

ReferenceCounted<Material> MakeMetalMaterial(
    ReferenceCounted<FloatTexture> eta_dielectric,
    ReferenceCounted<Spectrum> eta_conductor,
    ReferenceCounted<Spectrum> k_conductor,
    ReferenceCounted<FloatTexture> roughness_u,
    ReferenceCounted<FloatTexture> roughness_v, bool remap_roughness) {
  if (!eta_dielectric) {
    return ReferenceCounted<Material>();
  }

  return MakeReferenceCounted<MetalMaterial>(
      std::move(eta_dielectric), std::move(eta_conductor),
      std::move(k_conductor), std::move(roughness_u), std::move(roughness_v),
      remap_roughness);
}

}  // namespace materials
}  // namespace iris
