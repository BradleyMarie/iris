#ifndef _IRIS_BXDFS_INTERNAL_FRESNEL_
#define _IRIS_BXDFS_INTERNAL_FRESNEL_

#include "iris/float.h"
#include "iris/reflector.h"
#include "iris/spectral_allocator.h"
#include "iris/spectrum.h"
#include "iris/vector.h"

namespace iris {
namespace bxdfs {
namespace internal {

class Fresnel {
 public:
  virtual const Reflector* AttenuateReflectance(
      const Reflector& reflectance, visual_t cos_theta_incident,
      SpectralAllocator& allocator) const = 0;
  virtual const Reflector* AttenuateTransmittance(
      const Reflector& transmittance, visual_t cos_theta_incident,
      SpectralAllocator& allocator) const = 0;
  virtual bool IsValid() const = 0;
};

class FresnelDielectric final : public Fresnel {
 public:
  FresnelDielectric(visual_t eta_front, visual_t eta_back)
      : eta_front_(eta_front), eta_back_(eta_back) {}

  const Reflector* AttenuateReflectance(
      const Reflector& reflectance, visual_t cos_theta_incident,
      SpectralAllocator& allocator) const override;
  const Reflector* AttenuateTransmittance(
      const Reflector& transmittance, visual_t cos_theta_incident,
      SpectralAllocator& allocator) const override;
  bool IsValid() const override;

 private:
  visual_t eta_front_;
  visual_t eta_back_;
};

class FresnelConductor final : public Fresnel {
 public:
  FresnelConductor(visual_t eta_dielectric, const Spectrum* eta_conductor,
                   const Spectrum* k_conductor)
      : eta_dielectric_(eta_dielectric),
        eta_conductor_(eta_conductor),
        k_conductor_(k_conductor) {}

  const Reflector* AttenuateReflectance(
      const Reflector& reflectance, visual_t cos_theta_incident,
      SpectralAllocator& allocator) const override;
  const Reflector* AttenuateTransmittance(
      const Reflector& transmittance, visual_t cos_theta_incident,
      SpectralAllocator& allocator) const override;
  bool IsValid() const override;

 private:
  visual_t eta_dielectric_;
  const Spectrum* eta_conductor_;
  const Spectrum* k_conductor_;
};

class SchlickFresnel final : public Fresnel {
 public:
  SchlickFresnel(const Reflector* color,
                 visual_t scalar = static_cast<visual_t>(1.0))
      : color_(color), scalar_(scalar) {}

  const Reflector* AttenuateReflectance(
      const Reflector& reflectance, visual_t cos_theta_incident,
      SpectralAllocator& allocator) const override;
  const Reflector* AttenuateTransmittance(
      const Reflector& transmittance, visual_t cos_theta_incident,
      SpectralAllocator& allocator) const override;
  bool IsValid() const override;

 private:
  const Reflector* color_;
  visual_t scalar_;
};

class DisneyFresnel final : public Fresnel {
 public:
  DisneyFresnel(const Reflector* color, visual_t metallic,
                visual_t specular_tint, visual_t eta_front, visual_t eta_back)
      : color_(color),
        metallic_(metallic),
        specular_tint_(specular_tint),
        eta_front_(eta_front),
        eta_back_(eta_back) {}

  const Reflector* AttenuateReflectance(
      const Reflector& reflectance, visual_t cos_theta_incident,
      SpectralAllocator& allocator) const override;
  const Reflector* AttenuateTransmittance(
      const Reflector& transmittance, visual_t cos_theta_incident,
      SpectralAllocator& allocator) const override;
  bool IsValid() const override;

 private:
  const Reflector* color_;
  visual_t metallic_;
  visual_t specular_tint_;
  visual_t eta_front_;
  visual_t eta_back_;
};

}  // namespace internal
}  // namespace bxdfs
}  // namespace iris

#endif  // _IRIS_BXDFS_INTERNAL_FRESNEL_
