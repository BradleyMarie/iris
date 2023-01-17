#ifndef _FRONTENDS_PBRT_SPECTRUM_MANAGERS_LINEAR_RGB_SPECTRUM_MANAGER_
#define _FRONTENDS_PBRT_SPECTRUM_MANAGERS_LINEAR_RGB_SPECTRUM_MANAGER_

#include "frontends/pbrt/spectrum_manager.h"
#include "iris/color_matcher.h"

namespace iris::pbrt_frontend::spectrum_managers {

class LinearRGBColorMatcher final : public ColorMatcher {
 public:
  std::array<visual_t, 3> Match(const Spectrum& spectrum) const override;
  iris::Color::Space ColorSpace() const override;
};

class LinearRGBSpectrumManager final : public SpectrumManager {
 public:
  ReferenceCounted<Spectrum> AllocateSpectrum(const Color& color) override;

  ReferenceCounted<Spectrum> AllocateSpectrum(
      const std::map<visual, visual>& wavelengths) override;

  ReferenceCounted<Reflector> AllocateReflector(const Color& color) override;

  ReferenceCounted<Reflector> AllocateReflector(
      const std::map<visual, visual>& wavelengths) override;

  void Clear() override;
};

}  // namespace iris::pbrt_frontend::spectrum_managers

#endif  // _FRONTENDS_PBRT_SPECTRUM_MANAGERS_LINEAR_RGB_SPECTRUM_MANAGER_