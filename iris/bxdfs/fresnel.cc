#include "iris/bxdfs/fresnel.h"

#include <algorithm>
#include <cassert>
#include <cmath>

#include "iris/bxdfs/internal/math.h"

namespace iris {
namespace bxdfs {

const Reflector* FresnelDielectric::AttenuateReflectance(
    const Reflector& reflectance, visual_t cos_theta_incident,
    SpectralAllocator& allocator) const {
  return allocator.Scale(
      &reflectance, internal::FesnelDielectricReflectance(
                        cos_theta_incident, eta_incident_, eta_transmitted_));
}

const Reflector* FresnelDielectric::AttenuateTransmittance(
    const Reflector& transmittance, visual_t cos_theta_incident,
    SpectralAllocator& allocator) const {
  return allocator.Scale(
      &transmittance,
      static_cast<visual_t>(1.0) -
          internal::FesnelDielectricReflectance(
              cos_theta_incident, eta_incident_, eta_transmitted_));
}

}  // namespace bxdfs
}  // namespace iris