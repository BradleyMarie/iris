#include "frontends/pbrt/materials/fourier.h"

#include <expected>
#include <filesystem>
#include <fstream>
#include <utility>
#include <vector>

#include "frontends/pbrt/defaults.h"
#include "frontends/pbrt/materials/bumpmap.h"
#include "frontends/pbrt/spectrum_manager.h"
#include "frontends/pbrt/texture_manager.h"
#include "iris/material.h"
#include "iris/materials/fourier_material.h"
#include "iris/normal_map.h"
#include "iris/reference_counted.h"
#include "libfbsdf/readers/validating_bsdf_reader.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace materials {
namespace {

using ::iris::materials::MakeFourierMaterial;
using ::libfbsdf::ValidatingBsdfReader;
using ::pbrt_proto::v3::Material;
using ::pbrt_proto::v3::Shape;

struct FourierBsdfData final : public ValidatingBsdfReader {
  std::vector<float> elevational_samples_;
  std::vector<float> coefficients_;
  std::vector<float> cdf_;
  std::vector<std::pair<uint32_t, uint32_t>> series_;
  uint32_t num_color_channels;
  float eta_;

  std::expected<Options, std::string> Start(const Flags& flags,
                                            uint32_t num_basis_functions,
                                            size_t num_color_channels,
                                            float index_of_refraction,
                                            float roughness_top,
                                            float roughness_bottom) override {
    if (!flags.is_bsdf || flags.uses_harmonic_extrapolation ||
        num_basis_functions != 1 ||
        (num_color_channels != 1 && num_color_channels != 3) ||
        index_of_refraction < 1.0f) {
      return std::unexpected("ERROR: Unsupported fourier bsdf input");
    }

    num_color_channels = num_color_channels;
    eta_ = index_of_refraction;

    return Options{
        .parse_elevational_samples = true,
        .parse_parameter_sample_counts = false,
        .parse_parameter_values = false,
        .parse_cdf_mu = true,
        .parse_series = true,
        .parse_coefficients = true,
        .parse_metadata = false,
    };
  }

  void HandleElevationalSamples(std::vector<float> samples) override {
    elevational_samples_ = std::move(samples);
  }

  void HandleCdf(std::vector<float> values) override {
    cdf_ = std::move(values);
  }

  void HandleCoefficients(std::vector<float> coefficients) override {
    coefficients_ = std::move(coefficients);
  }

  void HandleSeries(
      std::vector<std::pair<uint32_t, uint32_t>> series) override {
    series_ = std::move(series);
  }
};

}  // namespace

MaterialResult MakeFourier(const Material::Fourier& fourier,
                           const Shape::MaterialOverrides& overrides,
                           const std::filesystem::path& search_root,
                           TextureManager& texture_manager,
                           SpectrumManager& spectrum_manager) {
  Material::Fourier with_defaults = fourier;
  with_defaults.MergeFromString(overrides.SerializeAsString());

  std::filesystem::path path = with_defaults.bsdffile();
  if (path.is_relative()) {
    path = search_root / path;
  }

  if (!std::filesystem::is_regular_file(path)) {
    std::cerr
        << "ERROR: Could not open file specified by fourier parameter: bsdffile"
        << std::endl;
    exit(EXIT_FAILURE);
  }

  std::ifstream file_stream(path, std::ios::in | std::ios::binary);
  if (file_stream.fail()) {
    std::cerr
        << "ERROR: Could not open file specified by fourier parameter: bsdffile"
        << std::endl;
    exit(EXIT_FAILURE);
  }

  FourierBsdfData data;
  if (std::expected<void, std::string> result = data.ReadFrom(file_stream);
      !result) {
    std::cerr << "ERROR: Failed to read fourier bsdf file with error: "
              << result.error() << std::endl;
    exit(EXIT_FAILURE);
  }

  ReferenceCounted<iris::Material> material;
  if (data.num_color_channels == 1) {
    material = MakeFourierMaterial(
        std::move(data.elevational_samples_), std::move(data.cdf_),
        std::move(data.coefficients_), std::move(data.series_), data.eta_);
  } else {
    size_t num_samples =
        data.elevational_samples_.size() * data.elevational_samples_.size();

    material = MakeFourierMaterial(
        spectrum_manager.AllocateReflector(static_cast<visual_t>(1.0),
                                           static_cast<visual_t>(0.0),
                                           static_cast<visual_t>(0.0)),
        spectrum_manager.AllocateReflector(static_cast<visual_t>(0.0),
                                           static_cast<visual_t>(1.0),
                                           static_cast<visual_t>(0.0)),
        spectrum_manager.AllocateReflector(static_cast<visual_t>(0.0),
                                           static_cast<visual_t>(0.0),
                                           static_cast<visual_t>(1.0)),
        std::move(data.elevational_samples_), std::move(data.cdf_),
        std::move(data.coefficients_),
        std::vector<std::pair<uint32_t, uint32_t>>(
            data.series_.begin() + 0 * num_samples,
            data.series_.begin() + 1 * num_samples),
        std::vector<std::pair<uint32_t, uint32_t>>(
            data.series_.begin() + 1 * num_samples,
            data.series_.begin() + 2 * num_samples),
        std::vector<std::pair<uint32_t, uint32_t>>(
            data.series_.begin() + 2 * num_samples,
            data.series_.begin() + 3 * num_samples),
        data.eta_);
  }

  return MaterialResult{{material, material},
                        MakeBumpMap(with_defaults.bumpmap(), texture_manager)};
}

}  // namespace materials
}  // namespace pbrt_frontend
}  // namespace iris
