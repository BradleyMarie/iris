#include "frontends/pbrt/spectrum_managers/test_spectrum_manager.h"

#include "iris/reflectors/mock_reflector.h"
#include "iris/spectra/mock_spectrum.h"

namespace iris::pbrt_frontend::spectrum_managers {

visual_t TestSpectrumManager::ComputeLuma(const Color& color) {
  return static_cast<visual_t>(1.0);
}

ReferenceCounted<Spectrum> TestSpectrumManager::AllocateSpectrum(
    const Color& color, visual_t* luma) {
  return iris::MakeReferenceCounted<iris::spectra::MockSpectrum>();
}

ReferenceCounted<Spectrum> TestSpectrumManager::AllocateSpectrum(
    const std::map<visual, visual>& wavelengths, visual_t* luma) {
  return iris::MakeReferenceCounted<iris::spectra::MockSpectrum>();
}

ReferenceCounted<Reflector> TestSpectrumManager::AllocateReflector(
    const Color& color) {
  return iris::MakeReferenceCounted<iris::reflectors::MockReflector>();
}

ReferenceCounted<Reflector> TestSpectrumManager::AllocateReflector(
    const std::map<visual, visual>& wavelengths) {
  return iris::MakeReferenceCounted<iris::reflectors::MockReflector>();
}

void TestSpectrumManager::Clear() {}

}  // namespace iris::pbrt_frontend::spectrum_managers