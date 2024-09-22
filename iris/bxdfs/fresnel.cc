#include "iris/bxdfs/fresnel.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>

#include "iris/bxdfs/internal/math.h"

namespace iris {
namespace bxdfs {

const Reflector* FresnelDielectric::AttenuateReflectance(
    const Reflector& reflectance, visual_t cos_theta_incident,
    SpectralAllocator& allocator) const {
  geometric_t eta_incident =
      std::signbit(cos_theta_incident) ? eta_back_ : eta_front_;
  geometric_t eta_transmitted =
      std::signbit(cos_theta_incident) ? eta_front_ : eta_back_;
  visual_t fresnel_reflectance = internal::FesnelDielectricReflectance(
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
  visual_t fresnel_reflectance = internal::FesnelDielectricReflectance(
      cos_theta_incident, eta_incident, eta_transmitted);
  return allocator.Scale(&transmittance,
                         static_cast<visual_t>(1.0) - fresnel_reflectance);
}

const Reflector* FresnelConductor::AttenuateReflectance(
    const Reflector& reflectance, visual_t cos_theta_incident,
    SpectralAllocator& allocator) const {
  const Spectrum* eta_incident =
      std::signbit(cos_theta_incident) ? eta_back_ : eta_front_;
  const Spectrum* eta_transmitted =
      std::signbit(cos_theta_incident) ? eta_front_ : eta_back_;
  const Reflector* fresnel = allocator.FresnelConductor(
      cos_theta_incident, eta_incident, eta_transmitted, k_);
  return allocator.Scale(&reflectance, fresnel);
}

const Reflector* FresnelConductor::AttenuateTransmittance(
    const Reflector& transmittance, visual_t cos_theta_incident,
    SpectralAllocator& allocator) const {
  const Spectrum* eta_incident =
      std::signbit(cos_theta_incident) ? eta_back_ : eta_front_;
  const Spectrum* eta_transmitted =
      std::signbit(cos_theta_incident) ? eta_front_ : eta_back_;
  const Reflector* fresnel = allocator.FresnelConductor(
      cos_theta_incident, eta_incident, eta_transmitted, k_);
  return allocator.Scale(&transmittance, allocator.Invert(fresnel));
}

}  // namespace bxdfs
}  // namespace iris