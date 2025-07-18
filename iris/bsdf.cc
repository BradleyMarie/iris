#include "iris/bsdf.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <utility>

#include "iris/bxdf.h"
#include "iris/float.h"
#include "iris/reflector.h"
#include "iris/sampler.h"
#include "iris/spectral_allocator.h"
#include "iris/vector.h"

namespace iris {
namespace {

std::pair<Vector, Vector> Normalize(const std::pair<Vector, Vector>& vectors) {
  return std::make_pair(Normalize(vectors.first), Normalize(vectors.second));
}

std::pair<Vector, Vector> MaybeNormalize(const Vector& surface_normal,
                                         const Vector& shading_normal,
                                         bool normalize) {
  std::pair<Vector, Vector> vectors =
      std::make_pair(surface_normal, shading_normal);
  if (!normalize) {
    return vectors;
  }

  return Normalize(vectors);
}

std::array<Vector, 4> ComputeState(const Vector& surface_normal,
                                   const Vector& shading_normal,
                                   bool normalize) {
  std::pair<Vector, Vector> vectors =
      MaybeNormalize(surface_normal, shading_normal, normalize);
  std::pair<Vector, Vector> coordinate_space = CoordinateSystem(vectors.second);
  return {coordinate_space.first, coordinate_space.second, vectors.second,
          vectors.first};
}

std::optional<std::pair<Vector, std::optional<Bxdf::SpecularSample>>>
SampleBxdf(const Bxdf& bxdf, const Vector& incoming,
           const std::optional<Bxdf::Differentials>& differentials,
           const Vector& surface_normal, Sampler& sampler,
           SpectralAllocator& allocator, bool diffuse_only) {
  if (diffuse_only) {
    std::optional<Vector> outgoing =
        bxdf.SampleDiffuse(incoming, surface_normal, sampler);
    if (!outgoing.has_value()) {
      return std::nullopt;
    }

    return std::make_pair(*outgoing, std::nullopt);
  }

  auto sample =
      bxdf.Sample(incoming, differentials, surface_normal, sampler, allocator);
  switch (sample.index()) {
    case 0:  // std::monostate
      return std::nullopt;
    case 1:  // Bxdf::DiffuseSample
      return std::make_pair(std::get<Bxdf::DiffuseSample>(sample).direction,
                            std::nullopt);
    case 2:  // Bxdf::SpecularSample
      break;
  }

  const Bxdf::SpecularSample& specular_sample =
      std::get<Bxdf::SpecularSample>(sample);
  if (specular_sample.reflectance == nullptr ||
      specular_sample.pdf <= static_cast<visual_t>(0.0)) {
    return std::nullopt;
  }

  return std::make_pair(specular_sample.direction, specular_sample);
}

}  // namespace

Bsdf::Bsdf(const Bxdf& bxdf, const Vector& surface_normal,
           const Vector& shading_normal, bool normalize) noexcept
    : Bsdf(bxdf,
           ComputeState(surface_normal, shading_normal, normalize).data()) {}

Bsdf::Bsdf(const Bxdf& bxdf, const Vector vectors[4]) noexcept
    : bxdf_(bxdf),
      x_(vectors[0]),
      y_(vectors[1]),
      z_(vectors[2]),
      surface_normal_(vectors[3]),
      local_surface_normal_(ToLocal(surface_normal_)),
      is_diffuse_(bxdf.IsDiffuse(&diffuse_pdf_)) {
  if (std::isnan(diffuse_pdf_)) {
    diffuse_pdf_ = static_cast<visual_t>(1.0);
  } else {
    diffuse_pdf_ = std::clamp(diffuse_pdf_, static_cast<visual_t>(0.0),
                              static_cast<visual_t>(1.0));
  }
}

std::optional<Bxdf::Differentials> Bsdf::ToLocal(
    const std::optional<Bsdf::Differentials>& differentials) const {
  if (!differentials) {
    return std::nullopt;
  }

  return Bxdf::Differentials{ToLocal(-differentials->dx),
                             ToLocal(-differentials->dy)};
}

Vector Bsdf::ToLocal(const Vector& vector) const {
  return Vector(DotProduct(x_, vector), DotProduct(y_, vector),
                DotProduct(z_, vector));
}

std::optional<Bsdf::Differentials> Bsdf::ToWorld(
    const std::optional<Differentials>& incoming_differentials,
    const std::optional<Bxdf::Differentials>& outgoing_differentials) const {
  if (!incoming_differentials || !outgoing_differentials) {
    return std::nullopt;
  }

  return Bsdf::Differentials{ToWorld(outgoing_differentials->dx),
                             ToWorld(outgoing_differentials->dy)};
}

Vector Bsdf::ToWorld(const Vector& vector) const {
  return Vector(x_.x * vector.x + y_.x * vector.y + z_.x * vector.z,
                x_.y * vector.x + y_.y * vector.y + z_.y * vector.z,
                x_.z * vector.x + y_.z * vector.y + z_.z * vector.z);
}

bool Bsdf::IsDiffuse() const { return is_diffuse_; }

std::optional<Bsdf::SampleResult> Bsdf::Sample(
    const Vector& incoming, const std::optional<Differentials>& differentials,
    Sampler sampler, SpectralAllocator& allocator, bool diffuse_only) const {
  geometric_t dp_incoming = DotProduct(incoming, surface_normal_);
  if (dp_incoming == static_cast<geometric_t>(0.0)) {
    return std::nullopt;
  }

  Vector local_incoming = ToLocal(-incoming);
  std::optional<Bxdf::Differentials> local_incoming_differentials =
      ToLocal(differentials);

  auto sample =
      SampleBxdf(bxdf_, local_incoming, local_incoming_differentials,
                 local_surface_normal_, sampler, allocator, diffuse_only);
  if (!sample) {
    return std::nullopt;
  }

  Vector world_outgoing = ToWorld(sample->first);

  geometric_t dp_outgoing = DotProduct(world_outgoing, surface_normal_);
  if (dp_outgoing == static_cast<geometric_t>(0.0)) {
    return std::nullopt;
  }

  Bxdf::Hemisphere type =
      (std::signbit(dp_incoming) == std::signbit(dp_outgoing))
          ? Bxdf::Hemisphere::BTDF
          : Bxdf::Hemisphere::BRDF;

  if (sample->second.has_value()) {
    if (sample->second->hemisphere != type) {
      return std::nullopt;
    }

    return Bsdf::SampleResult{
        *sample->second->reflectance, world_outgoing,
        ToWorld(differentials, sample->second->differentials),
        sample->second->pdf, sample->second->etendue_conservation_scalar};
  }

  visual_t pdf = bxdf_.PdfDiffuse(local_incoming, sample->first,
                                  local_surface_normal_, type);
  if (!std::isfinite(pdf) || pdf <= static_cast<visual_t>(0.0)) {
    return std::nullopt;
  }

  if (!diffuse_only) {
    pdf *= diffuse_pdf_;
  }

  const Reflector* reflector =
      bxdf_.ReflectanceDiffuse(local_incoming, sample->first, type, allocator);
  if (!reflector) {
    return std::nullopt;
  }

  return Bsdf::SampleResult{*reflector, world_outgoing, std::nullopt, pdf,
                            std::nullopt};
}

std::optional<Bsdf::ReflectanceResult> Bsdf::Reflectance(
    const Vector& incoming, const Vector& outgoing,
    SpectralAllocator& allocator) const {
  if (!is_diffuse_) {
    return std::nullopt;
  }

  geometric_t dp_incoming = DotProduct(incoming, surface_normal_);
  if (dp_incoming == static_cast<geometric_t>(0.0)) {
    return std::nullopt;
  }

  geometric_t dp_outgoing = DotProduct(outgoing, surface_normal_);
  if (dp_outgoing == static_cast<geometric_t>(0.0)) {
    return std::nullopt;
  }

  Bxdf::Hemisphere type =
      (std::signbit(dp_incoming) == std::signbit(dp_outgoing))
          ? Bxdf::Hemisphere::BTDF
          : Bxdf::Hemisphere::BRDF;

  Vector local_incoming = ToLocal(-incoming);
  Vector local_outgoing = ToLocal(outgoing);
  visual_t pdf = bxdf_.PdfDiffuse(local_incoming, local_outgoing,
                                  local_surface_normal_, type);
  if (!std::isfinite(pdf) || pdf <= static_cast<visual_t>(0.0)) {
    return std::nullopt;
  }

  const Reflector* reflector =
      bxdf_.ReflectanceDiffuse(local_incoming, local_outgoing, type, allocator);
  if (!reflector) {
    return std::nullopt;
  }

  return Bsdf::ReflectanceResult{*reflector, pdf};
}

}  // namespace iris
