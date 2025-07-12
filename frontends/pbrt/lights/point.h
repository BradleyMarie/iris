#ifndef _FRONTENDS_PBRT_LIGHTS_POINT_
#define _FRONTENDS_PBRT_LIGHTS_POINT_

#include "frontends/pbrt/spectrum_manager.h"
#include "iris/light.h"
#include "iris/matrix.h"
#include "iris/reference_counted.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace lights {

ReferenceCounted<Light> MakePoint(
    const pbrt_proto::v3::LightSource::Point& distant,
    const Matrix& model_to_world, SpectrumManager& spectrum_manager);

}  // namespace lights
}  // namespace pbrt_frontend
}  // namespace iris

#endif  // _FRONTENDS_PBRT_LIGHTS_POINT_
