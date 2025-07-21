#include "iris/materials/glass_material.h"

#include <utility>

#include "iris/bxdf.h"
#include "iris/bxdf_allocator.h"
#include "iris/bxdfs/composite_bxdf.h"
#include "iris/bxdfs/microfacet_bxdf.h"
#include "iris/bxdfs/specular_dielectric_bxdf.h"
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
using ::iris::bxdfs::MakeMicrofacetDielectricBrdf;
using ::iris::bxdfs::MakeMicrofacetDielectricBtdf;
using ::iris::bxdfs::MakeSpecularDielectricBxdf;
using ::iris::textures::FloatTexture;
using ::iris::textures::ReflectorTexture;

class GlassMaterial final : public Material {
 public:
  GlassMaterial(ReferenceCounted<ReflectorTexture> reflectance,
                ReferenceCounted<ReflectorTexture> transmittance,
                ReferenceCounted<FloatTexture> eta_incident,
                ReferenceCounted<FloatTexture> eta_transmitted,
                ReferenceCounted<FloatTexture> roughness_u,
                ReferenceCounted<FloatTexture> roughness_v,
                bool remap_roughness)
      : reflectance_(std::move(reflectance)),
        transmittance_(std::move(transmittance)),
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
  ReferenceCounted<FloatTexture> eta_incident_;
  ReferenceCounted<FloatTexture> eta_transmitted_;
  ReferenceCounted<FloatTexture> roughness_u_;
  ReferenceCounted<FloatTexture> roughness_v_;
  bool remap_roughness_;
};

const Bxdf* GlassMaterial::Evaluate(
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

  visual eta_incident = static_cast<visual>(0.0);
  if (eta_incident_) {
    eta_incident = eta_incident_->Evaluate(texture_coordinates);
  }

  visual eta_transmitted = static_cast<visual>(0.0);
  if (eta_transmitted_) {
    eta_transmitted = eta_transmitted_->Evaluate(texture_coordinates);
  }

  visual roughness_u = static_cast<visual>(0.0);
  if (roughness_u_) {
    roughness_u = roughness_u_->Evaluate(texture_coordinates);
  }

  visual roughness_v = static_cast<visual>(0.0);
  if (roughness_v_) {
    roughness_v = roughness_v_->Evaluate(texture_coordinates);
  }

  if (roughness_u == static_cast<visual>(0.0) &&
      roughness_v == static_cast<visual>(0.0)) {
    return MakeSpecularDielectricBxdf(bxdf_allocator, reflectance,
                                      transmittance, eta_incident,
                                      eta_transmitted);
  }

  const Bxdf* microfacet_brdf = MakeMicrofacetDielectricBtdf(
      bxdf_allocator, reflectance, eta_incident, eta_transmitted, roughness_u,
      roughness_v, remap_roughness_);

  const Bxdf* microfacet_btdf = MakeMicrofacetDielectricBrdf(
      bxdf_allocator, transmittance, eta_incident, eta_transmitted, roughness_u,
      roughness_v, remap_roughness_);

  return MakeCompositeBxdf(bxdf_allocator, microfacet_brdf, microfacet_btdf);
}

}  // namespace

ReferenceCounted<Material> MakeGlassMaterial(
    ReferenceCounted<ReflectorTexture> reflectance,
    ReferenceCounted<ReflectorTexture> transmittance,
    ReferenceCounted<FloatTexture> eta_incident,
    ReferenceCounted<FloatTexture> eta_transmitted,
    ReferenceCounted<FloatTexture> roughness_u,
    ReferenceCounted<FloatTexture> roughness_v, bool remap_roughness) {
  if (!eta_incident || !eta_transmitted) {
    eta_incident.Reset();
    eta_transmitted.Reset();
    transmittance.Reset();
  }

  if (!reflectance && !transmittance) {
    return ReferenceCounted<Material>();
  }

  return MakeReferenceCounted<GlassMaterial>(
      std::move(reflectance), std::move(transmittance), std::move(eta_incident),
      std::move(eta_transmitted), std::move(roughness_u),
      std::move(roughness_v), remap_roughness);
}

}  // namespace materials
}  // namespace iris
