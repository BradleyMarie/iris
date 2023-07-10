#include "iris/bxdfs/mirror_brdf.h"

namespace iris {
namespace bxdfs {

std::optional<Bxdf::SampleResult> MirrorBrdf::Sample(
    const Vector& incoming,
    const std::optional<Bxdf::Differentials>& differentials,
    Sampler& sampler) const {
  Vector reflected(-incoming.x, -incoming.y, incoming.z);
  if (!differentials) {
    return Bxdf::SampleResult{reflected, std::nullopt, this};
  }

  Differentials outgoing_diffs = {
      Vector(-differentials->dx.x, -differentials->dx.y, differentials->dx.z),
      Vector(-differentials->dy.x, -differentials->dy.y, differentials->dy.z)};
  return Bxdf::SampleResult{reflected, outgoing_diffs, this};
}

std::optional<visual_t> MirrorBrdf::Pdf(const Vector& incoming,
                                        const Vector& outgoing,
                                        const Bxdf* sample_source,
                                        Hemisphere hemisphere) const {
  if (sample_source != this || hemisphere != Hemisphere::BRDF) {
    return static_cast<geometric_t>(0.0);
  }

  return std::nullopt;
}

const Reflector* MirrorBrdf::Reflectance(const Vector& incoming,
                                         const Vector& outgoing,
                                         const Bxdf* sample_source,
                                         Hemisphere hemisphere,
                                         SpectralAllocator& allocator) const {
  if (sample_source != this || hemisphere != Hemisphere::BRDF) {
    return nullptr;
  }

  return &reflector_;
}

}  // namespace bxdfs
}  // namespace iris