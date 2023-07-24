#include "iris/bxdfs/lambertian_brdf.h"

#define _USE_MATH_CONSTANTS
#include <cassert>
#include <cmath>

namespace iris {
namespace bxdfs {

std::optional<Bxdf::SampleResult> LambertianBrdf::Sample(
    const Vector& incoming,
    const std::optional<Bxdf::Differentials>& differentials,
    const Vector& surface_normal, Sampler& sampler) const {
  auto radius_squared = sampler.Next();
  auto radius = std::sqrt(radius_squared);

  auto theta = sampler.Next() * static_cast<geometric>(2.0 * M_PI);
  auto sin_theta = std::sin(theta);
  auto cos_theta = std::cos(theta);

  auto x = radius * cos_theta;
  auto y = radius * sin_theta;
  auto z = std::sqrt(static_cast<geometric>(1.0) - radius_squared);
  Vector outgoing(x, y, std::copysign(z, incoming.z));

  return SampleResult{outgoing.AlignWith(surface_normal), std::nullopt, this};
}

std::optional<visual_t> LambertianBrdf::Pdf(const Vector& incoming,
                                            const Vector& outgoing,
                                            const Vector& surface_normal,
                                            const Bxdf* sample_source,
                                            Hemisphere hemisphere) const {
  if (hemisphere != Hemisphere::BRDF) {
    return static_cast<visual_t>(0.0);
  }

  return std::abs(outgoing.z * M_1_PI);
}

const Reflector* LambertianBrdf::Reflectance(
    const Vector& incoming, const Vector& outgoing, const Bxdf* sample_source,
    Hemisphere hemisphere, SpectralAllocator& allocator) const {
  if (hemisphere != Hemisphere::BRDF) {
    return nullptr;
  }

  return allocator.Scale(&reflector_, M_1_PI);
}

}  // namespace bxdfs
}  // namespace iris