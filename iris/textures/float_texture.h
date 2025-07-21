#ifndef _IRIS_TEXTURES_FLOAT_TEXTURE_
#define _IRIS_TEXTURES_FLOAT_TEXTURE_

#include "iris/float.h"
#include "iris/reference_countable.h"
#include "iris/texture_coordinates.h"
#include "iris/textures/mask_texture.h"

namespace iris {
namespace textures {

class FloatTexture : public MaskTexture {
 public:
  virtual visual_t Evaluate(const TextureCoordinates& coordinates) const = 0;
  virtual ~FloatTexture() = default;

 private:
  bool Included(const TextureCoordinates& coordinates) const final override;
};

}  // namespace textures
}  // namespace iris

#endif  // _IRIS_TEXTURES_FLOAT_TEXTURE_
