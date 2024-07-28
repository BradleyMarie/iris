#include "iris/light_scenes/power_light_scene.h"

#include <algorithm>

namespace iris {
namespace light_scenes {

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

  visual_t total_pdf = static_cast<visual_t>(0.0);
  for (size_t i = 0; i < scene_objects.NumLights(); i++) {
    pdf_.push_back(scene_objects.GetLight(i).Power(power_matcher));
    total_pdf += pdf_.back();
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