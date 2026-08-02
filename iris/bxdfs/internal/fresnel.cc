#include "iris/bxdfs/internal/fresnel.h"

#include "iris/bxdfs/internal/math.h"
#include "iris/float.h"
#include "iris/reflector.h"
#include "iris/spectral_allocator.h"
#include "iris/spectrum.h"
#include "iris/vector.h"

namespace iris {
namespace bxdfs {
namespace internal {

const Reflector* FresnelDielectric::AttenuateReflectance(
    const Reflector& reflectance, visual_t cos_theta_incident,
    SpectralAllocator& allocator) const {
  geometric_t eta_incident =
      std::signbit(cos_theta_incident) ? eta_back_ : eta_front_;
  geometric_t eta_transmitted =
      std::signbit(cos_theta_incident) ? eta_front_ : eta_back_;
  visual_t fresnel_reflectance = FesnelDielectricReflectance(
      cos_theta_incident, eta_incident, eta_transmitted);
  return allocator.Scale(&reflectance, fresnel_reflectance);
}

const Reflector* FresnelDielectric::AttenuateTransmittance(
    const Reflector& transmittance, visual_t cos_theta_incident,
    SpectralAllocator& allocator) const {
  geometric_t eta_incident =
      std::signbit(cos_theta_incident) ? eta_back_ : eta_front_;
  geometric_t eta_transmitted =
      std::signbit(cos_theta_incident) ? eta_front_ : eta_back_;
  visual_t fresnel_reflectance = FesnelDielectricReflectance(
      cos_theta_incident, eta_incident, eta_transmitted);
  return allocator.Scale(&transmittance,
                         static_cast<visual_t>(1.0) - fresnel_reflectance);
}

bool FresnelDielectric::IsValid() const {
  return std::isfinite(eta_front_) &&
         eta_front_ >= static_cast<visual_t>(1.0) && std::isfinite(eta_back_) &&
         eta_back_ >= static_cast<visual_t>(1.0);
}

const Reflector* FresnelConductor::AttenuateReflectance(
    const Reflector& reflectance, visual_t cos_theta_incident,
    SpectralAllocator& allocator) const {
  const Reflector* fresnel = allocator.FresnelConductor(
      eta_dielectric_, eta_conductor_, k_conductor_, cos_theta_incident);
  return allocator.Scale(&reflectance, fresnel);
}

const Reflector* FresnelConductor::AttenuateTransmittance(
    const Reflector& transmittance, visual_t cos_theta_incident,
    SpectralAllocator& allocator) const {
  return nullptr;
}

bool FresnelConductor::IsValid() const {
  return std::isfinite(eta_dielectric_) &&
         eta_dielectric_ >= static_cast<visual_t>(1.0);
}

}  // namespace internal
}  // namespace bxdfs
}  // namespace iris
