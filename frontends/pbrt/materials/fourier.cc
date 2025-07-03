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
#include "libfbsdf/readers/standard_bsdf_reader.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace materials {
namespace {

using ::iris::materials::MakeFourierMaterial;
using ::libfbsdf::ReadFromStandardBsdf;
using ::libfbsdf::ReadFromStandardBsdfResult;
using ::pbrt_proto::v3::Material;
using ::pbrt_proto::v3::Shape;

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

  std::expected<ReadFromStandardBsdfResult, std::string> result =
      ReadFromStandardBsdf(file_stream);
  if (!result) {
    std::cerr << "ERROR: Failed to read fourier bsdf file with error: "
              << result.error() << std::endl;
    exit(EXIT_FAILURE);
  }

  ReferenceCounted<iris::Material> material;
  if (!result->r_coefficients.empty() && !result->b_coefficients.empty()) {
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
        std::move(result->elevational_samples), std::move(result->cdf),
        std::move(result->series_extents), std::move(result->y_coefficients),
        std::move(result->r_coefficients), std::move(result->b_coefficients),
        result->index_of_refraction);
  } else {
    material = MakeFourierMaterial(
        std::move(result->elevational_samples), std::move(result->cdf),
        std::move(result->series_extents), std::move(result->y_coefficients),
        result->index_of_refraction);
  }

  return MaterialResult{{material, material},
                        MakeBumpMap(with_defaults.bumpmap(), texture_manager)};
}

}  // namespace materials
}  // namespace pbrt_frontend
}  // namespace iris
