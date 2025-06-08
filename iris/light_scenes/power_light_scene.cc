#include "iris/light_scenes/power_light_scene.h"

#include <algorithm>
#include <memory>
#include <optional>
#include <vector>

#include "iris/float.h"
#include "iris/light_sample.h"
#include "iris/light_sample_allocator.h"
#include "iris/light_scene.h"
#include "iris/point.h"
#include "iris/power_matcher.h"
#include "iris/random.h"
#include "iris/scene_objects.h"

namespace iris {
namespace light_scenes {
namespace {

class PowerLightScene final : public LightScene {
 public:
  class Builder final : public LightScene::Builder {
   public:
    std::unique_ptr<LightScene> Build(
        const SceneObjects& scene_objects,
        const PowerMatcher& power_matcher) const override;
  };

  PowerLightScene(const SceneObjects& scene_objects,
                  const PowerMatcher& power_matcher) noexcept;

  LightSample* Sample(const Point& hit_point, Random& rng,
                      LightSampleAllocator& allocator) const override;

 private:
  const SceneObjects& scene_objects_;
  std::vector<visual_t> cdf_;
  std::vector<visual_t> pdf_;
};

visual_t ComputeWorldRadiusSquared(const BoundingBox& bounds) {
  Vector radius = static_cast<geometric_t>(0.5) * (bounds.upper - bounds.lower);
  return DotProduct(radius, radius);
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

}  // namespace

std::unique_ptr<LightScene::Builder> MakePowerLightSceneBuilder() {
  return std::make_unique<PowerLightScene::Builder>();
}

}  // namespace light_scenes
}  // namespace iris
