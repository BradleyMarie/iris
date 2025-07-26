#ifndef _IRIS_NORMALS_BUMP_NORMAL_MAP_
#define _IRIS_NORMALS_BUMP_NORMAL_MAP_

#include "iris/float.h"
#include "iris/normal_map.h"
#include "iris/reference_counted.h"
#include "iris/textures/float_texture.h"

namespace iris {
namespace normal_maps {

ReferenceCounted<NormalMap> MakeBumpNormalMap(
    ReferenceCounted<textures::FloatTexture> bump_map);

}  // namespace normal_maps
}  // namespace iris

#endif  // _IRIS_NORMALS_BUMP_NORMAL_MAP_
