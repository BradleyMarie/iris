#include "frontends/pbrt/textures/imagemap.h"

#include <fstream>
#include <memory>
#include <optional>
#include <sstream>
#include <string_view>
#include <vector>

#include "iris/textures/image_texture.h"
#include "third_party/stb/stb_image.h"

namespace iris::pbrt_frontend::textures {
namespace {

static const std::unordered_map<std::string_view, Parameter::Type>
    g_float_parameters = {
        {"filename", Parameter::FILE_PATH}, {"gamma", Parameter::BOOL},
        {"mapping", Parameter::STRING},     {"maxanisotropy", Parameter::FLOAT},
        {"scale", Parameter::FLOAT},        {"udelta", Parameter::FLOAT},
        {"uscale", Parameter::FLOAT},       {"vdelta", Parameter::FLOAT},
        {"vscale", Parameter::FLOAT},       {"wrap", Parameter::STRING},
};

static const std::unordered_map<std::string_view, Parameter::Type>
    g_spectrum_parameters = {
        {"filename", Parameter::FILE_PATH}, {"gamma", Parameter::BOOL},
        {"mapping", Parameter::STRING},     {"maxanisotropy", Parameter::FLOAT},
        {"scale", Parameter::FLOAT},        {"udelta", Parameter::FLOAT},
        {"uscale", Parameter::FLOAT},       {"vdelta", Parameter::FLOAT},
        {"vscale", Parameter::FLOAT},       {"wrap", Parameter::STRING},
};

visual_t InverseGamma(visual_t value) {
  if (value <= static_cast<visual_t>(0.04045)) {
    return value / static_cast<visual_t>(12.92);
  }

  return std::pow(
      (value + static_cast<visual_t>(0.055)) / static_cast<visual_t>(1.055),
      static_cast<visual_t>(2.4));
}

enum class ImageWrapping {
  BLACK,
  CLAMP,
  REPEAT,
};

ImageWrapping ParseImageWrapping(const std::string& value) {
  if (value == "black") {
    return ImageWrapping::BLACK;
  }

  if (value == "clamp") {
    return ImageWrapping::CLAMP;
  }

  if (value == "repeat") {
    return ImageWrapping::REPEAT;
  }

  std::cerr << "ERROR: Out of range value for parameter: wrap" << std::endl;
  exit(EXIT_FAILURE);
}

enum class TextureMapping {
  CYLINDRICAL,
  PLANAR,
  SPHERICAL,
  UV,
};

TextureMapping ParseTextureMapping(const std::string& value) {
  if (value == "uv") {
    return TextureMapping::UV;
  }

  std::cerr << "ERROR: Out of range value for parameter: mapping" << std::endl;
  exit(EXIT_FAILURE);
}

struct Parameters {
  std::filesystem::path filename;
  bool inverse_gamma = true;
  TextureMapping mapping = TextureMapping::UV;
  visual_t maxanisotropy = 8.0;
  visual_t scale = 1.0;
  geometric u_delta = 0.0;
  geometric u_scale = 1.0;
  geometric v_delta = 0.0;
  geometric v_scale = 1.0;
  ImageWrapping wrap = ImageWrapping::REPEAT;
};

Parameters ParseParameters(
    const std::unordered_map<std::string_view, Parameter>& parameters) {
  Parameters result;

  auto filename_iter = parameters.find("filename");
  if (filename_iter == parameters.end()) {
    std::cerr << "ERROR: Missing required image parameter: filename"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  result.filename = filename_iter->second.GetFilePaths().front();

  auto mapping_iter = parameters.find("mapping");
  if (mapping_iter != parameters.end()) {
    result.mapping =
        ParseTextureMapping(mapping_iter->second.GetStringValues().front());
  }

  auto maxanisotropy_iter = parameters.find("maxanisotropy");
  if (maxanisotropy_iter != parameters.end()) {
    result.maxanisotropy = maxanisotropy_iter->second.GetFloatValues().front();
    if (!std::isfinite(result.maxanisotropy) ||
        result.maxanisotropy <= static_cast<visual_t>(0.0)) {
      std::cerr << "ERROR: Out of range value for parameter: maxanisotropy"
                << std::endl;
      exit(EXIT_FAILURE);
    }
  }

  auto scale_iter = parameters.find("scale");
  if (scale_iter != parameters.end()) {
    result.scale = scale_iter->second.GetFloatValues().front();
    if (!std::isfinite(result.scale)) {
      std::cerr << "ERROR: Out of range value for parameter: scale"
                << std::endl;
      exit(EXIT_FAILURE);
    }
  }

  auto u_delta_iter = parameters.find("udelta");
  if (u_delta_iter != parameters.end()) {
    result.u_delta = u_delta_iter->second.GetFloatValues().front();
    if (!std::isfinite(result.u_delta)) {
      std::cerr << "ERROR: Out of range value for parameter: udelta"
                << std::endl;
      exit(EXIT_FAILURE);
    }
  }

  auto u_scale_iter = parameters.find("uscale");
  if (u_scale_iter != parameters.end()) {
    result.u_scale = u_scale_iter->second.GetFloatValues().front();
    if (!std::isfinite(result.u_scale) || result.u_scale == 0.0) {
      std::cerr << "ERROR: Out of range value for parameter: uscale"
                << std::endl;
      exit(EXIT_FAILURE);
    }
  }

  auto v_delta_iter = parameters.find("vdelta");
  if (v_delta_iter != parameters.end()) {
    result.v_delta = v_delta_iter->second.GetFloatValues().front();
    if (!std::isfinite(result.v_delta)) {
      std::cerr << "ERROR: Out of range value for parameter: vdelta"
                << std::endl;
      exit(EXIT_FAILURE);
    }
  }

  auto v_scale_iter = parameters.find("vscale");
  if (v_scale_iter != parameters.end()) {
    result.v_scale = v_scale_iter->second.GetFloatValues().front();
    if (!std::isfinite(result.v_scale) || result.v_scale == 0.0) {
      std::cerr << "ERROR: Out of range value for parameter: vscale"
                << std::endl;
      exit(EXIT_FAILURE);
    }
  }

  auto wrap_iter = parameters.find("wrap");
  if (wrap_iter != parameters.end()) {
    result.wrap =
        ParseImageWrapping(wrap_iter->second.GetStringValues().front());
  }

  auto extension = result.filename.extension();
  if (extension == ".png" || extension == ".tga") {
    auto gamma_iter = parameters.find("gamma");
    if (gamma_iter != parameters.end()) {
      if (!gamma_iter->second.GetBoolValues().front()) {
        result.inverse_gamma = false;
      }
    }
  } else {
    std::stringstream stream;
    if (extension.empty()) {
      stream << result.filename.filename();
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

  return result;
}

class ImageFloatTextureBuilder final
    : public ObjectBuilder<void, TextureManager&, SpectrumManager&,
                           const std::string&> {
 public:
  ImageFloatTextureBuilder() : ObjectBuilder(g_float_parameters) {}

  void Build(const std::unordered_map<std::string_view, Parameter>& parameters,
             TextureManager& texture_manager, SpectrumManager& spectrum_manager,
             const std::string& name) const override {
    Parameters parsed_params = ParseParameters(parameters);

    stbi_ldr_to_hdr_gamma(1.0);

    std::ifstream stream(parsed_params.filename.native(),
                         std::ios::in | std::ios::binary);

    std::vector<stbi_uc> contents;
    char c;
    while (stream.get(c)) {
      contents.push_back(static_cast<stbi_uc>(c));
    }

    if (contents.size() > std::numeric_limits<int>::max()) {
      std::cerr << "ERROR: Image loading failed with error: file is too large"
                << std::endl;
      exit(EXIT_FAILURE);
    }

    int nx, ny;
    float* values = stbi_loadf_from_memory(
        contents.data(), static_cast<int>(contents.size()), &nx, &ny, nullptr,
        /*desired_channels=*/1);
    if (!values) {
      std::cerr << "ERROR: Image loading failed with error: "
                << stbi_failure_reason() << std::endl;
      exit(EXIT_FAILURE);
    }

    std::vector<visual> scaled_values;
    scaled_values.reserve(nx * ny);

    for (int y = 0; y < ny; y++) {
      for (int x = 0; x < nx; x++) {
        visual_t value = values[(ny - y - 1) * nx + x];
        if (parsed_params.inverse_gamma) {
          value = InverseGamma(value);
        }
        scaled_values.push_back(value * parsed_params.scale);
      }
    }

    stbi_image_free(values);

    auto image = std::make_shared<iris::textures::Image2D<visual>>(
        std::move(scaled_values),
        std::make_pair(static_cast<size_t>(ny), static_cast<size_t>(nx)));

    switch (parsed_params.wrap) {
      case ImageWrapping::BLACK:
        texture_manager.Put(
            name, iris::MakeReferenceCounted<
                      iris::textures::BorderedImageTexture2D<visual>>(
                      std::move(image), static_cast<iris::geometric>(0.0),
                      parsed_params.u_scale, parsed_params.v_scale,
                      parsed_params.u_delta, parsed_params.v_delta));
        break;
      case ImageWrapping::CLAMP:
        texture_manager.Put(
            name,
            iris::MakeReferenceCounted<
                iris::textures::ClampedImageTexture2D<visual>>(
                std::move(image), parsed_params.u_scale, parsed_params.v_scale,
                parsed_params.u_delta, parsed_params.v_delta));
        break;
      case ImageWrapping::REPEAT:
        texture_manager.Put(
            name,
            iris::MakeReferenceCounted<
                iris::textures::RepeatedImageTexture2D<visual>>(
                std::move(image), parsed_params.u_scale, parsed_params.v_scale,
                parsed_params.u_delta, parsed_params.v_delta));
        break;
    };
  }
};

class ImageSpectrumTextureBuilder final
    : public ObjectBuilder<void, TextureManager&, SpectrumManager&,
                           const std::string&> {
 public:
  ImageSpectrumTextureBuilder() : ObjectBuilder(g_spectrum_parameters) {}

  void Build(const std::unordered_map<std::string_view, Parameter>& parameters,
             TextureManager& texture_manager, SpectrumManager& spectrum_manager,
             const std::string& name) const override {
    Parameters parsed_params = ParseParameters(parameters);

    if (parsed_params.scale < 0.0 || parsed_params.scale > 1.0) {
      std::cerr << "ERROR: Out of range value for parameter: scale"
                << std::endl;
      exit(EXIT_FAILURE);
    }

    stbi_ldr_to_hdr_gamma(1.0);

    std::ifstream stream(parsed_params.filename.native(),
                         std::ios::in | std::ios::binary);

    std::vector<stbi_uc> contents;
    char c;
    while (stream.get(c)) {
      contents.push_back(static_cast<stbi_uc>(c));
    }

    if (contents.size() > std::numeric_limits<int>::max()) {
      std::cerr << "ERROR: Image loading failed with error: file is too large"
                << std::endl;
      exit(EXIT_FAILURE);
    }

    int nx, ny, channels;
    float* values = stbi_loadf_from_memory(
        contents.data(), static_cast<int>(contents.size()), &nx, &ny, &channels,
        /*desired_channels=*/3);
    if (!values) {
      std::cerr << "ERROR: Image loading failed with error: "
                << stbi_failure_reason() << std::endl;
      exit(EXIT_FAILURE);
    }

    std::vector<ReferenceCounted<Reflector>> scaled_reflectors;
    scaled_reflectors.reserve(nx * ny);

    for (int y = 0; y < ny; y++) {
      for (int x = 0; x < nx; x++) {
        visual_t r = values[3 * ((ny - y - 1) * nx + x) + 0];
        visual_t g = values[3 * ((ny - y - 1) * nx + x) + 1];
        visual_t b = values[3 * ((ny - y - 1) * nx + x) + 2];

        if (parsed_params.inverse_gamma) {
          r = InverseGamma(r);
          g = InverseGamma(g);
          b = InverseGamma(b);
        }

        if (r < 0.0 || r > 1.0 || g < 0.0 || g > 1.0 || b < 0.0 || b > 1.0) {
          std::cerr << "ERROR: Image file contained an out of range value"
                    << stbi_failure_reason() << std::endl;
          exit(EXIT_FAILURE);
        }

        if (channels == 1) {
          scaled_reflectors.push_back(
              texture_manager.AllocateUniformReflector(r));
        } else {
          scaled_reflectors.push_back(
              spectrum_manager.AllocateReflector(Color({r, g, b}, Color::RGB)));
        }
      }
    }

    stbi_image_free(values);

    auto image =
        std::make_shared<iris::textures::Image2D<ReferenceCounted<Reflector>>>(
            std::move(scaled_reflectors),
            std::make_pair(static_cast<size_t>(ny), static_cast<size_t>(nx)));

    switch (parsed_params.wrap) {
      case ImageWrapping::BLACK:
        texture_manager.Put(name,
                            iris::MakeReferenceCounted<
                                iris::textures::BorderedSpectralImageTexture2D<
                                    Reflector, SpectralAllocator>>(
                                std::move(image), ReferenceCounted<Reflector>(),
                                parsed_params.u_scale, parsed_params.v_scale,
                                parsed_params.u_delta, parsed_params.v_delta));
        break;
      case ImageWrapping::CLAMP:
        texture_manager.Put(
            name,
            iris::MakeReferenceCounted<
                iris::textures::ClampedSpectralImageTexture2D<
                    Reflector, SpectralAllocator>>(
                std::move(image), parsed_params.u_scale, parsed_params.v_scale,
                parsed_params.u_delta, parsed_params.v_delta));
        break;
      case ImageWrapping::REPEAT:
        texture_manager.Put(
            name,
            iris::MakeReferenceCounted<
                iris::textures::RepeatedSpectralImageTexture2D<
                    Reflector, SpectralAllocator>>(
                std::move(image), parsed_params.u_scale, parsed_params.v_scale,
                parsed_params.u_delta, parsed_params.v_delta));
        break;
    };
  }
};

}  // namespace

const std::unique_ptr<const ObjectBuilder<void, TextureManager&,
                                          SpectrumManager&, const std::string&>>
    g_float_imagemap_builder = std::make_unique<ImageFloatTextureBuilder>();
const std::unique_ptr<const ObjectBuilder<void, TextureManager&,
                                          SpectrumManager&, const std::string&>>
    g_spectrum_imagemap_builder =
        std::make_unique<ImageSpectrumTextureBuilder>();

}  // namespace iris::pbrt_frontend::textures