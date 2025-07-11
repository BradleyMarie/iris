#include "frontends/pbrt/lights/infinite.h"

#include <cstdlib>
#include <filesystem>
#include <iostream>

#include "frontends/pbrt/defaults.h"
#include "frontends/pbrt/spectrum_manager.h"
#include "iris/environmental_light.h"
#include "iris/environmental_lights/image_environmental_light.h"
#include "iris/matrix.h"
#include "iris/reference_counted.h"
#include "pbrt_proto/v3/pbrt.pb.h"
#include "third_party/tinyexr/tinyexr.h"

namespace iris {
namespace pbrt_frontend {
namespace lights {

using ::iris::environmental_lights::MakeImageEnvironmentalLight;
using ::pbrt_proto::v3::LightSource;

ReferenceCounted<EnvironmentalLight> MakeInfinite(
    const pbrt_proto::v3::LightSource::Infinite& infinite,
    const std::filesystem::path& search_root, const Matrix& model_to_world,
    SpectrumManager& spectrum_manager) {
  LightSource::Infinite with_defaults = Defaults().light_sources().infinite();
  with_defaults.MergeFrom(infinite);

  ReferenceCounted<Spectrum> l =
      spectrum_manager.AllocateSpectrum(with_defaults.l());
  if (!l) {
    return ReferenceCounted<EnvironmentalLight>();
  }

  ReferenceCounted<Spectrum> scale =
      spectrum_manager.AllocateSpectrum(with_defaults.scale());
  if (!scale) {
    return ReferenceCounted<EnvironmentalLight>();
  }

  visual_t scaled_luma;
  ReferenceCounted<Spectrum> scaled =
      spectrum_manager.AllocateSpectrum(l, scale, &scaled_luma);
  if (!scaled) {
    return ReferenceCounted<EnvironmentalLight>();
  }

  std::vector<std::pair<ReferenceCounted<Spectrum>, visual>> spectra_and_luma;
  std::pair<size_t, size_t> size;
  if (with_defaults.has_mapname()) {
    std::filesystem::path filename = with_defaults.mapname();
    if (filename.is_relative()) {
      filename = search_root / filename;
    }

    if (!std::filesystem::is_regular_file(filename)) {
      std::cerr << "ERROR: Could not find file: " << with_defaults.mapname()
                << std::endl;
      exit(EXIT_FAILURE);
    }

    if (filename.extension() == ".png") {
      std::cerr << "ERROR: Unimplemented image file type: .png" << std::endl;
      exit(EXIT_FAILURE);
    } else if (filename.extension() == ".tga") {
      std::cerr << "ERROR: Unimplemented image file type: .tga" << std::endl;
      exit(EXIT_FAILURE);
    } else if (filename.extension() == ".exr") {
      float* values;
      int width, height;
      const char* error_message;
      int error = LoadEXR(&values, &width, &height, filename.native().c_str(),
                          &error_message);
      if (error < 0) {
        std::cerr << "ERROR: Image loading failed with error: " << error_message
                  << std::endl;
        exit(EXIT_FAILURE);
      }

      spectra_and_luma.reserve(height * width);
      for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
          visual r = static_cast<visual>(values[4 * (y * width + x) + 0]);
          visual g = static_cast<visual>(values[4 * (y * width + x) + 1]);
          visual b = static_cast<visual>(values[4 * (y * width + x) + 2]);

          if (!std::isfinite(r) || r < 0.0 || !std::isfinite(g) || g < 0.0 ||
              !std::isfinite(b) || b < 0.0) {
            std::cerr << "ERROR: Image file contained an out of range value"
                      << std::endl;
            exit(EXIT_FAILURE);
          }

          visual_t luma_value;
          ReferenceCounted<Spectrum> pixel_spectrum =
              spectrum_manager.AllocateSpectrum(
                  spectrum_manager.AllocateSpectrum(r, g, b), scaled,
                  &luma_value);

          spectra_and_luma.emplace_back(std::move(pixel_spectrum), luma_value);
        }
      }

      free(values);

      size.first = static_cast<size_t>(height);
      size.second = static_cast<size_t>(width);
    } else {
      std::stringstream stream;
      if (filename.extension().empty()) {
        stream << filename.filename();
        std::string filename = stream.str();
        std::cerr << "ERROR: Unsupported image file (no extension): "
                  << filename.substr(1, filename.size() - 2) << std::endl;
      } else {
        stream << filename.extension();
        std::string ext = stream.str();
        std::cerr << "ERROR: Unsupported image file type: "
                  << ext.substr(1, ext.size() - 2) << std::endl;
      }

      exit(EXIT_FAILURE);
    }
  } else {
    spectra_and_luma.emplace_back(scaled, scaled_luma);
    size.first = 1;
    size.second = 1;
  }

  return MakeImageEnvironmentalLight(std::move(spectra_and_luma), size,
                                     model_to_world);
}

}  // namespace lights
}  // namespace pbrt_frontend
}  // namespace iris
