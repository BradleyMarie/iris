#ifndef _FRONTENDS_PBRT_SPECTRUM_MANAGERS_INTERNAL_COLOR_REFLECTOR_
#define _FRONTENDS_PBRT_SPECTRUM_MANAGERS_INTERNAL_COLOR_REFLECTOR_

#include <algorithm>

#include "frontends/pbrt/color.h"
#include "iris/color.h"
#include "iris/float.h"
#include "iris/reflector.h"

namespace iris::pbrt_frontend::spectrum_managers::internal {

class ColorReflector final : public Reflector {
 public:
  ColorReflector(const iris::pbrt_frontend::Color& color) noexcept
      : values_(color.values) {
    assert(color.space == iris::pbrt_frontend::Color::RGB);
    assert(values_[0] <= 1.0);
    assert(values_[1] <= 1.0);
    assert(values_[2] <= 1.0);
  }

  ColorReflector(const iris::Color& color) noexcept
      : values_({color.r, color.g, color.b}) {
    assert(color.space == iris::Color::LINEAR_SRGB);
    assert(values_[0] <= 1.0);
    assert(values_[1] <= 1.0);
    assert(values_[2] <= 1.0);
  }

  visual_t Reflectance(visual_t wavelength) const override;

 private:
  const std::array<visual_t, 3> values_;
};

}  // namespace iris::pbrt_frontend::spectrum_managers::internal

#endif  // _FRONTENDS_PBRT_SPECTRUM_MANAGERS_INTERNAL_COLOR_REFLECTOR_