#ifndef _IRIS_TESTING_SAMPLER_
#define _IRIS_TESTING_SAMPLER_

#include <utility>
#include <vector>

#include "iris/float.h"
#include "iris/sampler.h"

namespace iris {
namespace testing {

Sampler MakeSampler(
    std::vector<visual_t> visual_samples,
    std::vector<std::pair<geometric_t, geometric_t>> geometric_samples);

}  // namespace testing
}  // namespace iris

#endif  // _IRIS_TESTING_SAMPLER_
