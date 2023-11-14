#ifndef _IRIS_ENVIRONMENTAL_LIGHTS_INTERNAL_DISTRIBUTION_2D_
#define _IRIS_ENVIRONMENTAL_LIGHTS_INTERNAL_DISTRIBUTION_2D_

#include <span>
#include <utility>

#include "iris/environmental_lights/internal/distribution_1d.h"

namespace iris {
namespace environmental_lights {
namespace internal {

class Distribution2D {
 public:
  Distribution2D(std::span<const visual> values,
                 std::pair<size_t, size_t> size);

  std::pair<geometric_t, geometric_t> Sample(Sampler& sampler,
                                             visual_t* pdf = nullptr,
                                             size_t* offset = nullptr) const;
  visual_t Pdf(geometric_t u, geometric_t v) const;

 private:
  Distribution2D(std::vector<Distribution1D> rows,
                 std::pair<size_t, size_t> size);

  Distribution1D row_distribution_;
  std::vector<Distribution1D> rows_;
  std::pair<size_t, size_t> size_;
};

}  // namespace internal
}  // namespace environmental_lights
}  // namespace iris

#endif  // _IRIS_ENVIRONMENTAL_LIGHTS_INTERNAL_DISTRIBUTION_2D_