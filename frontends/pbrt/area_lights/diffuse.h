#ifndef _FRONTENDS_PBRT_AREA_LIGHTS_DIFFUSE_
#define _FRONTENDS_PBRT_AREA_LIGHTS_DIFFUSE_

#include <array>

#include "frontends/pbrt/spectrum_manager.h"
#include "iris/emissive_material.h"
#include "iris/reference_counted.h"
#include "pbrt_proto/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace area_lights {

std::array<ReferenceCounted<EmissiveMaterial>, 2> MakeDiffuse(
    const pbrt_proto::DiffuseAreaLightSourceV1& diffuse,
    SpectrumManager& spectrum_manager);

}  // namespace area_lights
}  // namespace pbrt_frontend
}  // namespace iris

#endif  // _FRONTENDS_PBRT_AREA_LIGHTS_DIFFUSE_
