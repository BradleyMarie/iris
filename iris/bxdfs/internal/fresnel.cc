#include "iris/bxdfs/internal/fresnel.h"

#include "iris/bxdfs/internal/math.h"
#include "iris/color.h"
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
  visual_t eta_incident =
      std::signbit(cos_theta_incident) ? eta_back_ : eta_front_;
  visual_t eta_transmitted =
      std::signbit(cos_theta_incident) ? eta_front_ : eta_back_;
  visual_t fresnel_reflectance = FresnelDielectricReflectance(
      cos_theta_incident, eta_incident, eta_transmitted);
  return allocator.Scale(&reflectance, fresnel_reflectance);
}

const Reflector* FresnelDielectric::AttenuateTransmittance(
    const Reflector& transmittance, visual_t cos_theta_incident,
    SpectralAllocator& allocator) const {
  visual_t eta_incident =
      std::signbit(cos_theta_incident) ? eta_back_ : eta_front_;
  visual_t eta_transmitted =
      std::signbit(cos_theta_incident) ? eta_front_ : eta_back_;
  visual_t fresnel_reflectance = FresnelDielectricReflectance(
      cos_theta_incident, eta_incident, eta_transmitted);
  return allocator.Scale(&transmittance,
                         (static_cast<visual_t>(1.0) - fresnel_reflectance));
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

const Reflector* DisneyFresnel::AttenuateReflectance(
    const Reflector& reflectance, visual_t cos_theta_incident,
    SpectralAllocator& allocator) const {
  visual_t eta_incident =
      std::signbit(cos_theta_incident) ? eta_back_ : eta_front_;
  visual_t eta_transmitted =
      std::signbit(cos_theta_incident) ? eta_front_ : eta_back_;

  // TODO: Make this something SpectralAllocator can do
  visual_t color_luma = static_cast<visual_t>(0.0);
  if (color_) {
    visual_t r = color_->Reflectance(static_cast<visual_t>(0.5));
    visual_t g = color_->Reflectance(static_cast<visual_t>(1.5));
    visual_t b = color_->Reflectance(static_cast<visual_t>(2.5));
    color_luma = Color(r, g, b, Color::LINEAR_SRGB).Luma();
  }

  const Reflector* schlick_dielectric_color = allocator.Invert(nullptr);
  if (color_luma > static_cast<visual_t>(0.0)) {
    schlick_dielectric_color =
        allocator.Lerp(schlick_dielectric_color,
                       allocator.UnboundedScale(
                           color_, static_cast<visual_t>(1.0) / color_luma),
                       specular_tint_);
  }

  visual_t schlick_r0 =
      (eta_transmitted - eta_incident) / (eta_transmitted + eta_incident);
  schlick_r0 *= schlick_r0;

  schlick_dielectric_color =
      allocator.Scale(schlick_dielectric_color, schlick_r0);

  visual_t fresnel_reflectance = FresnelDielectricReflectance(
      cos_theta_incident, eta_incident, eta_transmitted);
  visual_t schlick_reflectance = SchlickWeight(cos_theta_incident);
  visual_t non_metallic = static_cast<visual_t>(1.0) - metallic_;

  const Reflector* metallic = allocator.Scale(
      allocator.Lerp(color_, allocator.Invert(nullptr), schlick_reflectance),
      metallic_);
  const Reflector* fresnel_dielectric =
      allocator.Scale(allocator.Invert(nullptr),
                      fresnel_reflectance * non_metallic *
                          (static_cast<visual_t>(1.0) - specular_tint_));
  const Reflector* schlick_dielectric = allocator.Scale(
      allocator.Lerp(schlick_dielectric_color, allocator.Invert(nullptr),
                     schlick_reflectance),
      non_metallic * specular_tint_);

  return allocator.UnboundedAdd(metallic, fresnel_dielectric,
                                schlick_dielectric);
}

const Reflector* DisneyFresnel::AttenuateTransmittance(
    const Reflector& transmittance, visual_t cos_theta_incident,
    SpectralAllocator& allocator) const {
  return nullptr;
}

bool DisneyFresnel::IsValid() const {
  return std::isfinite(metallic_) && metallic_ >= static_cast<visual_t>(0.0) &&
         std::isfinite(specular_tint_) &&
         specular_tint_ >= static_cast<visual_t>(0.0) &&
         std::isfinite(eta_front_) &&
         eta_front_ >= static_cast<visual_t>(1.0) && std::isfinite(eta_back_) &&
         eta_back_ >= static_cast<visual_t>(1.0);
}

}  // namespace internal
}  // namespace bxdfs
}  // namespace iris
