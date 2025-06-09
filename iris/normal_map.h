#ifndef _IRIS_NORMAL_MAP_
#define _IRIS_NORMAL_MAP_

#include <optional>
#include <utility>

#include "iris/float.h"
#include "iris/reference_countable.h"
#include "iris/texture_coordinates.h"
#include "iris/vector.h"

namespace iris {

class NormalMap : public ReferenceCountable {
 public:
  struct Differentials {
    const enum Type {
      DX_DY = 0,
      DU_DV = 1,
    } type;
    const std::pair<Vector, Vector> dp;
  };

  virtual Vector Evaluate(const TextureCoordinates& texture_coordinates,
                          const std::optional<Differentials>& differentials,
                          const Vector& surface_normal) const = 0;
  virtual ~NormalMap() {}
};

}  // namespace iris

#endif  // _IRIS_NORMAL_MAP_
