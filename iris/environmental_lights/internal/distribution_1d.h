#ifndef _IRIS_ENVIRONMENTAL_LIGHTS_INTERNAL_DISTRIBUTION_1D_
#define _IRIS_ENVIRONMENTAL_LIGHTS_INTERNAL_DISTRIBUTION_1D_

#include <span>
#include <vector>

#include "iris/float.h"
#include "iris/sampler.h"

namespace iris {
namespace environmental_lights {
namespace internal {

class Distribution1D {
 public:
  Distribution1D(std::span<const visual> values);
  Distribution1D(std::vector<visual> values);

  geometric_t SampleContinuous(Sampler& sampler, visual_t* pdf = nullptr,
                               size_t* offset = nullptr) const;
  visual_t PdfContinuous(geometric_t sample) const;

  size_t SampleDiscrete(Sampler& sampler, visual_t* pdf = nullptr) const;
  visual_t PdfDiscrete(size_t sample) const;

  visual_t Average() const { return average_value_; }

 private:
  std::vector<visual> values_;
  std::vector<visual> cdf_;
  visual average_value_;
};

}  // namespace internal
}  // namespace environmental_lights
}  // namespace iris

#endif  // _IRIS_ENVIRONMENTAL_LIGHTS_INTERNAL_DISTRIBUTION_1D_