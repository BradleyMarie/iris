#include "iris/textures/float_texture.h"

#include "iris/reference_countable.h"
#include "iris/texture_coordinates.h"

namespace iris {
namespace textures {

bool FloatTexture::Included(const TextureCoordinates& coordinates) const {
  return Evaluate(coordinates) != static_cast<visual_t>(0.0);
}

}  // namespace textures
}  // namespace iris
