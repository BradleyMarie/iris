#include "frontends/pbrt/spectrum_managers/color_spectrum_manager.h"

#include "frontends/pbrt/spectrum_managers/internal/color_reflector.h"
#include "frontends/pbrt/spectrum_managers/internal/color_spectrum.h"
#include "iris/color_matchers/cie_color_matcher.h"
#include "iris/reflectors/sampled_reflector.h"
#include "iris/spectra/sampled_spectrum.h"

namespace iris {
namespace pbrt_frontend {
namespace spectrum_managers {
namespace {

using ::iris::pbrt_frontend::spectrum_managers::internal::ColorReflector;
using ::iris::pbrt_frontend::spectrum_managers::internal::ColorSpectrum;

static iris::color_matchers::CieColorMatcher g_color_matcher;

visual_t ComputeSpectralScalar(bool all_spectra_are_reflective) {
  if (!all_spectra_are_reflective) {
    return static_cast<visual_t>(1.0);
  }

  std::map<visual, visual> wavelengths = {
      {static_cast<visual>(1.0), static_cast<visual>(1.0)}};
  iris::spectra::SampledSpectrum sampled_spectrum(wavelengths);
  return static_cast<visual_t>(1.0) /
         g_color_matcher.Match(sampled_spectrum)[1];
}

Color ToRGB(const Color& color) {
  if (color.space == Color::RGB) {
    return color;
  }

  iris::Color xyz_color(color.values[0], color.values[1], color.values[2],
                        iris::Color::CIE_XYZ);
  iris::Color rgb_color = xyz_color.ConvertTo(iris::Color::LINEAR_SRGB);

  return Color({rgb_color.r, rgb_color.g, rgb_color.b}, Color::RGB);
}

visual_t ToLuma(const Color& color) {
  if (color.space == Color::XYZ) {
    return color.values[1];
  }

  iris::Color rgb_color(color.values[0], color.values[1], color.values[2],
                        iris::Color::LINEAR_SRGB);
  return rgb_color.Luma();
}

};  // namespace

std::array<visual_t, 3> ColorColorMatcher::Match(
    const Spectrum& spectrum) const {
  return {spectrum.Intensity(static_cast<visual_t>(0.5)),
          spectrum.Intensity(static_cast<visual_t>(1.5)),
          spectrum.Intensity(static_cast<visual_t>(2.5))};
}

iris::Color::Space ColorColorMatcher::ColorSpace() const {
  return iris::Color::LINEAR_SRGB;
}

visual_t ColorAlbedoMatcher::Match(const Reflector& reflector) const {
  iris::Color color(reflector.Reflectance(static_cast<visual_t>(0.5)),
                    reflector.Reflectance(static_cast<visual_t>(1.5)),
                    reflector.Reflectance(static_cast<visual_t>(2.5)),
                    iris::Color::LINEAR_SRGB);
  return color.Luma();
}

visual_t ColorPowerMatcher::Match(const Spectrum& spectrum) const {
  iris::Color color(spectrum.Intensity(static_cast<visual_t>(0.5)),
                    spectrum.Intensity(static_cast<visual_t>(1.5)),
                    spectrum.Intensity(static_cast<visual_t>(2.5)),
                    iris::Color::LINEAR_SRGB);
  return color.Luma();
}

ColorSpectrumManager::ColorSpectrumManager(bool all_spectra_are_reflective)
    : spectral_scalar_(ComputeSpectralScalar(all_spectra_are_reflective)) {}

visual_t ColorSpectrumManager::ComputeLuma(const Color& color) {
  return ToLuma(color);
}

ReferenceCounted<Spectrum> ColorSpectrumManager::AllocateSpectrum(
    const Color& color, visual_t* luma) {
  if (luma) {
    *luma = ToLuma(color);
  }

  Color rgb_color = ToRGB(color);
  if (rgb_color.values[0] <= static_cast<visual_t>(0.0) &&
      rgb_color.values[1] <= static_cast<visual_t>(0.0) &&
      rgb_color.values[2] <= static_cast<visual_t>(0.0)) {
    return ReferenceCounted<Spectrum>();
  }

  return MakeReferenceCounted<ColorSpectrum>(rgb_color);
}

ReferenceCounted<Spectrum> ColorSpectrumManager::AllocateSpectrum(
    const std::map<visual, visual>& wavelengths, visual_t* luma) {
  iris::spectra::SampledSpectrum sampled_spectrum(wavelengths);

  auto values = g_color_matcher.Match(sampled_spectrum);
  iris::Color xyz_color(
      values[0] * spectral_scalar_, values[1] * spectral_scalar_,
      values[2] * spectral_scalar_, g_color_matcher.ColorSpace());

  if (luma) {
    *luma = xyz_color.y;
  }

  auto rgb_color = xyz_color.ConvertTo(iris::Color::LINEAR_SRGB);
  if (rgb_color.r <= static_cast<visual>(0.0) &&
      rgb_color.g <= static_cast<visual>(0.0) &&
      rgb_color.b <= static_cast<visual>(0.0)) {
    return ReferenceCounted<Spectrum>();
  }

  return MakeReferenceCounted<ColorSpectrum>(rgb_color);
}

ReferenceCounted<Spectrum> ColorSpectrumManager::AllocateSpectrum(
    const ReferenceCounted<Spectrum>& spectrum0,
    const ReferenceCounted<Spectrum>& spectrum1, visual_t* luma) {
  if (!spectrum0) {
    if (luma) {
      *luma = static_cast<visual_t>(0.0);
    }

    return spectrum0;
  }

  if (!spectrum1) {
    if (luma) {
      *luma = static_cast<visual_t>(0.0);
    }

    return spectrum1;
  }

  visual_t spectrum0_r = spectrum0->Intensity(static_cast<visual_t>(0.5));
  visual_t spectrum0_g = spectrum0->Intensity(static_cast<visual_t>(1.5));
  visual_t spectrum0_b = spectrum0->Intensity(static_cast<visual_t>(2.5));

  visual_t spectrum1_r = spectrum1->Intensity(static_cast<visual_t>(0.5));
  visual_t spectrum1_g = spectrum1->Intensity(static_cast<visual_t>(1.5));
  visual_t spectrum1_b = spectrum1->Intensity(static_cast<visual_t>(2.5));

  visual r = spectrum0_r * spectrum1_r;
  visual g = spectrum0_g * spectrum1_g;
  visual b = spectrum0_b * spectrum1_b;
  Color rgb_color({r, g, b}, Color::RGB);

  if (luma) {
    *luma = ToLuma(rgb_color);
  }

  if (spectrum0_r == static_cast<visual_t>(1.0) &&
      spectrum0_g == static_cast<visual_t>(1.0) &&
      spectrum0_b == static_cast<visual_t>(1.0)) {
    return spectrum1;
  }

  if (spectrum1_r == static_cast<visual_t>(1.0) &&
      spectrum1_g == static_cast<visual_t>(1.0) &&
      spectrum1_b == static_cast<visual_t>(1.0)) {
    return spectrum0;
  }

  return MakeReferenceCounted<ColorSpectrum>(rgb_color);
}

ReferenceCounted<Reflector> ColorSpectrumManager::AllocateReflector(
    const Color& color) {
  Color rgb_color = ToRGB(color);
  if (rgb_color.values[0] <= static_cast<visual_t>(0.0) &&
      rgb_color.values[1] <= static_cast<visual_t>(0.0) &&
      rgb_color.values[2] <= static_cast<visual_t>(0.0)) {
    return ReferenceCounted<Reflector>();
  }

  return MakeReferenceCounted<ColorReflector>(rgb_color);
}

ReferenceCounted<Reflector> ColorSpectrumManager::AllocateReflector(
    const std::map<visual, visual>& wavelengths) {
  ReferenceCounted<Reflector> sampled_reflector =
      reflectors::CreateSampledReflector(wavelengths);
  if (!sampled_reflector) {
    return ReferenceCounted<Reflector>();
  }

  auto values = g_color_matcher.Match(*sampled_reflector);
  iris::Color xyz_color(values[0], values[1], values[2],
                        g_color_matcher.ColorSpace());
  auto rgb_color = xyz_color.ConvertTo(iris::Color::LINEAR_SRGB);
  if (rgb_color.r <= static_cast<visual>(0.0) &&
      rgb_color.g <= static_cast<visual>(0.0) &&
      rgb_color.b <= static_cast<visual>(0.0)) {
    return ReferenceCounted<Reflector>();
  }

  auto bounded_rgb_color =
      iris::Color(std::min(static_cast<visual>(1.0), rgb_color.r),
                  std::min(static_cast<visual>(1.0), rgb_color.g),
                  std::min(static_cast<visual>(1.0), rgb_color.b),
                  iris::Color::LINEAR_SRGB);

  return MakeReferenceCounted<ColorReflector>(bounded_rgb_color);
}

void ColorSpectrumManager::Clear() {}

}  // namespace spectrum_managers
}  // namespace pbrt_frontend
}  // namespace iris