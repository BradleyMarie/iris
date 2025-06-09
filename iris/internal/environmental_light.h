#ifndef _IRIS_INTERNAL_ENVIRONMENTAL_LIGHT_
#define _IRIS_INTERNAL_ENVIRONMENTAL_LIGHT_

#include "iris/environmental_light.h"
#include "iris/light.h"
#include "iris/reference_counted.h"

namespace iris {
namespace internal {

ReferenceCounted<Light> MakeEnvironmentalLight(const EnvironmentalLight& light);

}  // namespace internal
}  // namespace iris

#endif  // _IRIS_INTERNAL_ENVIRONMENTAL_LIGHT_
