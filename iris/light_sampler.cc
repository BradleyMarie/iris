#include "iris/light_sampler.h"

#include "iris/light_sample.h"
#include "iris/point.h"

namespace iris {

LightSample* LightSampler::Sample(const Point& hit_point) {
  return scene_.Sample(hit_point, rng_, allocator_);
}

}  // namespace iris
