#include "frontends/pbrt/spectrum_managers/color_spectrum_manager.h"

#include <algorithm>
#include <cstdlib>
#include <expected>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>

#include "frontends/pbrt/spectrum_manager.h"
#include "frontends/pbrt/spectrum_managers/internal/color_reflector.h"
#include "frontends/pbrt/spectrum_managers/internal/color_spectrum.h"
#include "iris/albedo_matcher.h"
#include "iris/color_matcher.h"
#include "iris/color_matchers/cie_color_matcher.h"
#include "iris/power_matcher.h"
#include "iris/reference_counted.h"
#include "iris/reflector.h"
#include "iris/reflectors/sampled_reflector.h"
#include "iris/spectra/blackbody_spectrum.h"
#include "iris/spectra/sampled_spectrum.h"
#include "iris/spectrum.h"
#include "libspd/readers/emissive_spd_reader.h"
#include "libspd/readers/reflective_spd_reader.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace spectrum_managers {
namespace {

using ::iris::color_matchers::kCieYIntegral;
using ::iris::color_matchers::MakeCieColorMatcher;
using ::iris::pbrt_frontend::spectrum_managers::internal::MakeColorReflector;
using ::iris::pbrt_frontend::spectrum_managers::internal::MakeColorSpectrum;
using ::iris::reflectors::CreateSampledReflector;
using ::iris::spectra::MakeSampledSpectrum;
using ::iris::spectra::MakeScaledBlackbodySpectrum;
using ::libspd::ReadEmissiveSpdFrom;
using ::libspd::ReadReflectiveSpdFrom;
using ::pbrt_proto::v3::SampledSpectrum;
using ::pbrt_proto::v3::Spectrum;

std::map<visual, visual> FromFile(const std::filesystem::path& search_root,
                                  std::filesystem::path path, bool reflective) {
  if (path.is_relative()) {
    path = search_root / path;
  }

  if (!std::filesystem::is_regular_file(path)) {
    std::cerr << "ERROR: Could not find file: " << path << std::endl;
    exit(EXIT_FAILURE);
  }

  std::ifstream file(path, std::ios::in | std::ios::binary);

  std::expected<std::map<visual, visual>, std::string> values;
  if (reflective) {
    values = ReadReflectiveSpdFrom<visual>(file);
  } else {
    values = ReadEmissiveSpdFrom<visual>(file);
  }

  if (!values) {
    std::cerr << "ERROR: Failed to read SPD file with error: " << values.error()
              << std::endl;
    exit(EXIT_FAILURE);
  }

  return std::move(*values);
}

std::map<visual, visual> ToMap(const SampledSpectrum& sampled) {
  std::map<visual, visual> result;
  for (const auto& entry : sampled.samples()) {
    result[entry.wavelength()] = entry.intensity();
  }
  return result;
}

Color ToReflectorColor(std::map<visual, visual> samples) {
  std::unique_ptr<ColorMatcher> color_matcher = MakeCieColorMatcher();

  for (auto& [_, intensity] : samples) {
    intensity = std::min(static_cast<visual>(1.0), intensity);
  }

  std::array<visual, 3> colors = {static_cast<visual>(0.0),
                                  static_cast<visual>(0.0),
                                  static_cast<visual>(0.0)};
  if (ReferenceCounted<iris::Spectrum> reflector = MakeSampledSpectrum(samples);
      reflector) {
    colors = color_matcher->Match(*reflector);
    colors[0] /= kCieYIntegral;
    colors[1] /= kCieYIntegral;
    colors[2] /= kCieYIntegral;
  }

  Color result(colors[0], colors[1], colors[2], color_matcher->ColorSpace());
  return result.ConvertTo(Color::LINEAR_SRGB);
}

Color ToSpectrumColor(std::map<visual, visual> samples, bool normalize_luma) {
  std::unique_ptr<ColorMatcher> color_matcher = MakeCieColorMatcher();

  std::array<visual, 3> colors = {static_cast<visual>(0.0),
                                  static_cast<visual>(0.0),
                                  static_cast<visual>(0.0)};
  if (ReferenceCounted<iris::Spectrum> spectrum = MakeSampledSpectrum(samples);
      spectrum) {
    colors = color_matcher->Match(*spectrum);
    if (normalize_luma) {
      colors[0] /= kCieYIntegral;
      colors[1] /= kCieYIntegral;
      colors[2] /= kCieYIntegral;
    }
  }

  Color result(colors[0], colors[1], colors[2], color_matcher->ColorSpace());
  return result.ConvertTo(Color::LINEAR_SRGB);
}

Color ToReflectorColor(visual temperature_kelvin, visual_t scale) {
  std::unique_ptr<ColorMatcher> color_matcher = MakeCieColorMatcher();

  std::array<visual, 3> colors = {static_cast<visual>(0.0),
                                  static_cast<visual>(0.0),
                                  static_cast<visual>(0.0)};
  if (ReferenceCounted<iris::Spectrum> reflector = MakeScaledBlackbodySpectrum(
          temperature_kelvin, static_cast<visual>(1.0));
      reflector) {
    colors = color_matcher->Match(*reflector);
    colors[0] *= scale / kCieYIntegral;
    colors[1] *= scale / kCieYIntegral;
    colors[2] *= scale / kCieYIntegral;
  }

  Color result(colors[0], colors[1], colors[2], color_matcher->ColorSpace());
  return result.ConvertTo(Color::LINEAR_SRGB);
}

Color ToSpectrumColor(visual temperature_kelvin, visual_t scale,
                      bool normalize_luma) {
  std::unique_ptr<ColorMatcher> color_matcher = MakeCieColorMatcher();

  std::array<visual, 3> colors = {static_cast<visual>(0.0),
                                  static_cast<visual>(0.0),
                                  static_cast<visual>(0.0)};
  if (ReferenceCounted<iris::Spectrum> spectrum = MakeScaledBlackbodySpectrum(
          temperature_kelvin, static_cast<visual>(1.0));
      spectrum) {
    colors = color_matcher->Match(*spectrum);
    if (normalize_luma) {
      colors[0] /= kCieYIntegral;
      colors[1] /= kCieYIntegral;
      colors[2] /= kCieYIntegral;
    }
    colors[0] *= scale;
    colors[1] *= scale;
    colors[2] *= scale;
  }

  Color result(colors[0], colors[1], colors[2], color_matcher->ColorSpace());
  return result.ConvertTo(Color::LINEAR_SRGB);
}

}  // namespace

