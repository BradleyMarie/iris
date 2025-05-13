#include "frontends/pbrt/textures/parse.h"

#include "frontends/pbrt/image_manager.h"
#include "frontends/pbrt/spectrum_manager.h"
#include "frontends/pbrt/texture_manager.h"
#include "frontends/pbrt/textures/constant.h"
#include "frontends/pbrt/textures/imagemap.h"
#include "frontends/pbrt/textures/scale.h"
#include "iris/reference_counted.h"
#include "iris/reflector.h"
#include "iris/spectral_allocator.h"
#include "iris/textures/texture2d.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {

using ::iris::textures::PointerTexture2D;
using ::iris::textures::ValueTexture2D;
using ::pbrt_proto::v3::FloatTexture;
using ::pbrt_proto::v3::SpectrumTexture;

void ParseFloatTexture(const FloatTexture& float_texture,
                       ImageManager& image_manager,
                       TextureManager& texture_manager) {
  ReferenceCounted<ValueTexture2D<visual>> result;
  switch (float_texture.float_texture_type_case()) {
    case FloatTexture::kBilerp:
      break;
    case FloatTexture::kCheckerboard2D:
      break;
    case FloatTexture::kCheckerboard3D:
      break;
    case FloatTexture::kConstant:
      result =
          textures::MakeConstant(float_texture.constant(), texture_manager);
      break;
    case FloatTexture::kDots:
      break;
    case FloatTexture::kFbm:
      break;
    case FloatTexture::kImagemap:
      result = textures::MakeImageMap(float_texture.imagemap(), image_manager,
                                      texture_manager);
      break;
    case FloatTexture::kMarble:
      break;
    case FloatTexture::kMix:
      break;
    case FloatTexture::kPtex:
      break;
    case FloatTexture::kScale:
      result = textures::MakeScale(float_texture.scale(), texture_manager);
      break;
    case FloatTexture::kWindy:
      break;
    case FloatTexture::kWrinkled:
      break;
    case FloatTexture::FLOAT_TEXTURE_TYPE_NOT_SET:
      break;
  }

  if (result) {
    texture_manager.Put(float_texture.name(), std::move(result));
  }
}

void ParseSpectrumTexture(const SpectrumTexture& spectrum_texture,
                          ImageManager& image_manager,
                          TextureManager& texture_manager) {
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> result;
  switch (spectrum_texture.spectrum_texture_type_case()) {
    case SpectrumTexture::kBilerp:
      break;
    case SpectrumTexture::kCheckerboard2D:
      break;
    case SpectrumTexture::kCheckerboard3D:
      break;
    case SpectrumTexture::kConstant:
      result =
          textures::MakeConstant(spectrum_texture.constant(), texture_manager);
      break;
    case SpectrumTexture::kDots:
      break;
    case SpectrumTexture::kFbm:
      break;
    case SpectrumTexture::kImagemap:
      result = textures::MakeImageMap(spectrum_texture.imagemap(),
                                      image_manager, texture_manager);
      break;
    case SpectrumTexture::kMarble:
      break;
    case SpectrumTexture::kMix:
      break;
    case SpectrumTexture::kPtex:
      break;
    case SpectrumTexture::kScale:
      result = textures::MakeScale(spectrum_texture.scale(), texture_manager);
      break;
    case SpectrumTexture::kUv:
      break;
    case SpectrumTexture::kWindy:
      break;
    case SpectrumTexture::kWrinkled:
      break;
    case SpectrumTexture::SPECTRUM_TEXTURE_TYPE_NOT_SET:
      break;
  }

  if (result) {
    texture_manager.Put(spectrum_texture.name(), std::move(result));
  }
}

}  // namespace pbrt_frontend
}  // namespace iris