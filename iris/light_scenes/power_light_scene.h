#ifndef _IRIS_LIGHT_SCENES_POWER_LIGHT_SCENE_
#define _IRIS_LIGHT_SCENES_POWER_LIGHT_SCENE_

#include <memory>
#include <optional>
#include <vector>

#include "iris/light_sample.h"
#include "iris/light_sample_allocator.h"
#include "iris/light_scene.h"
#include "iris/point.h"
#include "iris/power_matcher.h"
#include "iris/random.h"
#include "iris/scene_objects.h"

namespace iris {
namespace light_scenes {

class PowerLightScene final : public LightScene {
 public:
  class Builder final : public LightScene::Builder {
   public:
    static std::unique_ptr<LightScene::Builder> Create();
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

}  // namespace light_scenes
}  // namespace iris

#endif  // _IRIS_LIGHT_SCENES_POWER_LIGHT_SCENE_