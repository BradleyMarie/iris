#ifndef _IRIS_TEXTURES_SPECTRUM_TEXTURE_
#define _IRIS_TEXTURES_SPECTRUM_TEXTURE_

#include "iris/reference_countable.h"
#include "iris/spectral_allocator.h"
#include "iris/spectrum.h"
#include "iris/texture_coordinates.h"

namespace iris {
namespace textures {

class SpectrumTexture : public ReferenceCountable {
 public:
  virtual const Spectrum* Eval(const TextureCoordinates& coordinates,
                               SpectralAllocator& allocator) const = 0;
  virtual ~SpectrumTexture() = default;
};

}  // namespace textures
}  // namespace iris

#endif  // _IRIS_TEXTURES_SPECTRUM_TEXTURE_
