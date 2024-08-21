#ifndef _IRIS_REFLECTORS_SAMPLED_REFLECTOR_
#define _IRIS_REFLECTORS_SAMPLED_REFLECTOR_

#include <map>

#include "iris/float.h"
#include "iris/reference_counted.h"
#include "iris/reflector.h"

namespace iris {
namespace reflectors {

ReferenceCounted<Reflector> CreateSampledReflector(
    const std::map<visual, visual>& samples);

}  // namespace reflectors
}  // namespace iris

#endif  // _IRIS_REFLECTORS_SAMPLED_REFLECTOR_