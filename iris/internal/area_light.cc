#include "iris/internal/area_light.h"

#include <numbers>
#include <optional>

#include "iris/float.h"
#include "iris/geometry.h"
#include "iris/hit_point.h"
#include "iris/integer.h"
#include "iris/internal/visibility_tester.h"
#include "iris/light.h"
#include "iris/matrix.h"
#include "iris/power_matcher.h"
#include "iris/ray.h"
#include "iris/reference_counted.h"
#include "iris/spectral_allocator.h"
#include "iris/spectrum.h"

namespace iris {
namespace internal {
namespace {

class AreaLight final : public Light {
 public:
  AreaLight(const Geometry& geometry, const Matrix* model_to_world,
            face_t face) noexcept;

  std::optional<SampleResult> Sample(
      const HitPoint& hit_point, Sampler sampler,
      iris::VisibilityTester& tester,
      SpectralAllocator& allocator) const override;

  const Spectrum* Emission(const Ray& to_light, iris::VisibilityTester& tester,
                           SpectralAllocator& allocator,
                           visual_t* pdf = nullptr) const override;

  visual_t Power(const PowerMatcher& power_matcher,
                 geometric_t world_radius_squared) const override;

 private:
  const Geometry& geometry_;
  const Matrix* model_to_world_;
  face_t face_;
};

std::optional<Vector> ToLight(
    const Point& model_origin,
    const std::variant<std::monostate, Point, Vector>& sample) {
  if (const Vector* to_geometry = std::get_if<Vector>(&sample)) {
    return *to_geometry;
  }

  if (const Point* on_geometry = std::get_if<Point>(&sample)) {
    return *on_geometry - model_origin;
  }

  return std::nullopt;
}

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

  std::optional<Vector> model_to_light = ToLight(
      model_origin, geometry_.SampleBySolidAngle(model_origin, face_, sampler));
  if (!model_to_light) {
    return std::nullopt;
  }

  Vector to_light =
      Normalize(model_to_world_ ? model_to_world_->Multiply(*model_to_light)
                                : *model_to_light);

  visual_t pdf;
  const Spectrum* emission =
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
  VisibilityTester& internal_tester = static_cast<VisibilityTester&>(tester);

  std::optional<VisibilityTester::VisibleResult> result =
      internal_tester.Visible(to_light, geometry_, model_to_world_, face_);
  if (!result) {
    return nullptr;
  }

  if (pdf) {
    *pdf = result->pdf;
  }

  return &result->emission;
}

visual_t AreaLight::Power(const PowerMatcher& power_matcher,
                          geometric_t world_radius_squared) const {
  visual_t unit_power =
      geometry_.GetEmissiveMaterial(face_)->UnitPower(power_matcher);

  visual_t surface_area =
      std::max(static_cast<visual_t>(0.0),
               geometry_.ComputeSurfaceArea(face_, model_to_world_)
                   .value_or(static_cast<visual_t>(1.0)));

  return std::numbers::pi_v<visual_t> * unit_power * surface_area;
}

}  // namespace

ReferenceCounted<Light> MakeAreaLight(const Geometry& geometry,
                                      const Matrix* model_to_world,
                                      face_t face) {
  return MakeReferenceCounted<AreaLight>(geometry, model_to_world, face);
}

}  // namespace internal
}  // namespace iris
