#ifndef _IRIS_BXDFS_FRESNEL_
#define _IRIS_BXDFS_FRESNEL_

#include "iris/float.h"
#include "iris/spectral_allocator.h"

namespace iris {
namespace bxdfs {

class Fresnel {
 public:
  virtual const Reflector* AttenuateReflectance(
      const Reflector& reflectance, visual_t cos_theta_incident,
      SpectralAllocator& allocator) const = 0;
  virtual const Reflector* AttenuateTransmittance(
      const Reflector& transmittance, visual_t cos_theta_incident,
      SpectralAllocator& allocator) const = 0;
};

class FresnelNoOp final : public Fresnel {
 public:
  const Reflector* AttenuateReflectance(
      const Reflector& reflectance, visual_t cos_theta_incident,
      SpectralAllocator& allocator) const override {
    return &reflectance;
  }

  const Reflector* AttenuateTransmittance(
      const Reflector& transmittance, visual_t cos_theta_incident,
      SpectralAllocator& allocator) const override {
    return &transmittance;
  }
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

 private:
  visual_t eta_front_;
  visual_t eta_back_;
};

class FresnelConductor final : public Fresnel {
 public:
  FresnelConductor(const Spectrum* eta_front, const Spectrum* eta_back,
                   const Spectrum* k)
      : eta_front_(eta_front), eta_back_(eta_back), k_(k) {}

  const Reflector* AttenuateReflectance(
      const Reflector& reflectance, visual_t cos_theta_incident,
      SpectralAllocator& allocator) const override;
  const Reflector* AttenuateTransmittance(
      const Reflector& transmittance, visual_t cos_theta_incident,
      SpectralAllocator& allocator) const override;

 private:
  const Spectrum* eta_front_;
  const Spectrum* eta_back_;
  const Spectrum* k_;
};

}  // namespace bxdfs
}  // namespace iris

#endif  // _IRIS_BXDFS_MICROFACET_BXDF_
