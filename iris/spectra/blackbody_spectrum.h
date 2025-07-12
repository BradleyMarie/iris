#ifndef _IRIS_SPECTRA_BLACKBODY_SPECTRUM_
#define _IRIS_SPECTRA_BLACKBODY_SPECTRUM_

#include "iris/float.h"
#include "iris/reference_counted.h"
#include "iris/spectrum.h"

namespace iris {
namespace spectra {

ReferenceCounted<Spectrum> MakeBlackbodySpectrum(visual temperature_kelvin);

ReferenceCounted<Spectrum> MakeScaledBlackbodySpectrum(
    visual temperature_kelvin, visual peak_intensity);

}  // namespace spectra
}  // namespace iris

#endif  // _IRIS_SPECTRA_BLACKBODY_SPECTRUM_
