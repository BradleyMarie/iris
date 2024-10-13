#ifndef _FRONTENDS_PBRT_SPECTRUM_MANAGERS_COLOR_SPECTRUM_MANAGER_
#define _FRONTENDS_PBRT_SPECTRUM_MANAGERS_COLOR_SPECTRUM_MANAGER_

#include "frontends/pbrt/spectrum_manager.h"
#include "iris/albedo_matcher.h"
#include "iris/color_matcher.h"
#include "iris/power_matcher.h"

namespace iris {
namespace pbrt_frontend {
namespace spectrum_managers {

class ColorColorMatcher final : public ColorMatcher {
 public:
  std::array<visual_t, 3> Match(const Spectrum& spectrum) const override;
  iris::Color::Space ColorSpace() const override;
};

class ColorAlbedoMatcher : public AlbedoMatcher {
 public:
  visual_t Match(const Reflector& reflector) const override;
};

class ColorPowerMatcher : public PowerMatcher {
 public:
  visual_t Match(const Spectrum& spectrum) const override;
};

class ColorSpectrumManager final : public SpectrumManager {
 public:
  ColorSpectrumManager(bool all_spectra_are_reflective);

  visual_t ComputeLuma(const Color& color) override;

  ReferenceCounted<Spectrum> AllocateSpectrum(
      const Color& color, visual_t* luma = nullptr) override;

  ReferenceCounted<Spectrum> AllocateSpectrum(
      const std::map<visual, visual>& wavelengths,
      visual_t* luma = nullptr) override;

  ReferenceCounted<Spectrum> AllocateSpectrum(
      const ReferenceCounted<Spectrum>& spectrum0,
      const ReferenceCounted<Spectrum>& spectrum1, visual_t* luma) override;

  ReferenceCounted<Reflector> AllocateReflector(const Color& color) override;

  ReferenceCounted<Reflector> AllocateReflector(
      const std::map<visual, visual>& wavelengths) override;

  void Clear() override;

 private:
  visual_t spectral_scalar_;
};

}  // namespace spectrum_managers
}  // namespace pbrt_frontend
}  // namespace iris

#endif  // _FRONTENDS_PBRT_SPECTRUM_MANAGERS_COLOR_SPECTRUM_MANAGER_