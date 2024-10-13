#include "iris/light_scenes/power_light_scene.h"

#include <algorithm>

namespace iris {
namespace light_scenes {
namespace {

visual_t ComputeWorldRadiusSquared(const BoundingBox& bounds) {
  Vector radius = static_cast<geometric_t>(0.5) * (bounds.upper - bounds.lower);
  return DotProduct(radius, radius);
}

}  // namespace

std::unique_ptr<LightScene::Builder> PowerLightScene::Builder::Create() {
  return std::make_unique<PowerLightScene::Builder>();
}

std::unique_ptr<LightScene> PowerLightScene::Builder::Build(
    const SceneObjects& scene_objects,
    const PowerMatcher& power_matcher) const {
  return std::make_unique<PowerLightScene>(scene_objects, power_matcher);
}

PowerLightScene::PowerLightScene(const SceneObjects& scene_objects,
                                 const PowerMatcher& power_matcher) noexcept
    : scene_objects_(scene_objects) {
  if (scene_objects.NumLights() < 2) {
    return;
  }

  visual_t world_radius_squared =
      ComputeWorldRadiusSquared(scene_objects.GetBounds());

  visual_t total_pdf = static_cast<visual_t>(0.0);
  for (size_t i = 0; i < scene_objects.NumLights(); i++) {
    visual_t power =
        scene_objects.GetLight(i).Power(power_matcher, world_radius_squared);
    total_pdf += power;
    pdf_.push_back(power);
  }

  visual_t running_pdf = static_cast<visual_t>(0.0);
  for (size_t i = 0; i < pdf_.size(); i++) {
    running_pdf += pdf_[i];
    pdf_[i] /= total_pdf;
    cdf_.push_back(running_pdf / total_pdf);
  }

  cdf_.back() = static_cast<visual_t>(1.0);
}

LightSample* PowerLightScene::Sample(const Point& hit_point, Random& rng,
                                     LightSampleAllocator& allocator) const {
  if (scene_objects_.NumLights() == 0) {
    return nullptr;
  }

  if (scene_objects_.NumLights() == 1) {
    return &allocator.Allocate(scene_objects_.GetLight(0), std::nullopt);
  }

  auto iter = std::upper_bound(cdf_.begin(), cdf_.end(), rng.NextVisual());
  size_t index = iter - cdf_.begin();

  return &allocator.Allocate(scene_objects_.GetLight(index), pdf_[index]);
}

}  // namespace light_scenes
}  // namespace iris