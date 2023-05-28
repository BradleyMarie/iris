#include "iris/internal/area_light.h"

#include "iris/internal/visibility_tester.h"

namespace iris::internal {

AreaLight::AreaLight(const Geometry& geometry, const Matrix* model_to_world,
                     face_t face) noexcept
    : geometry_(geometry), model_to_world_(model_to_world), face_(face) {}

std::optional<Light::SampleResult> AreaLight::Sample(
    const Point& hit_point, Sampler sampler, iris::VisibilityTester& tester,
    SpectralAllocator& allocator) const {
  auto sample = geometry_.SampleSurfaceArea(face_, sampler);
  if (!sample) {
    return std::nullopt;
  }

  auto world_sample_point =
      model_to_world_ ? model_to_world_->Multiply(*sample) : *sample;

  auto to_light = Normalize(world_sample_point - hit_point);

  visual_t pdf;
  auto* emission = Emission(Ray(hit_point, to_light), tester, allocator, &pdf);
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