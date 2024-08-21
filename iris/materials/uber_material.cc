#include "iris/materials/uber_material.h"

#include "iris/bxdfs/composite_bxdf.h"
#include "iris/bxdfs/lambertian_bxdf.h"
#include "iris/bxdfs/microfacet_bxdf.h"
#include "iris/bxdfs/specular_bxdf.h"
#include "iris/reflectors/uniform_reflector.h"

namespace iris {
namespace materials {
namespace {

static const ReferenceCounted<Reflector> kWhite =
    reflectors::CreateUniformReflector(static_cast<visual>(1.0));

}

const Bxdf* UberMaterial::Evaluate(
    const TextureCoordinates& texture_coordinates,
    SpectralAllocator& spectral_allocator,
    BxdfAllocator& bxdf_allocator) const {
  visual_t opacity =
      std::clamp(opacity_->Evaluate(texture_coordinates),
                 static_cast<visual>(0.0), static_cast<visual>(1.0));
  visual_t transparency = static_cast<visual>(1.0) - opacity;

  const Bxdf* transparent_btdf = nullptr;
  if (transparency > static_cast<visual_t>(0.0)) {
    transparent_btdf =
        &bxdf_allocator.Allocate<bxdfs::SpecularBtdf<bxdfs::FresnelNoOp>>(
            *spectral_allocator.Scale(kWhite.Get(), transparency),
            static_cast<geometric_t>(1.0), static_cast<geometric_t>(1.0),
            bxdfs::FresnelNoOp());
  }

  const Bxdf* lambertian_brdf = nullptr;
  const Bxdf* microfacet_brdf = nullptr;
  const Bxdf* specular_brdf = nullptr;
  const Bxdf* specular_btdf = nullptr;
  if (opacity > static_cast<visual_t>(0.0)) {
    visual eta_incident = eta_incident_->Evaluate(texture_coordinates);
    visual eta_transmitted = eta_transmitted_->Evaluate(texture_coordinates);

    if (const Reflector* diffuse =
            diffuse_->Evaluate(texture_coordinates, spectral_allocator);
        diffuse != nullptr) {
      lambertian_brdf = &bxdf_allocator.Allocate<bxdfs::LambertianBrdf>(
          *spectral_allocator.Scale(diffuse, opacity));
    }

    if (const Reflector* specular =
            specular_->Evaluate(texture_coordinates, spectral_allocator);
        specular != nullptr) {
      visual_t roughness_u = roughness_u_->Evaluate(texture_coordinates);
      visual_t roughness_v = roughness_v_->Evaluate(texture_coordinates);
      if (remap_roughness_) {
        roughness_u =
            bxdfs::TrowbridgeReitzDistribution::RoughnessToAlpha(roughness_u);
        roughness_v =
            bxdfs::TrowbridgeReitzDistribution::RoughnessToAlpha(roughness_v);
      }

      microfacet_brdf = &bxdf_allocator.Allocate<bxdfs::MicrofacetBrdf<
          bxdfs::TrowbridgeReitzDistribution, bxdfs::FresnelDielectric>>(
          *spectral_allocator.Scale(specular, opacity),
          bxdfs::TrowbridgeReitzDistribution(roughness_u, roughness_v),
          bxdfs::FresnelDielectric(eta_incident, eta_transmitted));
    }

    if (const Reflector* reflectance =
            reflectance_->Evaluate(texture_coordinates, spectral_allocator);
        reflectance != nullptr) {
      specular_brdf =
          &bxdf_allocator
               .Allocate<bxdfs::SpecularBrdf<bxdfs::FresnelDielectric>>(
                   *spectral_allocator.Scale(reflectance, opacity),
                   bxdfs::FresnelDielectric(eta_incident, eta_transmitted));
    }

    if (const Reflector* transmittance =
            transmittance_->Evaluate(texture_coordinates, spectral_allocator);
        transmittance != nullptr) {
      specular_brdf =
          &bxdf_allocator
               .Allocate<bxdfs::SpecularBtdf<bxdfs::FresnelDielectric>>(
                   *spectral_allocator.Scale(transmittance, opacity),
                   eta_incident, eta_transmitted,
                   bxdfs::FresnelDielectric(eta_incident, eta_transmitted));
    }
  }

  return bxdfs::MakeCompositeBxdf(bxdf_allocator, transparent_btdf,
                                  lambertian_brdf, microfacet_brdf,
                                  specular_brdf, specular_btdf);
}

}  // namespace materials
}  // namespace iris