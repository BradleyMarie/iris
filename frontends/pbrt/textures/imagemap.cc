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
#include "iris/textures/image_texture.h"
#include "iris/textures/scaled_texture.h"
#include "iris/textures/texture2d.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace textures {

using ::iris::textures::BorderedImageTexture2D;
using ::iris::textures::BorderedSpectralImageTexture2D;
using ::iris::textures::ClampedImageTexture2D;
using ::iris::textures::ClampedSpectralImageTexture2D;
using ::iris::textures::PointerTexture2D;
using ::iris::textures::RepeatedImageTexture2D;
using ::iris::textures::RepeatedSpectralImageTexture2D;
using ::iris::textures::ScaledSpectralTexture2D;
using ::iris::textures::ScaledValueTexture2D;
using ::iris::textures::ValueTexture2D;
using ::pbrt_proto::v3::FloatTexture;
using ::pbrt_proto::v3::ImageWrapping;
using ::pbrt_proto::v3::SpectrumTexture;

ReferenceCounted<ValueTexture2D<visual>> MakeImageMap(
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

  std::filesystem::path path = with_defaults.filename();
  if (!std::filesystem::is_regular_file(path)) {
    std::cerr << "ERROR: Missing required image parameter: filename"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  if (path.extension() == ".png" || path.extension() == ".tga") {
    if (!with_defaults.has_gamma()) {
      with_defaults.set_gamma(true);
    }
  } else {
    if (!with_defaults.has_gamma()) {
      with_defaults.set_gamma(false);
    }

    std::stringstream stream;
    if (path.extension().empty()) {
      stream << path.filename();
      std::string filename = stream.str();
      std::cerr << "ERROR: Unsupported image file (no extension): "
                << filename.substr(1, filename.size() - 2) << std::endl;
    } else {
      stream << path.extension();
      std::string ext = stream.str();
      std::cerr << "ERROR: Unsupported image file type: "
                << ext.substr(1, ext.size() - 2) << std::endl;
    }

    exit(EXIT_FAILURE);
  }

  auto image = image_manager.LoadFloatImageFromSDR(
      path, static_cast<visual>(with_defaults.gamma()));

  ReferenceCounted<ValueTexture2D<visual>> texture;
  switch (with_defaults.wrap()) {
    case ImageWrapping::BLACK:
      texture = MakeReferenceCounted<BorderedImageTexture2D<visual>>(
          std::move(image), static_cast<visual>(0.0),
          static_cast<visual>(with_defaults.uscale()),
          static_cast<visual>(with_defaults.vscale()),
          static_cast<visual>(with_defaults.udelta()),
          static_cast<visual>(with_defaults.vdelta()));
      break;
    case ImageWrapping::CLAMP:
      texture = MakeReferenceCounted<ClampedImageTexture2D<visual>>(
          std::move(image), static_cast<visual>(with_defaults.uscale()),
          static_cast<visual>(with_defaults.vscale()),
          static_cast<visual>(with_defaults.udelta()),
          static_cast<visual>(with_defaults.vdelta()));
      break;
    case ImageWrapping::REPEAT:
      texture = MakeReferenceCounted<RepeatedImageTexture2D<visual>>(
          std::move(image), static_cast<visual>(with_defaults.uscale()),
          static_cast<visual>(with_defaults.vscale()),
          static_cast<visual>(with_defaults.udelta()),
          static_cast<visual>(with_defaults.vdelta()));
      break;
  };

  if (with_defaults.scale() != static_cast<visual_t>(1.0)) {
    texture = MakeReferenceCounted<ScaledValueTexture2D<visual>>(
        texture_manager.AllocateFloatTexture(
            static_cast<visual>(with_defaults.scale())),
        std::move(texture));
  }

  return texture;
}

ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> MakeImageMap(
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

  std::filesystem::path path = with_defaults.filename();
  if (!std::filesystem::is_regular_file(path)) {
    std::cerr << "ERROR: Missing required image parameter: filename"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  if (path.extension() == ".png" || path.extension() == ".tga") {
    if (!with_defaults.has_gamma()) {
      with_defaults.set_gamma(true);
    }
  } else {
    if (!with_defaults.has_gamma()) {
      with_defaults.set_gamma(false);
    }

    std::stringstream stream;
    if (path.extension().empty()) {
      stream << path.filename();
      std::string filename = stream.str();
      std::cerr << "ERROR: Unsupported image file (no extension): "
                << filename.substr(1, filename.size() - 2) << std::endl;
    } else {
      stream << path.extension();
      std::string ext = stream.str();
      std::cerr << "ERROR: Unsupported image file type: "
                << ext.substr(1, ext.size() - 2) << std::endl;
    }

    exit(EXIT_FAILURE);
  }

  auto image = image_manager.LoadReflectorImageFromSDR(
      path, static_cast<visual>(with_defaults.gamma()));

  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> texture;
  switch (with_defaults.wrap()) {
    case ImageWrapping::BLACK:
      texture = MakeReferenceCounted<
          BorderedSpectralImageTexture2D<Reflector, SpectralAllocator>>(
          std::move(image), ReferenceCounted<Reflector>(),
          static_cast<visual>(with_defaults.uscale()),
          static_cast<visual>(with_defaults.vscale()),
          static_cast<visual>(with_defaults.udelta()),
          static_cast<visual>(with_defaults.vdelta()));
      break;
    case ImageWrapping::CLAMP:
      texture = iris::MakeReferenceCounted<
          ClampedSpectralImageTexture2D<Reflector, SpectralAllocator>>(
          std::move(image), static_cast<visual>(with_defaults.uscale()),
          static_cast<visual>(with_defaults.vscale()),
          static_cast<visual>(with_defaults.udelta()),
          static_cast<visual>(with_defaults.vdelta()));
      break;
    case ImageWrapping::REPEAT:
      texture = iris::MakeReferenceCounted<
          RepeatedSpectralImageTexture2D<Reflector, SpectralAllocator>>(
          std::move(image), static_cast<visual>(with_defaults.uscale()),
          static_cast<visual>(with_defaults.vscale()),
          static_cast<visual>(with_defaults.udelta()),
          static_cast<visual>(with_defaults.vdelta()));
      break;
  };

  if (with_defaults.scale() != static_cast<visual_t>(1.0)) {
    texture = MakeReferenceCounted<ScaledSpectralTexture2D<Reflector>>(
        texture_manager.AllocateReflectorTexture(
            static_cast<visual>(with_defaults.scale())),
        std::move(texture));
  }

  return texture;
}

}  // namespace textures
}  // namespace pbrt_frontend
}  // namespace iris
