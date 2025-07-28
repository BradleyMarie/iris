#include "frontends/pbrt/textures/parse.h"

#include <cstdlib>
#include <iostream>

#include "frontends/pbrt/image_manager.h"
#include "frontends/pbrt/texture_manager.h"
#include "frontends/pbrt/textures/constant.h"
#include "frontends/pbrt/textures/fbm.h"
#include "frontends/pbrt/textures/imagemap.h"
#include "frontends/pbrt/textures/marble.h"
#include "frontends/pbrt/textures/mix.h"
#include "frontends/pbrt/textures/ptex.h"
#include "frontends/pbrt/textures/scale.h"
#include "frontends/pbrt/textures/windy.h"
#include "frontends/pbrt/textures/wrinkled.h"
#include "iris/matrix.h"
#include "iris/reference_counted.h"
#include "iris/textures/float_texture.h"
#include "iris/textures/reflector_texture.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {

using ::pbrt_proto::v3::FloatTexture;
using ::pbrt_proto::v3::SpectrumTexture;

void ParseFloatTexture(const FloatTexture& float_texture,
                       ImageManager& image_manager,
                       TextureManager& texture_manager,
                       const Matrix& world_to_texture) {
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
      result = textures::MakeFbm(float_texture.fbm(), texture_manager,
                                 world_to_texture);
      break;
    case FloatTexture::kImagemap:
      result = textures::MakeImageMap(float_texture.imagemap(), image_manager,
                                      texture_manager);
      break;
    case FloatTexture::kMarble:
      return;
    case FloatTexture::kMix:
      result = textures::MakeMix(float_texture.mix(), texture_manager);
      break;
    case FloatTexture::kPtex:
      result = textures::MakePtex(float_texture.ptex(), image_manager,
                                  texture_manager);
      break;
    case FloatTexture::kScale:
      result = textures::MakeScale(float_texture.scale(), texture_manager);
      break;
    case FloatTexture::kWindy:
      result = textures::MakeWindy(float_texture.windy(), texture_manager,
                                   world_to_texture);
      break;
    case FloatTexture::kWrinkled:
      result = textures::MakeWrinkled(float_texture.wrinkled(), texture_manager,
                                      world_to_texture);
      break;
    case FloatTexture::FLOAT_TEXTURE_TYPE_NOT_SET:
      return;
  }

  texture_manager.Put(float_texture.name(), std::move(result));
}

void ParseSpectrumTexture(const SpectrumTexture& spectrum_texture,
                          ImageManager& image_manager,
                          TextureManager& texture_manager,
                          const Matrix& world_to_texture) {
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
      result = textures::MakeFbm(spectrum_texture.fbm(), texture_manager,
                                 world_to_texture);
      break;
    case SpectrumTexture::kImagemap:
      result = textures::MakeImageMap(spectrum_texture.imagemap(),
                                      image_manager, texture_manager);
      break;
    case SpectrumTexture::kMarble:
      result = textures::MakeMarble(spectrum_texture.marble(), texture_manager,
                                    world_to_texture);
      break;
    case SpectrumTexture::kMix:
      result = textures::MakeMix(spectrum_texture.mix(), texture_manager);
      break;
    case SpectrumTexture::kPtex:
      result = textures::MakePtex(spectrum_texture.ptex(), image_manager,
                                  texture_manager);
      break;
    case SpectrumTexture::kScale:
      result = textures::MakeScale(spectrum_texture.scale(), texture_manager);
      break;
    case SpectrumTexture::kUv:
      std::cerr << "ERROR: Unsupported SpectrumTexture type: uv" << std::endl;
      exit(EXIT_FAILURE);
      break;
    case SpectrumTexture::kWindy:
      result = textures::MakeWindy(spectrum_texture.windy(), texture_manager,
                                   world_to_texture);
      break;
    case SpectrumTexture::kWrinkled:
      result = textures::MakeWrinkled(spectrum_texture.wrinkled(),
                                      texture_manager, world_to_texture);
      break;
    case SpectrumTexture::SPECTRUM_TEXTURE_TYPE_NOT_SET:
      return;
  }

  texture_manager.Put(spectrum_texture.name(), std::move(result));
}

}  // namespace pbrt_frontend
}  // namespace iris
