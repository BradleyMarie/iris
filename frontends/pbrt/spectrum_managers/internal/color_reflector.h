#ifndef _FRONTENDS_PBRT_SPECTRUM_MANAGERS_INTERNAL_COLOR_REFLECTOR_
#define _FRONTENDS_PBRT_SPECTRUM_MANAGERS_INTERNAL_COLOR_REFLECTOR_

#include "iris/color.h"
#include "iris/float.h"
#include "iris/reference_counted.h"
#include "iris/reflector.h"

namespace iris {
namespace pbrt_frontend {
namespace spectrum_managers {
namespace internal {

ReferenceCounted<Reflector> MakeColorReflector(visual v0, visual v1, visual v2,
                                               Color::Space color_space);
ReferenceCounted<Reflector> MakeColorReflector(const Color& color);

}  // namespace internal
}  // namespace spectrum_managers
}  // namespace pbrt_frontend
}  // namespace iris

#endif  // _FRONTENDS_PBRT_SPECTRUM_MANAGERS_INTERNAL_COLOR_REFLECTOR_