std::array<visual_t, 3> ColorColorMatcher::Match(
    const iris::Spectrum& spectrum) const {
  return {spectrum.Intensity(static_cast<visual_t>(0.5)),
          spectrum.Intensity(static_cast<visual_t>(1.5)),
          spectrum.Intensity(static_cast<visual_t>(2.5))};
}

Color::Space ColorColorMatcher::ColorSpace() const {
  return Color::LINEAR_SRGB;
}

visual_t ColorAlbedoMatcher::Match(const Reflector& reflector) const {
  return std::max(reflector.Reflectance(static_cast<visual_t>(0.5)),
                  std::max(reflector.Reflectance(static_cast<visual_t>(1.5)),
                           reflector.Reflectance(static_cast<visual_t>(2.5))));
}

visual_t ColorPowerMatcher::Match(const iris::Spectrum& spectrum) const {
  Color color(spectrum.Intensity(static_cast<visual_t>(0.5)),
              spectrum.Intensity(static_cast<visual_t>(1.5)),
              spectrum.Intensity(static_cast<visual_t>(2.5)),
              Color::LINEAR_SRGB);
  return color.Luma();
}

visual_t ColorSpectrumManager::ComputeLuma(visual_t r, visual_t g, visual_t b) {
  Color color(r, g, b, Color::LINEAR_SRGB);
  return color.Luma();
}

