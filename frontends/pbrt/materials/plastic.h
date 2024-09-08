#ifndef _FRONTENDS_PBRT_MATERIALS_PLASTIC_
#define _FRONTENDS_PBRT_MATERIALS_PLASTIC_

#include <memory>

#include "frontends/pbrt/materials/material_builder.h"

namespace iris {
namespace pbrt_frontend {
namespace materials {

extern const std::unique_ptr<const MaterialBuilder> g_plastic_builder;

}  // namespace materials
}  // namespace pbrt_frontend
}  // namespace iris

#endif  // _FRONTENDS_PBRT_MATERIALS_PLASTIC_