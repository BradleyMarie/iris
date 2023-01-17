#include "frontends/pbrt/spectrum_managers/linear_rgb_spectrum_manager.h"

#include "frontends/pbrt/spectrum_managers/internal/color_reflector.h"
#include "frontends/pbrt/spectrum_managers/internal/color_spectrum.h"
#include "iris/color_matchers/cie_color_matcher.h"
#include "iris/reflectors/sampled_reflector.h"
#include "iris/spectra/sampled_spectrum.h"

namespace iris::pbrt_frontend::spectrum_managers {
namespace {

static iris::color_matchers::CieColorMatcher g_color_matcher;

};  // namespace

std::array<visual_t, 3> LinearRGBColorMatcher::Match(
    const Spectrum& spectrum) const {
  return {spectrum.Intensity(0.5), spectrum.Intensity(1.5),
          spectrum.Intensity(2.5)};
}

iris::Color::Space LinearRGBColorMatcher::ColorSpace() const {
  return iris::Color::LINEAR_SRGB;
}

ReferenceCounted<Spectrum> LinearRGBSpectrumManager::AllocateSpectrum(
    const Color& color) {
  return iris::MakeReferenceCounted<
      iris::pbrt_frontend::spectrum_managers::internal::ColorSpectrum>(color);
}

ReferenceCounted<Spectrum> LinearRGBSpectrumManager::AllocateSpectrum(
    const std::map<visual, visual>& wavelengths) {
  iris::spectra::SampledSpectrum sampled_spectrum(wavelengths);

  auto values = g_color_matcher.Match(sampled_spectrum);
  iris::Color xyz_color(values[0], values[1], values[2],
                        g_color_matcher.ColorSpace());
  auto rgb_color = xyz_color.ConvertTo(iris::Color::LINEAR_SRGB);

  return iris::MakeReferenceCounted<
      iris::pbrt_frontend::spectrum_managers::internal::ColorSpectrum>(
      rgb_color);
}

ReferenceCounted<Reflector> LinearRGBSpectrumManager::AllocateReflector(
    const Color& color) {
  return iris::MakeReferenceCounted<
      iris::pbrt_frontend::spectrum_managers::internal::ColorReflector>(color);
}

ReferenceCounted<Reflector> LinearRGBSpectrumManager::AllocateReflector(
    const std::map<visual, visual>& wavelengths) {
  iris::reflectors::SampledReflector sampled_reflector(wavelengths);

  auto values = g_color_matcher.Match(sampled_reflector);
  iris::Color xyz_color(values[0], values[1], values[2],
                        g_color_matcher.ColorSpace());
  auto rgb_color = xyz_color.ConvertTo(iris::Color::LINEAR_SRGB);

  return iris::MakeReferenceCounted<
      iris::pbrt_frontend::spectrum_managers::internal::ColorReflector>(
      rgb_color);
}

void LinearRGBSpectrumManager::Clear() {}

}  // namespace iris::pbrt_frontend::spectrum_managers