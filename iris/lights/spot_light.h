#ifndef _IRIS_LIGHTS_SPOT_LIGHT_
#define _IRIS_LIGHTS_SPOT_LIGHT_

#include "iris/light.h"
#include "iris/point.h"
#include "iris/reference_counted.h"
#include "iris/spectrum.h"
#include "iris/vector.h"

namespace iris {
namespace lights {

ReferenceCounted<Light> MakeSpotLight(const Point& location,
                                      const Vector& direction,
                                      geometric cutoff_angle,
                                      geometric falloff_start_angle,
                                      ReferenceCounted<Spectrum> spectrum);

}  // namespace lights
}  // namespace iris

#endif  // _IRIS_LIGHTS_SPOT_LIGHT_
