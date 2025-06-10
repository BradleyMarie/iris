#include "iris/light_sample_allocator.h"

#include <optional>

#include "iris/float.h"
#include "iris/internal/arena.h"
#include "iris/light.h"
#include "iris/light_sample.h"

namespace iris {

LightSample& LightSampleAllocator::Allocate(const Light& light,
                                            const std::optional<visual_t>& pdf,
                                            LightSample* next) {
  return arena_.Allocate<LightSample>(next, light, pdf);
}

}  // namespace iris
