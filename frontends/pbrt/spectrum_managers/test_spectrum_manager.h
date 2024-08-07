#ifndef _FRONTENDS_PBRT_SPECTRUM_MANAGERS_TEST_SPECTRUM_MANAGER_
#define _FRONTENDS_PBRT_SPECTRUM_MANAGERS_TEST_SPECTRUM_MANAGER_

#include "frontends/pbrt/spectrum_manager.h"

namespace iris::pbrt_frontend::spectrum_managers {

class TestSpectrumManager final : public SpectrumManager {
 public:
  visual_t ComputeLuma(const Color& color) override;

  ReferenceCounted<Spectrum> AllocateSpectrum(
      const Color& color, visual_t* luma = nullptr) override;

  ReferenceCounted<Spectrum> AllocateSpectrum(
      const std::map<visual, visual>& wavelengths,
      visual_t* luma = nullptr) override;

  ReferenceCounted<Reflector> AllocateReflector(const Color& color) override;

  ReferenceCounted<Reflector> AllocateReflector(
      const std::map<visual, visual>& wavelengths) override;

  void Clear() override;
};

}  // namespace iris::pbrt_frontend::spectrum_managers

#endif  // _FRONTENDS_PBRT_SPECTRUM_MANAGERS_TEST_SPECTRUM_MANAGER_