#ifndef _FRONTENDS_PBRT_MATERIALS_RESULT_
#define _FRONTENDS_PBRT_MATERIALS_RESULT_

#include <array>

#include "iris/material.h"
#include "iris/normal_map.h"
#include "iris/reference_counted.h"

namespace iris {
namespace pbrt_frontend {

struct MaterialResult {
  ReferenceCounted<Material> material;
  std::array<ReferenceCounted<NormalMap>, 2> bumpmaps;
};

}  // namespace pbrt_frontend
}  // namespace iris

#endif  // _FRONTENDS_PBRT_MATERIALS_RESULT_
