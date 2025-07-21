#include "iris/textures/test_util.h"

#include "iris/reference_counted.h"
#include "iris/reflector.h"
#include "iris/spectral_allocator.h"
#include "iris/texture_coordinates.h"
#include "iris/textures/float_texture.h"
#include "iris/textures/reflector_texture.h"

namespace iris {
namespace textures {
namespace {

class ZeroTexture : public FloatTexture {
 public:
  visual_t Evaluate(const TextureCoordinates& coordinates) const {
    return static_cast<visual_t>(0.0);
  }
};

class BlackTexture : public ReflectorTexture {
 public:
  const Reflector* Evaluate(const TextureCoordinates& coordinates,
                            SpectralAllocator& allocator) const {
    return nullptr;
  }
};

}  // namespace

ReferenceCounted<FloatTexture> MakeZeroTexture() {
  return MakeReferenceCounted<ZeroTexture>();
}

ReferenceCounted<ReflectorTexture> MakeBlackTexture() {
  return MakeReferenceCounted<BlackTexture>();
}

}  // namespace textures
}  // namespace iris
