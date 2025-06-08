#ifndef _IRIS_LIGHTS_DIRECTIONAL_LIGHT_
#define _IRIS_LIGHTS_DIRECTIONAL_LIGHT_

#include "iris/light.h"
#include "iris/reference_counted.h"
#include "iris/spectrum.h"
#include "iris/vector.h"

namespace iris {
namespace lights {

ReferenceCounted<Light> MakeDirectionalLight(
    const Vector& to_light, ReferenceCounted<Spectrum> spectrum);

}  // namespace lights
}  // namespace iris

#endif  // _IRIS_LIGHTS_DIRECTIONAL_LIGHT_
