#include "frontends/pbrt/spectrum_managers/color_spectrum_manager.h"

#include "frontends/pbrt/spectrum_managers/internal/color_reflector.h"
#include "frontends/pbrt/spectrum_managers/internal/color_spectrum.h"
#include "iris/color_matchers/cie_color_matcher.h"
#include "iris/reflectors/sampled_reflector.h"
#include "iris/spectra/sampled_spectrum.h"

namespace iris::pbrt_frontend::spectrum_managers {
namespace {

static iris::color_matchers::CieColorMatcher g_color_matcher;

visual_t ComputeSpectralScalar(bool all_spectra_are_reflective) {
  if (!all_spectra_are_reflective) {
    return 1.0;
  }

  std::map<visual, visual> wavelengths = {{1.0, 1.0}};
  iris::spectra::SampledSpectrum sampled_spectrum(wavelengths);
  return 1.0 / g_color_matcher.Match(sampled_spectrum)[1];
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

};  // namespace

std::array<visual_t, 3> ColorColorMatcher::Match(
    const Spectrum& spectrum) const {
  return {spectrum.Intensity(0.5), spectrum.Intensity(1.5),
          spectrum.Intensity(2.5)};
}

iris::Color::Space ColorColorMatcher::ColorSpace() const {
  return iris::Color::LINEAR_SRGB;
}

ColorSpectrumManager::ColorSpectrumManager(bool all_spectra_are_reflective)
    : spectral_scalar_(ComputeSpectralScalar(all_spectra_are_reflective)) {}

ReferenceCounted<Spectrum> ColorSpectrumManager::AllocateSpectrum(
    const Color& color) {
  Color rgb_color = ToRGB(color);
  if (rgb_color.values[0] <= 0.0 && rgb_color.values[1] <= 0.0 &&
      rgb_color.values[2] <= 0.0) {
    return ReferenceCounted<Spectrum>();
  }

  return iris::MakeReferenceCounted<
      iris::pbrt_frontend::spectrum_managers::internal::ColorSpectrum>(
      rgb_color);
}

ReferenceCounted<Spectrum> ColorSpectrumManager::AllocateSpectrum(
    const std::map<visual, visual>& wavelengths) {
  iris::spectra::SampledSpectrum sampled_spectrum(wavelengths);

  auto values = g_color_matcher.Match(sampled_spectrum);
  iris::Color xyz_color(
      values[0] * spectral_scalar_, values[1] * spectral_scalar_,
      values[2] * spectral_scalar_, g_color_matcher.ColorSpace());

  auto rgb_color = xyz_color.ConvertTo(iris::Color::LINEAR_SRGB);
  if (rgb_color.r <= 0.0 && rgb_color.g <= 0.0 && rgb_color.b <= 0.0) {
    return ReferenceCounted<Spectrum>();
  }

  return iris::MakeReferenceCounted<
      iris::pbrt_frontend::spectrum_managers::internal::ColorSpectrum>(
      rgb_color);
}

ReferenceCounted<Reflector> ColorSpectrumManager::AllocateReflector(
    const Color& color) {
  Color rgb_color = ToRGB(color);
  if (rgb_color.values[0] <= 0.0 && rgb_color.values[1] <= 0.0 &&
      rgb_color.values[2] <= 0.0) {
    return ReferenceCounted<Reflector>();
  }

  return iris::MakeReferenceCounted<
      iris::pbrt_frontend::spectrum_managers::internal::ColorReflector>(
      rgb_color);
}

ReferenceCounted<Reflector> ColorSpectrumManager::AllocateReflector(
    const std::map<visual, visual>& wavelengths) {
  iris::reflectors::SampledReflector sampled_reflector(wavelengths);

  auto values = g_color_matcher.Match(sampled_reflector);
  iris::Color xyz_color(values[0], values[1], values[2],
                        g_color_matcher.ColorSpace());
  auto rgb_color = xyz_color.ConvertTo(iris::Color::LINEAR_SRGB);
  if (rgb_color.r <= 0.0 && rgb_color.g <= 0.0 && rgb_color.b <= 0.0) {
    return ReferenceCounted<Reflector>();
  }

  auto bounded_rgb_color =
      iris::Color(std::min(static_cast<visual>(1.0), rgb_color.r),
                  std::min(static_cast<visual>(1.0), rgb_color.g),
                  std::min(static_cast<visual>(1.0), rgb_color.b),
                  iris::Color::LINEAR_SRGB);

  return iris::MakeReferenceCounted<
      iris::pbrt_frontend::spectrum_managers::internal::ColorReflector>(
      bounded_rgb_color);
}

void ColorSpectrumManager::Clear() {}

}  // namespace iris::pbrt_frontend::spectrum_managers