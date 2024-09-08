#ifndef _FRONTENDS_PBRT_MATERIALS_MIRROR_
#define _FRONTENDS_PBRT_MATERIALS_MIRROR_

#include <memory>

#include "frontends/pbrt/materials/material_builder.h"

namespace iris {
namespace pbrt_frontend {
namespace materials {

extern const std::unique_ptr<const MaterialBuilder> g_mirror_builder;

}  // namespace materials
}  // namespace pbrt_frontend
}  // namespace iris

#endif  // _FRONTENDS_PBRT_MATERIALS_MIRROR_