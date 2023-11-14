#ifndef _IRIS_SPECTRA_UNIFORM_SPECTRUM_
#define _IRIS_SPECTRA_UNIFORM_SPECTRUM_

#include <map>
#include <vector>

#include "iris/float.h"
#include "iris/spectrum.h"

namespace iris {
namespace spectra {

class UniformSpectrum final : public Spectrum {
 public:
  UniformSpectrum(visual value);

  visual_t Intensity(visual_t wavelength) const override;

 private:
  visual value_;
};

}  // namespace spectra
}  // namespace iris

#endif  // _IRIS_SPECTRA_UNIFORM_SPECTRUM_