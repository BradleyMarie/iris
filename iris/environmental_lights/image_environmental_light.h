#ifndef _IRIS_ENVIRONMENTAL_LIGHTS_IMAGE_ENVIRONMENTAL_LIGHT_
#define _IRIS_ENVIRONMENTAL_LIGHTS_IMAGE_ENVIRONMENTAL_LIGHT_

#include <utility>
#include <vector>

#include "iris/environmental_light.h"
#include "iris/float.h"
#include "iris/matrix.h"
#include "iris/reference_counted.h"
#include "iris/spectrum.h"

namespace iris {
namespace environmental_lights {

ReferenceCounted<EnvironmentalLight> MakeImageEnvironmentalLight(
    const std::vector<std::pair<ReferenceCounted<Spectrum>, visual>>&
        spectra_and_luma,
    std::pair<size_t, size_t> dimensions, const Matrix& model_to_world);

}  // namespace environmental_lights
}  // namespace iris

#endif  // _IRIS_ENVIRONMENTAL_LIGHTS_IMAGE_ENVIRONMENTAL_LIGHT_
