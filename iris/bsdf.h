#ifndef _IRIS_BSDF_
#define _IRIS_BSDF_

#include <optional>

#include "iris/bxdf.h"
#include "iris/float.h"
#include "iris/reflector.h"
#include "iris/sampler.h"
#include "iris/spectral_allocator.h"
#include "iris/vector.h"

namespace iris {

class Bsdf final {
 public:
  Bsdf(const Bxdf& bxdf, const Vector& surface_normal,
       const Vector& shading_normal, bool normalize = false) noexcept;

  bool IsDiffuse() const;

  struct Differentials {
    const Vector dx;
    const Vector dy;
  };

  struct SampleResult {
    const Reflector& reflector;
    const Vector direction;
    const std::optional<Differentials> differentials;
    visual_t pdf;

    // Never set on diffuse paths and always set on specular paths.
    std::optional<visual_t> etendue_conservation_factor;
  };

  std::optional<SampleResult> Sample(
      const Vector& incoming, const std::optional<Differentials>& differentials,
      Sampler sampler, SpectralAllocator& allocator,
      bool diffuse_only = false) const;

  struct ReflectanceResult {
    const Reflector& reflector;
    const visual_t pdf;
  };

  std::optional<ReflectanceResult> Reflectance(
      const Vector& incoming, const Vector& outgoing,
      SpectralAllocator& allocator) const;

 private:
  Bsdf(const Bxdf& bxdf, const Vector vectors[4]) noexcept;

  std::optional<Bxdf::Differentials> ToLocal(
      const std::optional<Differentials>& differentials) const;
  Vector ToLocal(const Vector& vector) const;

  std::optional<Differentials> ToWorld(
      const std::optional<Differentials>& incoming_differentials,
      const std::optional<Bxdf::Differentials>& outgoing_differentials) const;
  Vector ToWorld(const Vector& vector) const;

  const Bxdf& bxdf_;
  const Vector x_, y_, z_;
  const Vector surface_normal_;
  const Vector local_surface_normal_;
  bool is_diffuse_;
  visual_t diffuse_pdf_;
};

}  // namespace iris

#endif  // _IRIS_BSDF_
