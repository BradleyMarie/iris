#include "frontends/pbrt/spectrum_managers/test_spectrum_manager.h"

#include <map>

#include "iris/reference_counted.h"
#include "iris/reflector.h"
#include "iris/reflectors/mock_reflector.h"
#include "iris/spectra/mock_spectrum.h"
#include "iris/spectrum.h"
#include "pbrt_proto/v3/v3.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace spectrum_managers {

using ::iris::reflectors::MockReflector;
using ::iris::spectra::MockSpectrum;

visual_t TestSpectrumManager::ComputeLuma(visual_t r, visual_t g, visual_t b) {
  return static_cast<visual_t>(1.0);
}

ReferenceCounted<Spectrum> TestSpectrumManager::AllocateSpectrum(
    const pbrt_proto::v3::Spectrum& spectrum, visual_t* luma) {
  return MakeReferenceCounted<MockSpectrum>();
}

ReferenceCounted<Reflector> TestSpectrumManager::AllocateReflector(
    const pbrt_proto::v3::Spectrum& spectrum) {
  return MakeReferenceCounted<MockReflector>();
}

ReferenceCounted<Spectrum> TestSpectrumManager::AllocateSpectrum(
    const ReferenceCounted<Spectrum>& spectrum0,
    const ReferenceCounted<Spectrum>& spectrum1, visual_t* luma) {
  if (luma) {
    *luma = 1.0;
  }

  return MakeReferenceCounted<MockSpectrum>();
}

}  // namespace spectrum_managers
}  // namespace pbrt_frontend
}  // namespace iris
