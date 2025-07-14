#include "iris/bxdfs/mirror_brdf.h"

#include "iris/bxdf.h"
#include "iris/bxdf_allocator.h"
#include "iris/bxdfs/internal/specular_bxdf.h"
#include "iris/float.h"
#include "iris/reflector.h"
#include "iris/sampler.h"
#include "iris/spectral_allocator.h"
#include "iris/vector.h"

namespace iris {
namespace bxdfs {
namespace {

class MirrorBrdf final : public internal::SpecularBxdf {
 public:
  MirrorBrdf(const Reflector& reflectance) noexcept
      : reflectance_(reflectance) {}

  std::optional<Bxdf::SpecularSample> SampleSpecular(
      const Vector& incoming,
      const std::optional<Bxdf::Differentials>& differentials,
      const Vector& surface_normal, Sampler& sampler,
      SpectralAllocator& allocator) const override;

 private:
  const Reflector& reflectance_;
};

Vector ReflectVector(const Vector vector) {
  return Vector(-vector.x, -vector.y, vector.z);
}

std::optional<Bxdf::Differentials> ReflectDifferentials(
    const std::optional<Bxdf::Differentials>& differentials) {
  if (!differentials) {
    return std::nullopt;
  }

  return Bxdf::Differentials{ReflectVector(differentials->dx),
                             ReflectVector(differentials->dy)};
}

std::optional<Bxdf::SpecularSample> MirrorBrdf::SampleSpecular(
    const Vector& incoming, const std::optional<Differentials>& differentials,
    const Vector& surface_normal, Sampler& sampler,
    SpectralAllocator& allocator) const {
  return Bxdf::SpecularSample{
      .hemisphere = Hemisphere::BRDF,
      .direction = ReflectVector(incoming),
      .reflectance = &reflectance_,
      .differentials = ReflectDifferentials(differentials),
      .pdf = static_cast<visual_t>(1.0),
      .etendue_conservation_scalar = static_cast<visual_t>(1.0)};
}

}  // namespace

const Bxdf* MakeMirrorBrdf(BxdfAllocator& bxdf_allocator,
                           const Reflector* reflectance) {
  if (!reflectance) {
    return nullptr;
  }

  return &bxdf_allocator.Allocate<MirrorBrdf>(*reflectance);
}

}  // namespace bxdfs
}  // namespace iris
