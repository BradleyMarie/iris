#ifndef _FRONTENDS_PBRT_SPECTRUM_MANAGER_
#define _FRONTENDS_PBRT_SPECTRUM_MANAGER_

#include <map>

#include "frontends/pbrt/color.h"
#include "iris/reference_counted.h"
#include "iris/reflector.h"
#include "iris/spectrum.h"

namespace iris::pbrt_frontend {

class SpectrumManager {
 public:
  virtual visual_t ComputeLuma(const Color& color) = 0;

  virtual ReferenceCounted<Spectrum> AllocateSpectrum(const Color& color,
                                                      visual_t* luma) = 0;

  virtual ReferenceCounted<Spectrum> AllocateSpectrum(
      const std::map<visual, visual>& wavelengths, visual_t* luma) = 0;

  virtual ReferenceCounted<Reflector> AllocateReflector(const Color& color) = 0;

  virtual ReferenceCounted<Reflector> AllocateReflector(
      const std::map<visual, visual>& wavelengths) = 0;

  virtual void Clear() = 0;

  // These are here to facilitate the implementation of sperctral parsing,
  // otherwise it would be sufficient to just set luma to nullptr by default.
  ReferenceCounted<Spectrum> AllocateSpectrum(const Color& color) {
    return AllocateSpectrum(color, nullptr);
  }

  ReferenceCounted<Spectrum> AllocateSpectrum(
      const std::map<visual, visual>& wavelengths) {
    return AllocateSpectrum(wavelengths, nullptr);
  }
};

}  // namespace iris::pbrt_frontend

#endif  // _FRONTENDS_PBRT_SPECTRUM_MANAGER_