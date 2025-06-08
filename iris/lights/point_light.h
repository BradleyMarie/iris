#ifndef _IRIS_LIGHTS_POINT_LIGHT_
#define _IRIS_LIGHTS_POINT_LIGHT_

#include "iris/light.h"
#include "iris/point.h"
#include "iris/reference_counted.h"
#include "iris/spectrum.h"

namespace iris {
namespace lights {

ReferenceCounted<Light> MakePointLight(const Point& location,
                                       ReferenceCounted<Spectrum> spectrum);

}  // namespace lights
}  // namespace iris

#endif  // _IRIS_LIGHTS_POINT_LIGHT_
