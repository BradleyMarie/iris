#include "frontends/pbrt/textures/imagemap.h"

#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <utility>

#include "frontends/pbrt/defaults.h"
#include "frontends/pbrt/image_manager.h"
#include "frontends/pbrt/texture_manager.h"
#include "iris/reference_counted.h"
#include "iris/reflector.h"
#include "iris/spectral_allocator.h"
#include "iris/textures/float_texture.h"
#include "iris/textures/image_texture.h"
#include "iris/textures/reflector_texture.h"
#include "iris/textures/scaled_texture.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace textures {

using ::iris::textures::MakeBorderedImageTexture;
using ::iris::textures::MakeClampedImageTexture;
using ::iris::textures::MakeRepeatedImageTexture;
using ::iris::textures::MakeScaledTexture;
using ::pbrt_proto::v3::FloatTexture;
using ::pbrt_proto::v3::ImageWrapping;
using ::pbrt_proto::v3::SpectrumTexture;

ReferenceCounted<iris::textures::FloatTexture> MakeImageMap(
    const FloatTexture::ImageMap& imagemap, ImageManager& image_manager,
    TextureManager& texture_manager) {
  FloatTexture::ImageMap with_defaults = Defaults().float_textures().imagemap();
  with_defaults.MergeFrom(imagemap);

  if (!std::isfinite(with_defaults.maxanisotropy()) ||
      with_defaults.maxanisotropy() <= static_cast<visual_t>(0.0)) {
    std::cerr << "ERROR: Out of range value for parameter: maxanisotropy"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  if (!std::isfinite(with_defaults.scale())) {
    std::cerr << "ERROR: Out of range value for parameter: scale" << std::endl;
    exit(EXIT_FAILURE);
  }

  if (!std::isfinite(with_defaults.udelta())) {
    std::cerr << "ERROR: Out of range value for parameter: udelta" << std::endl;
    exit(EXIT_FAILURE);
  }

  if (!std::isfinite(with_defaults.uscale()) || with_defaults.uscale() == 0.0) {
    std::cerr << "ERROR: Out of range value for parameter: uscale" << std::endl;
    exit(EXIT_FAILURE);
  }

  if (!std::isfinite(with_defaults.vdelta())) {
    std::cerr << "ERROR: Out of range value for parameter: vdelta" << std::endl;
    exit(EXIT_FAILURE);
  }

  if (!std::isfinite(with_defaults.vscale()) || with_defaults.vscale() == 0.0) {
    std::cerr << "ERROR: Out of range value for parameter: vscale" << std::endl;
    exit(EXIT_FAILURE);
  }

  if (with_defaults.filename().empty()) {
    std::cerr << "ERROR: Missing required image parameter: filename"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  if (with_defaults.filename().ends_with(".png") ||
      with_defaults.filename().ends_with(".tga")) {
    if (!with_defaults.has_gamma()) {
      with_defaults.set_gamma(true);
    }
  } else {
    if (!with_defaults.has_gamma()) {
      with_defaults.set_gamma(false);
    }

    std::filesystem::path parsed(with_defaults.filename());

    std::stringstream stream;
    if (parsed.extension().empty()) {
      stream << parsed.filename();
      std::string filename = stream.str();
      std::cerr << "ERROR: Unsupported image file (no extension): "
                << filename.substr(1, filename.size() - 2) << std::endl;
    } else {
      stream << parsed.extension();
      std::string ext = stream.str();
      std::cerr << "ERROR: Unsupported image file type: "
                << ext.substr(1, ext.size() - 2) << std::endl;
    }

    exit(EXIT_FAILURE);
  }

  auto image = image_manager.LoadFloatImageFromSDR(
      with_defaults.filename(), static_cast<visual>(with_defaults.gamma()));

  ReferenceCounted<iris::textures::FloatTexture> texture;
  switch (with_defaults.wrap()) {
    case ImageWrapping::BLACK:
      texture =
          MakeBorderedImageTexture(std::move(image), static_cast<visual>(0.0),
                                   static_cast<visual>(with_defaults.uscale()),
                                   static_cast<visual>(with_defaults.vscale()),
                                   static_cast<visual>(with_defaults.udelta()),
                                   static_cast<visual>(with_defaults.vdelta()));
      break;
    case ImageWrapping::CLAMP:
      texture = MakeClampedImageTexture(
          std::move(image), static_cast<visual>(with_defaults.uscale()),
          static_cast<visual>(with_defaults.vscale()),
          static_cast<visual>(with_defaults.udelta()),
          static_cast<visual>(with_defaults.vdelta()));
      break;
    case ImageWrapping::REPEAT:
      texture = MakeRepeatedImageTexture(
          std::move(image), static_cast<visual>(with_defaults.uscale()),
          static_cast<visual>(with_defaults.vscale()),
          static_cast<visual>(with_defaults.udelta()),
          static_cast<visual>(with_defaults.vdelta()));
      break;
  };

  if (with_defaults.scale() != static_cast<visual_t>(1.0)) {
    texture = MakeScaledTexture(texture_manager.AllocateFloatTexture(
                                    static_cast<visual>(with_defaults.scale())),
                                std::move(texture));
  }

  return texture;
}

ReferenceCounted<iris::textures::ReflectorTexture> MakeImageMap(
    const SpectrumTexture::ImageMap& imagemap, ImageManager& image_manager,
    TextureManager& texture_manager) {
  SpectrumTexture::ImageMap with_defaults =
      Defaults().spectrum_textures().imagemap();
  with_defaults.MergeFrom(imagemap);

  if (!std::isfinite(with_defaults.maxanisotropy()) ||
      with_defaults.maxanisotropy() <= static_cast<visual_t>(0.0)) {
    std::cerr << "ERROR: Out of range value for parameter: maxanisotropy"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  if (!std::isfinite(with_defaults.scale())) {
    std::cerr << "ERROR: Out of range value for parameter: scale" << std::endl;
    exit(EXIT_FAILURE);
  }

  if (!std::isfinite(with_defaults.udelta())) {
    std::cerr << "ERROR: Out of range value for parameter: udelta" << std::endl;
    exit(EXIT_FAILURE);
  }

  if (!std::isfinite(with_defaults.uscale()) || with_defaults.uscale() == 0.0) {
    std::cerr << "ERROR: Out of range value for parameter: uscale" << std::endl;
    exit(EXIT_FAILURE);
  }

  if (!std::isfinite(with_defaults.vdelta())) {
    std::cerr << "ERROR: Out of range value for parameter: vdelta" << std::endl;
    exit(EXIT_FAILURE);
  }

  if (!std::isfinite(with_defaults.vscale()) || with_defaults.vscale() == 0.0) {
    std::cerr << "ERROR: Out of range value for parameter: vscale" << std::endl;
    exit(EXIT_FAILURE);
  }

  if (with_defaults.filename().empty()) {
    std::cerr << "ERROR: Missing required image parameter: filename"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  if (with_defaults.filename().ends_with(".png") ||
      with_defaults.filename().ends_with(".tga")) {
    if (!with_defaults.has_gamma()) {
      with_defaults.set_gamma(true);
    }
  } else {
    if (!with_defaults.has_gamma()) {
      with_defaults.set_gamma(false);
    }

    std::filesystem::path parsed(with_defaults.filename());

    std::stringstream stream;
    if (parsed.extension().empty()) {
      stream << parsed.filename();
      std::string filename = stream.str();
      std::cerr << "ERROR: Unsupported image file (no extension): "
                << filename.substr(1, filename.size() - 2) << std::endl;
    } else {
      stream << parsed.extension();
      std::string ext = stream.str();
      std::cerr << "ERROR: Unsupported image file type: "
                << ext.substr(1, ext.size() - 2) << std::endl;
    }

    exit(EXIT_FAILURE);
  }

  auto image = image_manager.LoadReflectorImageFromSDR(
      with_defaults.filename(), static_cast<visual>(with_defaults.gamma()));

  ReferenceCounted<iris::textures::ReflectorTexture> texture;
  switch (with_defaults.wrap()) {
    case ImageWrapping::BLACK:
      texture = MakeBorderedImageTexture(
          std::move(image), ReferenceCounted<Reflector>(),
          static_cast<visual>(with_defaults.uscale()),
          static_cast<visual>(with_defaults.vscale()),
          static_cast<visual>(with_defaults.udelta()),
          static_cast<visual>(with_defaults.vdelta()));
      break;
    case ImageWrapping::CLAMP:
      texture = MakeClampedImageTexture(
          std::move(image), static_cast<visual>(with_defaults.uscale()),
          static_cast<visual>(with_defaults.vscale()),
          static_cast<visual>(with_defaults.udelta()),
          static_cast<visual>(with_defaults.vdelta()));
      break;
    case ImageWrapping::REPEAT:
      texture = MakeRepeatedImageTexture(
          std::move(image), static_cast<visual>(with_defaults.uscale()),
          static_cast<visual>(with_defaults.vscale()),
          static_cast<visual>(with_defaults.udelta()),
          static_cast<visual>(with_defaults.vdelta()));
      break;
  };

  if (with_defaults.scale() != static_cast<visual_t>(1.0)) {
    texture = MakeScaledTexture(texture_manager.AllocateReflectorTexture(
                                    static_cast<visual>(with_defaults.scale())),
                                std::move(texture));
  }

  return texture;
}

}  // namespace textures
}  // namespace pbrt_frontend
}  // namespace iris
