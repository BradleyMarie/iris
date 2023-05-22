#ifndef _IRIS_NORMALS_BUMP_NORMAL_MAP_
#define _IRIS_NORMALS_BUMP_NORMAL_MAP_

#include <cassert>
#include <optional>

#include "iris/float.h"
#include "iris/normal_map.h"
#include "iris/reference_countable.h"
#include "iris/reference_counted.h"
#include "iris/texture_coordinates.h"
#include "iris/textures/texture2d.h"
#include "iris/vector.h"

namespace iris {
namespace normals {

class BumpNormalMap final : public NormalMap {
 public:
  BumpNormalMap(iris::ReferenceCounted<textures::ValueTexture2D<visual>> bumps)
      : bumps_(std::move(bumps)) {
    assert(bumps_);
  }

  Vector Evaluate(const TextureCoordinates& texture_coordinates,
                  const std::optional<Differentials>& Differentials,
                  const Vector& surface_normal) const override;

 private:
  iris::ReferenceCounted<textures::ValueTexture2D<visual>> bumps_;
};

}  // namespace normals
}  // namespace iris

#endif  // _IRIS_NORMALS_BUMP_NORMAL_MAP_