#include "frontends/pbrt/spectrum_managers/internal/color_reflector.h"

#include <algorithm>
#include <array>

#include "iris/color.h"
#include "iris/float.h"
#include "iris/reference_counted.h"
#include "iris/reflector.h"

namespace iris {
namespace pbrt_frontend {
namespace spectrum_managers {
namespace internal {
namespace {

class ColorReflector final : public Reflector {
 public:
  ColorReflector(visual r, visual g, visual b) noexcept : values_({r, g, b}) {}

  visual_t Reflectance(visual_t wavelength) const override;

 private:
  std::array<visual_t, 3> values_;
};

visual_t ColorReflector::Reflectance(visual_t wavelength) const {
  size_t index = static_cast<size_t>(wavelength);
  return values_[index];
}

}  // namespace

ReferenceCounted<Reflector> MakeColorReflector(visual v0, visual v1, visual v2,
                                               Color::Space color_space) {
  return MakeColorReflector(Color(v0, v1, v2, color_space));
}

ReferenceCounted<Reflector> MakeColorReflector(const Color& color) {
  Color rgb = color.ConvertTo(Color::LINEAR_SRGB);
  if (rgb.r <= static_cast<visual>(0.0) && rgb.g <= static_cast<visual>(0.0) &&
      rgb.b <= static_cast<visual>(0.0)) {
    return ReferenceCounted<Reflector>();
  }

  return MakeReferenceCounted<ColorReflector>(
      std::clamp(rgb.r, static_cast<visual>(0.0), static_cast<visual>(1.0)),
      std::clamp(rgb.g, static_cast<visual>(0.0), static_cast<visual>(1.0)),
      std::clamp(rgb.b, static_cast<visual>(0.0), static_cast<visual>(1.0)));
}

}  // namespace internal
}  // namespace spectrum_managers
}  // namespace pbrt_frontend
}  // namespace iris
