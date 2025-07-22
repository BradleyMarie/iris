#include "frontends/pbrt/textures/parse.h"

#include <cstdlib>
#include <iostream>

#include "frontends/pbrt/image_manager.h"
#include "frontends/pbrt/spectrum_manager.h"
#include "frontends/pbrt/texture_manager.h"
#include "frontends/pbrt/textures/constant.h"
#include "frontends/pbrt/textures/imagemap.h"
#include "frontends/pbrt/textures/mix.h"
#include "frontends/pbrt/textures/scale.h"
#include "iris/reference_counted.h"
#include "iris/reflector.h"
#include "iris/spectral_allocator.h"
#include "iris/textures/float_texture.h"
#include "iris/textures/reflector_texture.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {

using ::pbrt_proto::v3::FloatTexture;
using ::pbrt_proto::v3::SpectrumTexture;

void ParseFloatTexture(const FloatTexture& float_texture,
                       ImageManager& image_manager,
                       TextureManager& texture_manager) {
  ReferenceCounted<iris::textures::FloatTexture> result;
  switch (float_texture.float_texture_type_case()) {
    case FloatTexture::kBilerp:
      std::cerr << "ERROR: Unsupported FloatTexture type: bilerp" << std::endl;
      exit(EXIT_FAILURE);
      break;
    case FloatTexture::kCheckerboard2D:
      std::cerr << "ERROR: Unsupported FloatTexture type: checkerboard"
                << std::endl;
      exit(EXIT_FAILURE);
      break;
    case FloatTexture::kCheckerboard3D:
      std::cerr << "ERROR: Unsupported FloatTexture type: checkerboard"
                << std::endl;
      exit(EXIT_FAILURE);
      break;
    case FloatTexture::kConstant:
      result =
          textures::MakeConstant(float_texture.constant(), texture_manager);
      break;
    case FloatTexture::kDots:
      std::cerr << "ERROR: Unsupported FloatTexture type: dots" << std::endl;
      exit(EXIT_FAILURE);
      break;
    case FloatTexture::kFbm:
      std::cerr << "ERROR: Unsupported FloatTexture type: fbm" << std::endl;
      exit(EXIT_FAILURE);
      break;
    case FloatTexture::kImagemap:
      result = textures::MakeImageMap(float_texture.imagemap(), image_manager,
                                      texture_manager);
      break;
    case FloatTexture::kMarble:
      std::cerr << "ERROR: Unsupported FloatTexture type: marble" << std::endl;
      exit(EXIT_FAILURE);
      break;
    case FloatTexture::kMix:
      result = textures::MakeMix(float_texture.mix(), texture_manager);
      break;
    case FloatTexture::kPtex:
      std::cerr << "ERROR: Unsupported FloatTexture type: ptex" << std::endl;
      exit(EXIT_FAILURE);
      break;
    case FloatTexture::kScale:
      result = textures::MakeScale(float_texture.scale(), texture_manager);
      break;
    case FloatTexture::kWindy:
      std::cerr << "ERROR: Unsupported FloatTexture type: windy" << std::endl;
      exit(EXIT_FAILURE);
      break;
    case FloatTexture::kWrinkled:
      std::cerr << "ERROR: Unsupported FloatTexture type: wrinkled"
                << std::endl;
      exit(EXIT_FAILURE);
      break;
    case FloatTexture::FLOAT_TEXTURE_TYPE_NOT_SET:
      return;
  }

  texture_manager.Put(float_texture.name(), std::move(result));
}

void ParseSpectrumTexture(const SpectrumTexture& spectrum_texture,
                          ImageManager& image_manager,
                          TextureManager& texture_manager) {
  ReferenceCounted<iris::textures::ReflectorTexture> result;
  switch (spectrum_texture.spectrum_texture_type_case()) {
    case SpectrumTexture::kBilerp:
      std::cerr << "ERROR: Unsupported SpectrumTexture type: bilerp"
                << std::endl;
      exit(EXIT_FAILURE);
      break;
    case SpectrumTexture::kCheckerboard2D:
      std::cerr << "ERROR: Unsupported SpectrumTexture type: checkerboard"
                << std::endl;
      exit(EXIT_FAILURE);
      break;
    case SpectrumTexture::kCheckerboard3D:
      std::cerr << "ERROR: Unsupported SpectrumTexture type: checkerboard"
                << std::endl;
      exit(EXIT_FAILURE);
      break;
    case SpectrumTexture::kConstant:
      result =
          textures::MakeConstant(spectrum_texture.constant(), texture_manager);
      break;
    case SpectrumTexture::kDots:
      std::cerr << "ERROR: Unsupported SpectrumTexture type: dots" << std::endl;
      exit(EXIT_FAILURE);
      break;
    case SpectrumTexture::kFbm:
      std::cerr << "ERROR: Unsupported SpectrumTexture type: fbm" << std::endl;
      exit(EXIT_FAILURE);
      break;
    case SpectrumTexture::kImagemap:
      result = textures::MakeImageMap(spectrum_texture.imagemap(),
                                      image_manager, texture_manager);
      break;
    case SpectrumTexture::kMarble:
      std::cerr << "ERROR: Unsupported SpectrumTexture type: marble"
                << std::endl;
      exit(EXIT_FAILURE);
      break;
    case SpectrumTexture::kMix:
      result = textures::MakeMix(spectrum_texture.mix(), texture_manager);
      break;
    case SpectrumTexture::kPtex:
      std::cerr << "ERROR: Unsupported SpectrumTexture type: ptex" << std::endl;
      exit(EXIT_FAILURE);
      break;
    case SpectrumTexture::kScale:
      result = textures::MakeScale(spectrum_texture.scale(), texture_manager);
      break;
    case SpectrumTexture::kUv:
      std::cerr << "ERROR: Unsupported SpectrumTexture type: uv" << std::endl;
      exit(EXIT_FAILURE);
      break;
    case SpectrumTexture::kWindy:
      std::cerr << "ERROR: Unsupported SpectrumTexture type: windy"
                << std::endl;
      exit(EXIT_FAILURE);
      break;
    case SpectrumTexture::kWrinkled:
      std::cerr << "ERROR: Unsupported SpectrumTexture type: wrinkled"
                << std::endl;
      exit(EXIT_FAILURE);
      break;
    case SpectrumTexture::SPECTRUM_TEXTURE_TYPE_NOT_SET:
      return;
  }

  texture_manager.Put(spectrum_texture.name(), std::move(result));
}

}  // namespace pbrt_frontend
}  // namespace iris
