#ifndef _IRIS_TEXTURES_REFLECTOR_TEXTURE_
#define _IRIS_TEXTURES_REFLECTOR_TEXTURE_

#include "iris/reference_countable.h"
#include "iris/reflector.h"
#include "iris/spectral_allocator.h"
#include "iris/spectrum.h"
#include "iris/texture_coordinates.h"
#include "iris/textures/spectrum_texture.h"

namespace iris {
namespace textures {

class ReflectorTexture : public SpectrumTexture {
 public:
  virtual const Reflector* Evaluate(const TextureCoordinates& coordinates,
                                    SpectralAllocator& allocator) const = 0;
  virtual ~ReflectorTexture() = default;

 private:
  const Spectrum* Eval(const TextureCoordinates& coordinates,
                       SpectralAllocator& allocator) const final override;
};

}  // namespace textures
}  // namespace iris

#endif  // _IRIS_TEXTURES_REFLECTOR_TEXTURE_
