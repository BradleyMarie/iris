#ifndef _IRIS_NORMALS_BUMP_NORMAL_MAP_
#define _IRIS_NORMALS_BUMP_NORMAL_MAP_

#include "iris/float.h"
#include "iris/normal_map.h"
#include "iris/reference_counted.h"
#include "iris/textures/texture2d.h"

namespace iris {
namespace normal_maps {

ReferenceCounted<NormalMap> MakeBumpNormalMap(
    ReferenceCounted<textures::ValueTexture2D<visual>> bumps);

}  // namespace normal_maps
}  // namespace iris

#endif  // _IRIS_NORMALS_BUMP_NORMAL_MAP_
