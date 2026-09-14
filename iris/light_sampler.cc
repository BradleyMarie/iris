#include "iris/light_sampler.h"

#include "iris/light_sample.h"
#include "iris/point.h"
#include "iris/sampler.h"

namespace iris {

LightSample* LightSampler::Sample(const Point& hit_point, Sampler& sampler) {
  return scene_.Sample(hit_point, sampler, allocator_);
}

}  // namespace iris
