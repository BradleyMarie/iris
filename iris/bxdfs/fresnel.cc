#include "iris/bxdfs/fresnel.h"

#include <algorithm>
#include <cassert>
#include <cmath>

#include "iris/bxdfs/internal/math.h"
#include "iris/reflectors/uniform_reflector.h"

namespace iris {
namespace bxdfs {

const Reflector* FresnelDielectric::Evaluate(
    visual_t cos_theta_incident, SpectralAllocator& allocator) const {
  static const reflectors::UniformReflector perfect_reflector(1.0);
  return allocator.Scale(
      &perfect_reflector,
      internal::FesnelDielectricReflectance(cos_theta_incident, eta_incident_,
                                            eta_transmitted_));
}

const Reflector* FresnelDielectric::Complement(
    visual_t cos_theta_incident, SpectralAllocator& allocator) const {
  static const reflectors::UniformReflector perfect_reflector(1.0);
  return allocator.Scale(
      &perfect_reflector,
      static_cast<visual_t>(1.0) -
          internal::FesnelDielectricReflectance(
              cos_theta_incident, eta_incident_, eta_transmitted_));
}

}  // namespace bxdfs
}  // namespace iris