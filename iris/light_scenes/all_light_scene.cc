#include "iris/light_scenes/all_light_scene.h"

#include <memory>
#include <optional>

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

class AllLightScene final : public LightScene {
 public:
  class Builder final : public LightScene::Builder {
   public:
    std::unique_ptr<LightScene> Build(
        const SceneObjects& scene_objects,
        const PowerMatcher& power_matcher) const override;
  };

  AllLightScene(const SceneObjects& scene_objects) noexcept
      : scene_objects_(scene_objects) {}

  LightSample* Sample(const Point& hit_point, Random& rng,
                      LightSampleAllocator& allocator) const override;

 private:
  const SceneObjects& scene_objects_;
};

std::unique_ptr<LightScene> AllLightScene::Builder::Build(
    const SceneObjects& scene_objects,
    const PowerMatcher& power_matcher) const {
  return std::make_unique<AllLightScene>(scene_objects);
}

LightSample* AllLightScene::Sample(const Point& hit_point, Random& rng,
                                   LightSampleAllocator& allocator) const {
  LightSample* next = nullptr;
  for (size_t i = 0; i < scene_objects_.NumLights(); i++) {
    next = &allocator.Allocate(scene_objects_.GetLight(i), std::nullopt, next);
  }
  return next;
}

}  // namespace

std::unique_ptr<LightScene::Builder> MakeAllLightSceneBuilder() {
  return std::make_unique<AllLightScene::Builder>();
}

}  // namespace light_scenes
}  // namespace iris
