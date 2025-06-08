#include "iris/light_scenes/one_light_scene.h"

#include <memory>
#include <optional>

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

class OneLightScene final : public LightScene {
 public:
  class Builder final : public LightScene::Builder {
   public:
    std::unique_ptr<LightScene> Build(
        const SceneObjects& scene_objects,
        const PowerMatcher& power_matcher) const override;
  };

  OneLightScene(const SceneObjects& scene_objects) noexcept;

  LightSample* Sample(const Point& hit_point, Random& rng,
                      LightSampleAllocator& allocator) const override;

 private:
  const SceneObjects& scene_objects_;
  std::optional<visual_t> pdf_;
};

std::unique_ptr<LightScene> OneLightScene::Builder::Build(
    const SceneObjects& scene_objects,
    const PowerMatcher& power_matcher) const {
  return std::make_unique<OneLightScene>(scene_objects);
}

OneLightScene::OneLightScene(const SceneObjects& scene_objects) noexcept
    : scene_objects_(scene_objects),
      pdf_((scene_objects.NumLights() > 1)
               ? std::optional<visual_t>(static_cast<visual_t>(
                     static_cast<intermediate_t>(1.0) /
                     static_cast<intermediate_t>(scene_objects.NumLights())))
               : std::nullopt) {}

LightSample* OneLightScene::Sample(const Point& hit_point, Random& rng,
                                   LightSampleAllocator& allocator) const {
  if (scene_objects_.NumLights() == 0) {
    return nullptr;
  }

  size_t index = 0;
  if (scene_objects_.NumLights() != 1) {
    index = rng.NextIndex(scene_objects_.NumLights());
  }

  return &allocator.Allocate(scene_objects_.GetLight(index), pdf_);
}

}  // namespace

std::unique_ptr<LightScene::Builder> MakeOneLightSceneBuilder() {
  return std::make_unique<OneLightScene::Builder>();
}

}  // namespace light_scenes
}  // namespace iris
