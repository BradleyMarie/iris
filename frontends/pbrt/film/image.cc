#include "frontends/pbrt/film/image.h"

#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <functional>
#include <iostream>
#include <limits>

#include "frontends/pbrt/defaults.h"
#include "frontends/pbrt/film/result.h"
#include "iris/file/exr_writer.h"
#include "iris/file/pfm_writer.h"
#include "iris/framebuffer.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace film {

using ::iris::file::WriteExr;
using ::iris::file::WritePfm;
using ::pbrt_proto::v3::Film;

constexpr int32_t kMaxImageDimensionSize = 16384u;

std::unique_ptr<FilmResult> MakeImage(const Film::Image& image) {
  Film::Image with_defaults = Defaults().films().image();
  with_defaults.MergeFrom(image);

  if (with_defaults.cropwindow().x_min() < 0.0 ||
      with_defaults.cropwindow().x_min() > 1.0 ||
      with_defaults.cropwindow().x_max() < 0.0 ||
      with_defaults.cropwindow().x_max() > 1.0 ||
      with_defaults.cropwindow().y_min() < 0.0 ||
      with_defaults.cropwindow().y_min() > 1.0 ||
      with_defaults.cropwindow().y_max() < 0.0 ||
      with_defaults.cropwindow().y_max() > 1.0) {
    std::cerr << "ERROR: Out of range value for parameter: cropwindow"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  if (with_defaults.cropwindow().x_min() >=
          with_defaults.cropwindow().x_max() ||
      with_defaults.cropwindow().y_min() >=
          with_defaults.cropwindow().y_max()) {
    std::cerr << "ERROR: Invalid values for parameter list: cropwindow"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  if (with_defaults.xresolution() <= 0 ||
      with_defaults.xresolution() > kMaxImageDimensionSize) {
    std::cerr << "ERROR: Out of range value for parameter: xresolution"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  if (with_defaults.yresolution() <= 0 ||
      with_defaults.yresolution() > kMaxImageDimensionSize) {
    std::cerr << "ERROR: Out of range value for parameter: yresolution"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  if (with_defaults.diagonal() <= 0.0) {
    std::cerr << "ERROR: Out of range value for parameter: diagonal"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  if (with_defaults.scale() <= 0.0) {
    std::cerr << "ERROR: Out of range value for parameter: scale" << std::endl;
    exit(EXIT_FAILURE);
  }

  if (with_defaults.maxsampleluminance() <= 0.0) {
    std::cerr << "ERROR: Out of range value for parameter: maxsampleluminance"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  std::filesystem::path filename = with_defaults.filename();
  std::function<void(Framebuffer&, std::ofstream&)> write_to_file_function;
  if (filename.extension() == ".exr") {
    write_to_file_function = [](Framebuffer& framebuffer,
                                std::ofstream& output) {
      bool success = WriteExr(framebuffer, output);
      if (!success) {
        std::cerr << "ERROR: Failed to write output file" << std::endl;
        exit(EXIT_FAILURE);
      }
    };
  } else if (filename.extension() == ".pfm") {
    write_to_file_function = [](Framebuffer& framebuffer,
                                std::ofstream& output) {
      bool success = WritePfm(framebuffer, Color::LINEAR_SRGB, output);
      if (!success) {
        std::cerr << "ERROR: Failed to write output file" << std::endl;
        exit(EXIT_FAILURE);
      }
    };
  } else {
    std::cerr << "ERROR: Unsupported file extension for parameter: filename"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  std::array<size_t, 4> crop_pixels;
  crop_pixels[0] = static_cast<size_t>(with_defaults.cropwindow().y_min() *
                                       with_defaults.yresolution());
  crop_pixels[1] = static_cast<size_t>(with_defaults.cropwindow().y_max() *
                                       with_defaults.yresolution());
  crop_pixels[2] = static_cast<size_t>(with_defaults.cropwindow().x_min() *
                                       with_defaults.xresolution());
  crop_pixels[3] = static_cast<size_t>(with_defaults.cropwindow().x_max() *
                                       with_defaults.xresolution());

  std::function<void(Framebuffer&, std::ofstream&)> write_function =
      [crop_pixels, scale = with_defaults.scale(), write_to_file_function](
          Framebuffer& framebuffer, std::ofstream& output) {
        auto size = framebuffer.Size();

        if (scale != 1.0) {
          for (size_t y = 0; y < size.first; y++) {
            for (size_t x = 0; x < size.second; x++) {
              auto color =
                  framebuffer.Get(y, x).ConvertTo(iris::Color::LINEAR_SRGB);
              framebuffer.Set(
                  y, x,
                  iris::Color(color.r * scale, color.g * scale, color.b * scale,
                              iris::Color::LINEAR_SRGB));
            }
          }
        }

        Framebuffer cropped(
            {crop_pixels[1] - crop_pixels[0], crop_pixels[3] - crop_pixels[2]});
        for (size_t y = crop_pixels[0]; y < crop_pixels[1]; y++) {
          for (size_t x = crop_pixels[2]; x < crop_pixels[3]; x++) {
            cropped.Set(y - crop_pixels[0], x - crop_pixels[2],
                        framebuffer.Get(y, x));
          }
        }

        write_to_file_function(cropped, output);
      };

  std::function<bool(std::pair<size_t, size_t>, std::pair<size_t, size_t>)>
      skip_pixel_function =
          [crop_pixels](std::pair<size_t, size_t> pixel,
                        std::pair<size_t, size_t> with_defaults_dimensions) {
            return pixel.first < crop_pixels[0] ||
                   pixel.first >= crop_pixels[1] ||
                   pixel.second < crop_pixels[2] ||
                   pixel.second >= crop_pixels[3];
          };

  return std::make_unique<FilmResult>(FilmResult{
      filename,
      std::make_pair(static_cast<size_t>(with_defaults.yresolution()),
                     static_cast<size_t>(with_defaults.xresolution())),
      skip_pixel_function, write_function,
      static_cast<geometric_t>(with_defaults.diagonal()),
      static_cast<visual_t>(with_defaults.maxsampleluminance())});
}

}  // namespace film
}  // namespace pbrt_frontend
}  // namespace iris
