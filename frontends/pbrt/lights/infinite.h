#ifndef _FRONTENDS_PBRT_LIGHTS_INFINITE_
#define _FRONTENDS_PBRT_LIGHTS_INFINITE_

#include "frontends/pbrt/spectrum_manager.h"
#include "iris/environmental_light.h"
#include "iris/matrix.h"
#include "iris/reference_counted.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace lights {

ReferenceCounted<EnvironmentalLight> MakeInfinite(
    const pbrt_proto::v3::LightSource::Infinite& infinite,
    const std::filesystem::path& search_root, const Matrix& model_to_world,
    SpectrumManager& spectrum_manager);

}  // namespace lights
}  // namespace pbrt_frontend
}  // namespace iris

#endif  // _FRONTENDS_PBRT_LIGHTS_INFINITE_