ReferenceCounted<iris::Spectrum> ColorSpectrumManager::AllocateSpectrum(
    const Spectrum& spectrum, visual_t* luma) {
  ReferenceCounted<iris::Spectrum> result;
  switch (spectrum.spectrum_type_case()) {
    case Spectrum::kUniformSpectrum:
      result = MakeColorSpectrum(
          static_cast<visual>(spectrum.uniform_spectrum()),
          static_cast<visual>(spectrum.uniform_spectrum()),
          static_cast<visual>(spectrum.uniform_spectrum()), Color::LINEAR_SRGB);
      break;
    case Spectrum::kBlackbodySpectrum:
      result = MakeColorSpectrum(ToSpectrumColor(
          static_cast<visual>(spectrum.blackbody_spectrum().temperature()),
          static_cast<visual>(spectrum.blackbody_spectrum().scale()),
          all_sampled_spectra_are_reflective_));
      break;
    case Spectrum::kRgbSpectrum:
      result = MakeColorSpectrum(
          static_cast<visual>(spectrum.rgb_spectrum().r()),
          static_cast<visual>(spectrum.rgb_spectrum().g()),
          static_cast<visual>(spectrum.rgb_spectrum().b()), Color::LINEAR_SRGB);
      break;
    case Spectrum::kXyzSpectrum:
      result = MakeColorSpectrum(
          static_cast<visual>(spectrum.xyz_spectrum().x()),
          static_cast<visual>(spectrum.xyz_spectrum().y()),
          static_cast<visual>(spectrum.xyz_spectrum().z()), Color::CIE_XYZ);
      break;
    case Spectrum::kSampledSpectrum:
      result = MakeColorSpectrum(
          ToSpectrumColor(ToMap(spectrum.sampled_spectrum()),
                          all_sampled_spectra_are_reflective_));
      break;
    case Spectrum::kSampledSpectrumFilename:
      result = MakeColorSpectrum(ToSpectrumColor(
          FromFile(search_root_, spectrum.sampled_spectrum_filename(), false),
          all_sampled_spectra_are_reflective_));
      break;
    case Spectrum::SPECTRUM_TYPE_NOT_SET:
      break;
  }

  if (luma) {
    *luma = static_cast<visual_t>(0.0);
    if (result) {
      *luma = ColorPowerMatcher().Match(*result);
    }
  }

  return result;
}

ReferenceCounted<Reflector> ColorSpectrumManager::AllocateReflector(
    const Spectrum& spectrum) {
  ReferenceCounted<Reflector> result;
  switch (spectrum.spectrum_type_case()) {
    case Spectrum::kUniformSpectrum:
      result = MakeColorReflector(
          static_cast<visual>(spectrum.uniform_spectrum()),
          static_cast<visual>(spectrum.uniform_spectrum()),
          static_cast<visual>(spectrum.uniform_spectrum()), Color::LINEAR_SRGB);
      break;
    case Spectrum::kBlackbodySpectrum:
      result = MakeColorReflector(ToReflectorColor(
          static_cast<visual>(spectrum.blackbody_spectrum().temperature()),
          static_cast<visual>(spectrum.blackbody_spectrum().scale())));
      break;
    case Spectrum::kRgbSpectrum:
      result = MakeColorReflector(
          static_cast<visual>(spectrum.rgb_spectrum().r()),
          static_cast<visual>(spectrum.rgb_spectrum().g()),
          static_cast<visual>(spectrum.rgb_spectrum().b()), Color::LINEAR_SRGB);
      break;
    case Spectrum::kXyzSpectrum:
      result = MakeColorReflector(
          static_cast<visual>(spectrum.xyz_spectrum().x()),
          static_cast<visual>(spectrum.xyz_spectrum().y()),
          static_cast<visual>(spectrum.xyz_spectrum().z()), Color::CIE_XYZ);
      break;
    case Spectrum::kSampledSpectrum:
      result = MakeColorReflector(
          ToReflectorColor(ToMap(spectrum.sampled_spectrum())));
      break;
    case Spectrum::kSampledSpectrumFilename:
      result = MakeColorReflector(ToReflectorColor(
          FromFile(search_root_, spectrum.sampled_spectrum_filename(), true)));
      break;
    case Spectrum::SPECTRUM_TYPE_NOT_SET:
      break;
  }

  return result;
}

ReferenceCounted<iris::Spectrum> ColorSpectrumManager::AllocateSpectrum(
    const ReferenceCounted<iris::Spectrum>& spectrum0,
    const ReferenceCounted<iris::Spectrum>& spectrum1, visual_t* luma) {
  if (!spectrum0 || !spectrum1) {
    if (luma) {
      *luma = static_cast<visual_t>(0.0);
    }

    return ReferenceCounted<iris::Spectrum>();
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

  if (luma) {
    *luma = Color(r, g, b, Color::LINEAR_SRGB).Luma();
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

  return MakeColorSpectrum(r, g, b, Color::LINEAR_SRGB);
}

}  // namespace spectrum_managers
}  // namespace pbrt_frontend
}  // namespace iris
