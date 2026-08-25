#include "frontends/pbrt/lights/infinite.h"

#include <ImfArray.h>
#include <ImfRgbaFile.h>

#include <cstdlib>
#include <filesystem>
#include <iostream>

#include "frontends/pbrt/defaults.h"
#include "frontends/pbrt/spectrum_manager.h"
#include "iris/environmental_light.h"
#include "iris/environmental_lights/image_environmental_light.h"
#include "iris/matrix.h"
#include "iris/reference_counted.h"
#include "pbrt_proto/pbrt.pb.h"
#include "third_party/stb/stb_image.h"

namespace iris {
namespace pbrt_frontend {
namespace lights {

using ::iris::environmental_lights::MakeImageEnvironmentalLight;
using ::pbrt_proto::InfiniteLightSource;

ReferenceCounted<EnvironmentalLight> MakeInfinite(
    const InfiniteLightSource& infinite,
    const std::filesystem::path& search_root, const Matrix& model_to_world,
    SpectrumManager& spectrum_manager) {
  InfiniteLightSource with_defaults = Defaults().light_sources().infinite();
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
  if (with_defaults.has_filename()) {
    std::filesystem::path filename = with_defaults.filename();
    if (filename.is_relative()) {
      filename = search_root / filename;
    }

    if (!std::filesystem::is_regular_file(filename)) {
      std::cerr << "ERROR: Could not find file: " << with_defaults.filename()
                << std::endl;
      exit(EXIT_FAILURE);
    }

    if (filename.extension() == ".png" || filename.extension() == ".tga") {
      int nx, ny;
      if (stbi_is_16_bit(filename.native().c_str())) {
        int num_channels;
        stbi_us* values =
            stbi_load_16(filename.native().c_str(), &nx, &ny, &num_channels,
                         /*desired_channels=*/0);
        if (!values) {
          std::cerr << "ERROR: Failed to load image: " << filename << std::endl;
          exit(EXIT_FAILURE);
        }

        spectra_and_luma.reserve(nx * ny);
        if (num_channels == 1 || num_channels == 2) {
          for (int y = 0; y < ny; y++) {
            for (int x = 0; x < nx; x++) {
              visual_t grey =
                  static_cast<visual_t>(values[num_channels * y * nx + x]) /
                  static_cast<visual_t>(65535.0);

              visual_t luma_value;
              ReferenceCounted<Spectrum> pixel_spectrum =
                  spectrum_manager.AllocateSpectrum(
                      spectrum_manager.AllocateSpectrum(grey, grey, grey),
                      scaled, &luma_value);

              spectra_and_luma.emplace_back(std::move(pixel_spectrum),
                                            luma_value);
            }
          }
        } else {
          for (int y = 0; y < ny; y++) {
            for (int x = 0; x < nx; x++) {
              visual_t r = static_cast<visual_t>(
                               values[num_channels * (y * nx + x) + 0]) /
                           static_cast<visual_t>(65535.0);
              visual_t g = static_cast<visual_t>(
                               values[num_channels * (y * nx + x) + 1]) /
                           static_cast<visual_t>(65535.0);
              visual_t b = static_cast<visual_t>(
                               values[num_channels * (y * nx + x) + 2]) /
                           static_cast<visual_t>(65535.0);

              visual_t luma_value;
              ReferenceCounted<Spectrum> pixel_spectrum =
                  spectrum_manager.AllocateSpectrum(
                      spectrum_manager.AllocateSpectrum(r, g, b), scaled,
                      &luma_value);

              spectra_and_luma.emplace_back(std::move(pixel_spectrum),
                                            luma_value);
            }
          }
        }

        size.first = static_cast<size_t>(ny);
        size.second = static_cast<size_t>(nx);
        stbi_image_free(values);
      } else {
        int num_channels;
        stbi_uc* values =
            stbi_load(filename.native().c_str(), &nx, &ny, &num_channels,
                      /*desired_channels=*/0);
        if (!values) {
          std::cerr << "ERROR: Failed to load image: " << filename << std::endl;
          exit(EXIT_FAILURE);
        }

        spectra_and_luma.reserve(nx * ny);
        if (num_channels == 1 || num_channels == 2) {
          for (int y = 0; y < ny; y++) {
            for (int x = 0; x < nx; x++) {
              visual_t v =
                  static_cast<visual_t>(values[num_channels * (y * nx + x)]) /
                  static_cast<visual_t>(255.0);

              visual_t luma_value;
              ReferenceCounted<Spectrum> pixel_spectrum =
                  spectrum_manager.AllocateSpectrum(
                      spectrum_manager.AllocateSpectrum(v, v, v), scaled,
                      &luma_value);

              spectra_and_luma.emplace_back(std::move(pixel_spectrum),
                                            luma_value);
            }
          }
        } else {
          for (int y = 0; y < ny; y++) {
            for (int x = 0; x < nx; x++) {
              visual_t r = static_cast<visual_t>(
                               values[num_channels * (y * nx + x) + 0]) /
                           static_cast<visual_t>(255.0);
              visual_t g = static_cast<visual_t>(
                               values[num_channels * (y * nx + x) + 1]) /
                           static_cast<visual_t>(255.0);
              visual_t b = static_cast<visual_t>(
                               values[num_channels * (y * nx + x) + 2]) /
                           static_cast<visual_t>(255.0);

              visual_t luma_value;
              ReferenceCounted<Spectrum> pixel_spectrum =
                  spectrum_manager.AllocateSpectrum(
                      spectrum_manager.AllocateSpectrum(r, g, b), scaled,
                      &luma_value);

              spectra_and_luma.emplace_back(std::move(pixel_spectrum),
                                            luma_value);
            }
          }
        }

        size.first = static_cast<size_t>(ny);
        size.second = static_cast<size_t>(nx);
        stbi_image_free(values);
      }
    } else if (filename.extension() == ".exr") {
      try {
        Imf::RgbaInputFile file(filename.string().c_str());
        Imath::Box2i dw = file.dataWindow();
        int width = dw.max.x - dw.min.x + 1;
        int height = dw.max.y - dw.min.y + 1;

        Imf::Array2D<Imf::Rgba> pixels(height, width);
        file.setFrameBuffer(&pixels[0][0] - dw.min.x - dw.min.y * width, 1,
                            width);
        file.readPixels(dw.min.y, dw.max.y);

        spectra_and_luma.reserve(height * width);
        for (int y = 0; y < height; y++) {
          for (int x = 0; x < width; x++) {
            const Imf::Rgba& p = pixels[y][x];
            visual r = static_cast<visual>(p.r);
            visual g = static_cast<visual>(p.g);
            visual b = static_cast<visual>(p.b);

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

            spectra_and_luma.emplace_back(std::move(pixel_spectrum),
                                          luma_value);
          }
        }

        size.first = static_cast<size_t>(height);
        size.second = static_cast<size_t>(width);
      } catch (const std::exception& e) {
        std::cerr << "ERROR: Image loading failed with error: " << e.what()
                  << std::endl;
        exit(EXIT_FAILURE);
      }
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
