#ifndef _IRIS_INTERNAL_AREA_LIGHT_
#define _IRIS_INTERNAL_AREA_LIGHT_

#include "iris/geometry.h"
#include "iris/integer.h"
#include "iris/light.h"
#include "iris/matrix.h"
#include "iris/reference_counted.h"

namespace iris {
namespace internal {

ReferenceCounted<Light> MakeAreaLight(ReferenceCounted<Geometry> geometry,
                                      const Matrix* model_to_world, face_t face,
                                      bool invisible);

}  // namespace internal
}  // namespace iris

#endif  // _IRIS_INTERNAL_AREA_LIGHT_
