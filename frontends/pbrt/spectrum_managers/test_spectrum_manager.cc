#include "frontends/pbrt/spectrum_managers/test_spectrum_manager.h"

#include "iris/reflectors/mock_reflector.h"
#include "iris/spectra/mock_spectrum.h"

namespace iris {
namespace pbrt_frontend {
namespace spectrum_managers {

using ::iris::spectra::MockSpectrum;

visual_t TestSpectrumManager::ComputeLuma(const Color& color) {
  return static_cast<visual_t>(1.0);
}

ReferenceCounted<Spectrum> TestSpectrumManager::AllocateSpectrum(
    const Color& color, visual_t* luma) {
  return MakeReferenceCounted<MockSpectrum>();
}

ReferenceCounted<Spectrum> TestSpectrumManager::AllocateSpectrum(
    const std::map<visual, visual>& wavelengths, visual_t* luma) {
  return MakeReferenceCounted<MockSpectrum>();
}

ReferenceCounted<Spectrum> TestSpectrumManager::AllocateSpectrum(
    const ReferenceCounted<Spectrum>& spectrum0,
    const ReferenceCounted<Spectrum>& spectrum1, visual_t* luma) {
  return MakeReferenceCounted<MockSpectrum>();
}

ReferenceCounted<Reflector> TestSpectrumManager::AllocateReflector(
    const Color& color) {
  return MakeReferenceCounted<iris::reflectors::MockReflector>();
}

ReferenceCounted<Reflector> TestSpectrumManager::AllocateReflector(
    const std::map<visual, visual>& wavelengths) {
  return MakeReferenceCounted<iris::reflectors::MockReflector>();
}

void TestSpectrumManager::Clear() {}

}  // namespace spectrum_managers
}  // namespace pbrt_frontend
}  // namespace iris