#ifndef _IRIS_SPECTRA_UNIFORM_SPECTRUM_
#define _IRIS_SPECTRA_UNIFORM_SPECTRUM_

#include "iris/float.h"
#include "iris/reference_counted.h"
#include "iris/reflector.h"

namespace iris {
namespace reflectors {

ReferenceCounted<Reflector> CreateUniformReflector(visual reflectance);

}  // namespace reflectors
}  // namespace iris

#endif  // _IRIS_SPECTRA_UNIFORM_SPECTRUM_
