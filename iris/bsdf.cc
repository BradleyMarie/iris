#include "iris/bsdf.h"

#include <array>

namespace iris {
namespace {

std::pair<Vector, Vector> Normalize(const std::pair<Vector, Vector>& vectors) {
  return std::make_pair(Normalize(vectors.first), Normalize(vectors.second));
}

std::pair<Vector, Vector> MaybeNormalize(const Vector& surface_normal,
                                         const Vector& shading_normal,
                                         bool normalize) {
  auto vectors = std::make_pair(surface_normal, shading_normal);
  if (!normalize) {
    return vectors;
  }

  return Normalize(vectors);
}

std::array<Vector, 4> ComputeState(const Vector& surface_normal,
                                   const Vector& shading_normal,
                                   bool normalize) {
  auto vectors = MaybeNormalize(surface_normal, shading_normal, normalize);
  auto coordinate_space = CoordinateSystem(vectors.second);
  return {coordinate_space.first, coordinate_space.second, vectors.second,
          vectors.first};
}

}  // namespace

Bsdf::Bsdf(const Bxdf& bxdf, const Vector& surface_normal,
           const Vector& shading_normal, bool normalize) noexcept
    : Bsdf(bxdf,
           ComputeState(surface_normal, shading_normal, normalize).data()) {}

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
    const std::optional<Bxdf::Differentials>& differentials) const {
  if (!differentials) {
    return std::nullopt;
  }

  return Bsdf::Differentials{ToWorld(differentials->dx),
                             ToWorld(differentials->dy)};
}

Vector Bsdf::ToWorld(const Vector& vector) const {
  return Vector(x_.x * vector.x + y_.x * vector.y + z_.x * vector.z,
                x_.y * vector.x + y_.y * vector.y + z_.y * vector.z,
                x_.z * vector.x + y_.z * vector.y + z_.z * vector.z);
}

std::optional<Bsdf::SampleResult> Bsdf::Sample(
    const Vector& incoming, const std::optional<Differentials>& differentials,
    Sampler sampler, SpectralAllocator& allocator) const {
  geometric_t dp_incoming = DotProduct(incoming, surface_normal_);
  if (dp_incoming == static_cast<geometric_t>(0.0)) {
    return std::nullopt;
  }

  auto local_incoming = ToLocal(-incoming);
  auto local_incoming_differentials = ToLocal(differentials);

  auto sample_result =
      bxdf_.Sample(local_incoming, local_incoming_differentials,
                   local_surface_normal_, sampler);
  if (!sample_result) {
    return std::nullopt;
  }

  auto world_outgoing = ToWorld(sample_result->direction);

  geometric_t dp_outgoing = DotProduct(world_outgoing, surface_normal_);
  if (dp_outgoing == static_cast<geometric_t>(0.0)) {
    return std::nullopt;
  }

  auto type = (std::signbit(dp_incoming) == std::signbit(dp_outgoing))
                  ? Bxdf::Hemisphere::BTDF
                  : Bxdf::Hemisphere::BRDF;

  auto pdf =
      bxdf_.Pdf(local_incoming, sample_result->direction, local_surface_normal_,
                sample_result->sample_source, type);
  if (pdf.has_value() && *pdf <= 0.0) {
    return std::nullopt;
  }

  auto reflector =
      bxdf_.Reflectance(local_incoming, sample_result->direction,
                        sample_result->sample_source, type, allocator);
  if (!reflector) {
    return std::nullopt;
  }

  auto world_outgoing_differentials = ToWorld(sample_result->differentials);
  return Bsdf::SampleResult{
      *reflector, world_outgoing, world_outgoing_differentials,
      pdf.value_or(static_cast<visual_t>(1.0)) * sample_result->pdf_weight,
      pdf.has_value()};
}

std::optional<Bsdf::ReflectanceResult> Bsdf::Reflectance(
    const Vector& incoming, const Vector& outgoing,
    SpectralAllocator& allocator) const {
  geometric_t dp_incoming = DotProduct(incoming, surface_normal_);
  if (dp_incoming == static_cast<geometric_t>(0.0)) {
    return std::nullopt;
  }

  geometric_t dp_outgoing = DotProduct(outgoing, surface_normal_);
  if (dp_outgoing == static_cast<geometric_t>(0.0)) {
    return std::nullopt;
  }

  auto type = (std::signbit(dp_incoming) == std::signbit(dp_outgoing))
                  ? Bxdf::Hemisphere::BTDF
                  : Bxdf::Hemisphere::BRDF;

  auto local_incoming = ToLocal(-incoming);
  auto local_outgoing = ToLocal(outgoing);
  auto pdf = bxdf_.Pdf(local_incoming, local_outgoing, local_surface_normal_,
                       nullptr, type);
  if (pdf.value_or(0.0) <= 0.0) {
    return std::nullopt;
  }

  auto* reflector = bxdf_.Reflectance(local_incoming, local_outgoing, nullptr,
                                      type, allocator);
  if (!reflector) {
    return std::nullopt;
  }

  return Bsdf::ReflectanceResult{*reflector, *pdf};
}

}  // namespace iris