#ifndef _FRONTENDS_PBRT_SPECTRUM_MANAGERS_INTERNAL_COLOR_SPECTRUM_
#define _FRONTENDS_PBRT_SPECTRUM_MANAGERS_INTERNAL_COLOR_SPECTRUM_

#include "frontends/pbrt/color.h"
#include "iris/color.h"
#include "iris/float.h"
#include "iris/spectrum.h"

namespace iris::pbrt_frontend::spectrum_managers::internal {

class ColorSpectrum final : public Spectrum {
 public:
  ColorSpectrum(const iris::pbrt_frontend::Color& color) noexcept
      : values_(color.values) {}

  ColorSpectrum(const iris::Color& color) noexcept
      : values_({color.r, color.g, color.b}) {}

  visual_t Intensity(visual_t wavelength) const override;

 private:
  const std::array<visual_t, 3> values_;
};

}  // namespace iris::pbrt_frontend::spectrum_managers::internal

#endif  // _FRONTENDS_PBRT_SPECTRUM_MANAGERS_INTERNAL_COLOR_SPECTRUM_