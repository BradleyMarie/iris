#include "iris/textures/reflector_texture.h"

#include "iris/reference_countable.h"
#include "iris/reflector.h"
#include "iris/spectral_allocator.h"
#include "iris/spectrum.h"
#include "iris/texture_coordinates.h"

namespace iris {
namespace textures {
namespace {

class WhiteSpectrum : public Spectrum {
 public:
  visual_t Intensity(visual_t wavelength) const override {
    return static_cast<visual_t>(1.0);
  }
} kUniformSpectrum;

}  // namespace

const Spectrum* ReflectorTexture::Eval(const TextureCoordinates& coordinates,
                                       SpectralAllocator& allocator) const {
  const Reflector* reflector = Evaluate(coordinates, allocator);
  return allocator.Reflect(&kUniformSpectrum, reflector);
}

}  // namespace textures
}  // namespace iris
