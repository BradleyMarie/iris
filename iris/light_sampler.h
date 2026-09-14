#ifndef _IRIS_LIGHT_SAMPLER_
#define _IRIS_LIGHT_SAMPLER_

#include "iris/light_sample.h"
#include "iris/light_sample_allocator.h"
#include "iris/light_scene.h"
#include "iris/point.h"
#include "iris/sampler.h"

namespace iris {

class LightSampler final {
 public:
  LightSampler(const LightScene& scene,
               LightSampleAllocator& allocator) noexcept
      : scene_(scene), allocator_(allocator) {}

  LightSample* Sample(const Point& hit_point, Sampler& sampler);

 private:
  LightSampler(const LightSampler&) = delete;
  LightSampler& operator=(const LightSampler&) = delete;

  const LightScene& scene_;
  LightSampleAllocator& allocator_;
};

}  // namespace iris

#endif  // _IRIS_LIGHT_SAMPLER_
