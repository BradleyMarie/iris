#include "frontends/pbrt/textures/imagemap.h"

#include <memory>
#include <optional>
#include <string_view>
#include <vector>

#include "iris/textures/image_texture.h"
#include "third_party/stb/stb_image.h"

namespace iris::pbrt_frontend::textures {
namespace {

static const std::unordered_map<std::string_view, Parameter::Type>
    g_float_parameters = {
        {"filename", Parameter::FILE_PATH}, {"gamma", Parameter::BOOL},
        {"scale", Parameter::FLOAT},        {"udelta", Parameter::FLOAT},
        {"uscale", Parameter::FLOAT},       {"vdelta", Parameter::FLOAT},
        {"vscale", Parameter::FLOAT},       {"wrap", Parameter::STRING},
};

static const std::unordered_map<std::string_view, Parameter::Type>
    g_spectrum_parameters = {
        {"filename", Parameter::FILE_PATH}, {"gamma", Parameter::BOOL},
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

struct Parameters {
  const char* filename = nullptr;
  float gamma = 2.2;
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

  result.filename = filename_iter->second.GetFilePaths().front().c_str();

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

  const char* filename = filename_iter->second.GetFilePaths().front().c_str();
  std::string_view filename_view = filename;
  if (filename_view.ends_with(".png") || filename_view.ends_with(".tga")) {
    auto gamma_iter = parameters.find("gamma");
    if (gamma_iter != parameters.end()) {
      if (!gamma_iter->second.GetBoolValues().front()) {
        result.gamma = 1.0;
      }
    }
  } else {
    std::string_view extension =
        filename_iter->second.GetFilePaths().front().extension().c_str();
    if (extension.empty()) {
      std::cerr
          << "ERROR: Unsupported image file (no extension): "
          << filename_iter->second.GetFilePaths().front().filename().c_str()
          << std::endl;
    } else {
      std::cerr << "ERROR: Unsupported image file type: " << extension
                << std::endl;
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

    int nx, ny;
    float* values = stbi_loadf(parsed_params.filename, &nx, &ny, nullptr,
                               /*desired_channels=*/1);
    if (!values) {
      std::cerr << "ERROR: Image loading failed with error: "
                << stbi_failure_reason() << std::endl;
      exit(EXIT_FAILURE);
    }

    std::vector<visual> scaled_values;
    for (int y = 0; y < ny; y++) {
      for (int x = 0; x < nx; x++) {
        visual_t value = InverseGamma(values[(ny - y - 1) * nx + x]);
        scaled_values.push_back(value * parsed_params.scale);
      }
    }

    stbi_image_free(values);

    auto image = std::make_shared<iris::textures::Image2D<visual>>(
        std::move(scaled_values),
        std::make_pair(static_cast<size_t>(ny), static_cast<size_t>(nx)));

    switch (parsed_params.wrap) {
      case ImageWrapping::BLACK:
        texture_manager.Put(name,
                            iris::MakeReferenceCounted<
                                iris::textures::BorderedImageTexture2D<visual>>(
                                std::move(image), 0.0, parsed_params.u_scale,
                                parsed_params.v_scale, parsed_params.u_delta,
                                parsed_params.v_delta));
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

    int nx, ny, channels;
    float* values = stbi_loadf(parsed_params.filename, &nx, &ny, &channels,
                               /*desired_channels=*/3);
    if (!values) {
      std::cerr << "ERROR: Image loading failed with error: "
                << stbi_failure_reason() << std::endl;
      exit(EXIT_FAILURE);
    }

    std::vector<ReferenceCounted<Reflector>> scaled_reflectors;
    for (int y = 0; y < ny; y++) {
      for (int x = 0; x < nx; x++) {
        visual_t r = InverseGamma(values[3 * ((ny - y - 1) * nx + x) + 0]) *
                     parsed_params.scale;
        visual_t g = InverseGamma(values[3 * ((ny - y - 1) * nx + x) + 1]) *
                     parsed_params.scale;
        visual_t b = InverseGamma(values[3 * ((ny - y - 1) * nx + x) + 2]) *
                     parsed_params.scale;
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