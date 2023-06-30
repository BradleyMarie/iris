#include "frontends/pbrt/lights/infinite.h"

#include "iris/environmental_lights/image_environmental_light.h"
#include "iris/spectra/uniform_spectrum.h"
#include "third_party/stb/stb_image.h"
#include "third_party/tinyexr/tinyexr.h"

namespace iris::pbrt_frontend::lights {
namespace {

static const std::unordered_map<std::string_view, Parameter::Type>
    g_parameters = {
        {"L", Parameter::SPECTRUM},
        {"mapname", Parameter::FILE_PATH},
        {"samples", Parameter::INTEGER},
};

static const ReferenceCounted<Spectrum> kWhiteSpectrum =
    MakeReferenceCounted<spectra::UniformSpectrum>(1.0);

class InfiniteBuilder
    : public ObjectBuilder<std::variant<ReferenceCounted<Light>,
                                        ReferenceCounted<EnvironmentalLight>>,
                           SpectrumManager&, const Matrix&> {
 public:
  InfiniteBuilder() : ObjectBuilder(g_parameters) {}

  std::variant<ReferenceCounted<Light>, ReferenceCounted<EnvironmentalLight>>
  Build(const std::unordered_map<std::string_view, Parameter>& parameters,
        SpectrumManager& spectrum_manager,
        const Matrix& model_to_world) const override;
};

std::variant<ReferenceCounted<Light>, ReferenceCounted<EnvironmentalLight>>
InfiniteBuilder::Build(
    const std::unordered_map<std::string_view, Parameter>& parameters,
    SpectrumManager& spectrum_manager, const Matrix& model_to_world) const {
  std::vector<std::pair<ReferenceCounted<Spectrum>, visual>>
      spectra_and_luminance = {{kWhiteSpectrum, 1.0}};
  std::pair<size_t, size_t> size(1, 1);
  ReferenceCounted<Spectrum> scalar = kWhiteSpectrum;

  auto l_iter = parameters.find("L");
  if (l_iter != parameters.end()) {
    scalar = l_iter->second.GetSpectra().front();
  }

  auto mapname_iter = parameters.find("mapname");
  if (mapname_iter != parameters.end()) {
    spectra_and_luminance.clear();

    const char* filename = mapname_iter->second.GetFilePaths().front().c_str();

    std::string_view filename_view = filename;
    if (filename_view.ends_with(".png") || filename_view.ends_with(".tga")) {
      std::cerr
          << "ERROR: Unimplemented image file type: "
          << mapname_iter->second.GetFilePaths().front().extension().c_str()
          << std::endl;
      exit(EXIT_FAILURE);
    } else if (filename_view.ends_with(".exr")) {
      float* values;
      int width, height;
      const char* error_message;
      int error = LoadEXR(&values, &width, &height, filename, &error_message);
      if (error < 0) {
        std::cerr << "ERROR: Image loading failed with error: " << error_message
                  << std::endl;
        exit(EXIT_FAILURE);
      }

      for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
          visual r = static_cast<visual>(values[4 * (y * width + x) + 0]);
          visual g = static_cast<visual>(values[4 * (y * width + x) + 1]);
          visual b = static_cast<visual>(values[4 * (y * width + x) + 2]);

          if (!std::isfinite(r) || r < 0.0 || !std::isfinite(g) || g < 0.0 ||
              !std::isfinite(b) || b < 0.0) {
            std::cerr << "ERROR: Image file contained an out of range value"
                      << stbi_failure_reason() << std::endl;
            exit(EXIT_FAILURE);
          }

          // TODO: Compute a better value for luma
          visual luma = (r + g + b) / static_cast<visual>(3.0);

          spectra_and_luminance.push_back(
              {spectrum_manager.AllocateSpectrum(Color{{r, g, b}, Color::RGB}),
               luma});
        }
      }

      free(values);

      size.first = static_cast<size_t>(height);
      size.second = static_cast<size_t>(width);
    } else {
      std::string_view extension =
          mapname_iter->second.GetFilePaths().front().extension().c_str();
      if (extension.empty()) {
        std::cerr
            << "ERROR: Unsupported image file (no extension): "
            << mapname_iter->second.GetFilePaths().front().filename().c_str()
            << std::endl;
      } else {
        std::cerr << "ERROR: Unsupported image file type: " << extension
                  << std::endl;
      }

      exit(EXIT_FAILURE);
    }
  }

  return MakeReferenceCounted<environmental_lights::ImageEnvironmentalLight>(
      spectra_and_luminance, size, scalar, model_to_world);
}

};  // namespace

extern const std::unique_ptr<const ObjectBuilder<
    std::variant<ReferenceCounted<Light>, ReferenceCounted<EnvironmentalLight>>,
    SpectrumManager&, const Matrix&>>
    g_infinite_builder = std::make_unique<InfiniteBuilder>();

}  // namespace iris::pbrt_frontend::lights