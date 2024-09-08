#ifndef _FRONTENDS_PBRT_MATERIALS_MATTE_
#define _FRONTENDS_PBRT_MATERIALS_MATTE_

#include <memory>

#include "frontends/pbrt/materials/material_builder.h"

namespace iris {
namespace pbrt_frontend {
namespace materials {

extern const std::unique_ptr<const MaterialBuilder> g_matte_builder;

}  // namespace materials
}  // namespace pbrt_frontend
}  // namespace iris

#endif  // _FRONTENDS_PBRT_MATERIALS_MATTE_