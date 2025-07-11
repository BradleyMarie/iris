#ifndef _IRIS_LIGHT_SCENE_
#define _IRIS_LIGHT_SCENE_

#include <memory>

#include "iris/light_sample.h"
#include "iris/light_sample_allocator.h"
#include "iris/point.h"
#include "iris/power_matcher.h"
#include "iris/random.h"

namespace iris {

// Forward Declaration
class SceneObjects;

class LightScene {
 public:
  class Builder {
   public:
    virtual std::unique_ptr<LightScene> Build(
        const SceneObjects& scene_objects,
        const PowerMatcher& power_matcher) const = 0;
    virtual ~Builder() = default;
  };

  virtual LightSample* Sample(const Point& hit_point, Random& rng,
                              LightSampleAllocator& allocator) const = 0;
  virtual ~LightScene() {}
};

}  // namespace iris

#endif  // _IRIS_LIGHT_SCENE_
