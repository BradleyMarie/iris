#ifndef _IRIS_SPECTRA_SAMPLED_SPECTRUM_
#define _IRIS_SPECTRA_SAMPLED_SPECTRUM_

#include <map>

#include "iris/float.h"
#include "iris/reference_counted.h"
#include "iris/spectrum.h"

namespace iris {
namespace spectra {

ReferenceCounted<Spectrum> MakeSampledSpectrum(
    const std::map<visual, visual>& samples);

}  // namespace spectra
}  // namespace iris

#endif  // _IRIS_SPECTRA_SAMPLED_SPECTRUM_
