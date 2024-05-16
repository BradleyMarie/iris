#ifndef _FRONTENDS_PBRT_SPECTRUM_MANAGERS_COLOR_SPECTRUM_MANAGER_
#define _FRONTENDS_PBRT_SPECTRUM_MANAGERS_COLOR_SPECTRUM_MANAGER_

#include "frontends/pbrt/spectrum_manager.h"
#include "iris/color_matcher.h"

namespace iris::pbrt_frontend::spectrum_managers {

class ColorColorMatcher final : public ColorMatcher {
 public:
  std::array<visual_t, 3> Match(const Spectrum& spectrum) const override;
  iris::Color::Space ColorSpace() const override;
};

class ColorSpectrumManager final : public SpectrumManager {
 public:
  ColorSpectrumManager(bool all_spectra_are_reflective);

  ReferenceCounted<Spectrum> AllocateSpectrum(
      const Color& color, visual_t* luma = nullptr) override;

  ReferenceCounted<Spectrum> AllocateSpectrum(
      const std::map<visual, visual>& wavelengths,
      visual_t* luma = nullptr) override;

  ReferenceCounted<Reflector> AllocateReflector(const Color& color) override;

  ReferenceCounted<Reflector> AllocateReflector(
      const std::map<visual, visual>& wavelengths) override;

  void Clear() override;

 private:
  visual_t spectral_scalar_;
};

}  // namespace iris::pbrt_frontend::spectrum_managers

#endif  // _FRONTENDS_PBRT_SPECTRUM_MANAGERS_COLOR_SPECTRUM_MANAGER_