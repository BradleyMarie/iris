#include "iris/internal/area_light.h"

#include "iris/internal/visibility_tester.h"

namespace iris::internal {
namespace {

std::optional<Vector> ToLight(
    const Point& model_origin,
    const std::variant<std::monostate, Point, Vector>& sample) {
  if (const auto* to_geometry = std::get_if<Vector>(&sample)) {
    return *to_geometry;
  }

  if (const auto* on_geometry = std::get_if<Point>(&sample)) {
    return *on_geometry - model_origin;
  }

  return std::nullopt;
}

}  // namespace

AreaLight::AreaLight(const Geometry& geometry, const Matrix* model_to_world,
                     face_t face) noexcept
    : geometry_(geometry), model_to_world_(model_to_world), face_(face) {}

std::optional<Light::SampleResult> AreaLight::Sample(
    const HitPoint& hit_point, Sampler sampler, iris::VisibilityTester& tester,
    SpectralAllocator& allocator) const {
  Point model_origin =
      model_to_world_
          ? model_to_world_->InverseMultiply(hit_point.ApproximateLocation())
          : hit_point.ApproximateLocation();

  auto model_to_light = ToLight(
      model_origin, geometry_.SampleBySolidAngle(model_origin, face_, sampler));
  if (!model_to_light) {
    return std::nullopt;
  }

  auto to_light =
      Normalize(model_to_world_ ? model_to_world_->Multiply(*model_to_light)
                                : *model_to_light);

  visual_t pdf;
  auto* emission =
      Emission(hit_point.CreateRay(to_light), tester, allocator, &pdf);
  if (!emission) {
    return std::nullopt;
  }

  return Light::SampleResult{*emission, to_light, pdf};
}

const Spectrum* AreaLight::Emission(const Ray& to_light,
                                    iris::VisibilityTester& tester,
                                    SpectralAllocator& allocator,
                                    visual_t* pdf) const {
  internal::VisibilityTester& internal_tester =
      static_cast<internal::VisibilityTester&>(tester);

  auto result =
      internal_tester.Visible(to_light, geometry_, model_to_world_, face_);
  if (!result) {
    return nullptr;
  }

  if (pdf) {
    *pdf = result->pdf;
  }

  return &result->emission;
}

}  // namespace iris::internal