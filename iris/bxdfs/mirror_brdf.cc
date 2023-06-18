#include "iris/bxdfs/mirror_brdf.h"

namespace iris {
namespace bxdfs {

std::optional<Bxdf::SampleResult> MirrorBrdf::Sample(
    const Vector& incoming,
    const std::optional<Bxdf::Differentials>& differentials,
    Sampler& sampler) const {
  Vector reflected(-incoming.x, -incoming.y, incoming.z);
  if (!differentials) {
    return Bxdf::SampleResult{reflected};
  }

  Differentials outgoing_diffs = {
      Vector(-differentials->dx.x, -differentials->dx.y, differentials->dx.z),
      Vector(-differentials->dy.x, -differentials->dy.y, differentials->dy.z)};
  return Bxdf::SampleResult{{reflected}, {outgoing_diffs}};
}

std::optional<visual_t> MirrorBrdf::Pdf(const Vector& incoming,
                                        const Vector& outgoing,
                                        SampleSource sample_source) const {
  if (sample_source == SampleSource::LIGHT) {
    return static_cast<geometric_t>(0.0);
  }

  if (incoming.x != -outgoing.x || incoming.y != -outgoing.y ||
      incoming.z != outgoing.z) {
    return static_cast<geometric_t>(0.0);
  }

  return std::nullopt;
}

const Reflector* MirrorBrdf::Reflectance(const Vector& incoming,
                                         const Vector& outgoing,
                                         SampleSource sample_source,
                                         Hemisphere hemisphere,
                                         SpectralAllocator& allocator) const {
  if (sample_source == SampleSource::LIGHT) {
    return nullptr;
  }

  if (hemisphere != Hemisphere::BRDF) {
    return nullptr;
  }

  if (incoming.x != -outgoing.x || incoming.y != -outgoing.y ||
      incoming.z != outgoing.z) {
    return nullptr;
  }

  return &reflector_;
}

}  // namespace bxdfs
}  // namespace iris