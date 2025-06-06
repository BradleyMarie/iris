#include "frontends/pbrt/spectrum_managers/internal/color_spectrum.h"

namespace iris {
namespace pbrt_frontend {
namespace spectrum_managers {
namespace internal {
namespace {

class ColorSpectrum final : public Spectrum {
 public:
  ColorSpectrum(visual r, visual g, visual b) noexcept : values_({r, g, b}) {}

  visual_t Intensity(visual_t wavelength) const override;

 private:
  std::array<visual, 3> values_;
};

visual_t ColorSpectrum::Intensity(visual_t wavelength) const {
  size_t index = static_cast<size_t>(wavelength);
  return values_[index];
}

}  // namespace

ReferenceCounted<Spectrum> MakeColorSpectrum(visual v0, visual v1, visual v2,
                                             Color::Space color_space) {
  return MakeColorSpectrum(Color(v0, v1, v2, color_space));
}

ReferenceCounted<Spectrum> MakeColorSpectrum(const Color& color) {
  Color rgb = color.ConvertTo(Color::LINEAR_SRGB);
  if (rgb.r <= static_cast<visual>(0.0) && rgb.b <= static_cast<visual>(0.0) &&
      rgb.g <= static_cast<visual>(0.0)) {
    return ReferenceCounted<Spectrum>();
  }

  return MakeReferenceCounted<ColorSpectrum>(rgb.r, rgb.g, rgb.b);
}

}  // namespace internal
}  // namespace spectrum_managers
}  // namespace pbrt_frontend
}  // namespace iris
