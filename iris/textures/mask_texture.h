#ifndef _IRIS_TEXTURES_MASK_TEXTURE_
#define _IRIS_TEXTURES_MASK_TEXTURE_

#include "iris/reference_countable.h"
#include "iris/texture_coordinates.h"

namespace iris {
namespace textures {

class MaskTexture : public ReferenceCountable {
 public:
  virtual bool Included(const TextureCoordinates& coordinates) const = 0;
  virtual ~MaskTexture() = default;
};

}  // namespace textures
}  // namespace iris

#endif  // _IRIS_TEXTURES_MASK_TEXTURE_
