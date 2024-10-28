#include "frontends/pbrt/lights/infinite.h"

#include "iris/environmental_lights/image_environmental_light.h"
#include "third_party/stb/stb_image.h"
#include "third_party/tinyexr/tinyexr.h"

namespace iris {
namespace pbrt_frontend {
namespace lights {
namespace {

using ::iris::environmental_lights::ImageEnvironmentalLight;

static const std::unordered_map<std::string_view, Parameter::Type>
    g_parameters = {
        {"L", Parameter::SPECTRUM},
        {"mapname", Parameter::FILE_PATH},
        {"samples", Parameter::INTEGER},
        {"scale", Parameter::SPECTRUM},
};

static const Color kDefaultColor({1.0, 1.0, 1.0}, Color::Space::RGB);

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
  visual_t default_luma;
  std::vector<ReferenceCounted<Spectrum>> spectra = {
      spectrum_manager.AllocateSpectrum(kDefaultColor, &default_luma)};
  std::vector<visual> luma = {default_luma};
  std::pair<size_t, size_t> size(1, 1);

  // The interface for scaling the image should be cleaned up
  ReferenceCounted<Spectrum> scalar = spectra.front();

  auto l_iter = parameters.find("L");
  if (l_iter != parameters.end()) {
    scalar = l_iter->second.GetSpectra().front();
  }

  auto scale_iter = parameters.find("scale");
  if (scale_iter != parameters.end()) {
    scalar = spectrum_manager.AllocateSpectrum(
        scale_iter->second.GetSpectra().front(), scalar);
  }

  auto mapname_iter = parameters.find("mapname");
  if (mapname_iter != parameters.end()) {
    spectra.clear();
    luma.clear();

    const auto extension =
        mapname_iter->second.GetFilePaths().front().extension();
    if (extension == ".png") {
      std::cerr << "ERROR: Unimplemented image file type: .png" << std::endl;
      exit(EXIT_FAILURE);
    } else if (extension == ".tga") {
      std::cerr << "ERROR: Unimplemented image file type: .tga" << std::endl;
      exit(EXIT_FAILURE);
    } else if (extension == ".exr") {
      float* values;
      int width, height;
      const char* error_message;
      int error =
          LoadEXR(&values, &width, &height,
                  mapname_iter->second.GetFilePaths().front().native().c_str(),
                  &error_message);
      if (error < 0) {
        std::cerr << "ERROR: Image loading failed with error: " << error_message
                  << std::endl;
        exit(EXIT_FAILURE);
      }

      spectra.reserve(height * width);
      luma.reserve(height * width);
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

          ReferenceCounted<Spectrum> pixel_spectrum =
              spectrum_manager.AllocateSpectrum(Color{{r, g, b}, Color::RGB});

          visual_t luma_value;
          spectra.push_back(spectrum_manager.AllocateSpectrum(
              pixel_spectrum, scalar, &luma_value));
          luma.push_back(luma_value);
        }
      }

      free(values);

      size.first = static_cast<size_t>(height);
      size.second = static_cast<size_t>(width);
    } else {
      std::stringstream stream;
      if (extension.empty()) {
        stream << mapname_iter->second.GetFilePaths().front().filename();
        std::string filename = stream.str();
        std::cerr << "ERROR: Unsupported image file (no extension): "
                  << filename.substr(1, filename.size() - 2) << std::endl;
      } else {
        stream << extension;
        std::string ext = stream.str();
        std::cerr << "ERROR: Unsupported image file type: "
                  << ext.substr(1, ext.size() - 2) << std::endl;
      }

      exit(EXIT_FAILURE);
    }
  }

  return MakeReferenceCounted<ImageEnvironmentalLight>(std::move(spectra), luma,
                                                       size, model_to_world);
}

};  // namespace

extern const std::unique_ptr<const ObjectBuilder<
    std::variant<ReferenceCounted<Light>, ReferenceCounted<EnvironmentalLight>>,
    SpectrumManager&, const Matrix&>>
    g_infinite_builder = std::make_unique<InfiniteBuilder>();

}  // namespace lights
}  // namespace pbrt_frontend
}  // namespace iris