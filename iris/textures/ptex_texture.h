#ifndef _IRIS_TEXTURES_PTEX_TEXTURE_
#define _IRIS_TEXTURES_PTEX_TEXTURE_

#include <cstdint>
#include <memory>
#include <string>

#include "iris/float.h"
#include "iris/reference_counted.h"
#include "iris/textures/float_texture.h"
#include "iris/textures/reflector_texture.h"
#include "third_party/disney/ptex/Ptexture.h"

namespace iris {
namespace textures {

ReferenceCounted<FloatTexture> MakePtexTexture(
    std::shared_ptr<Ptex::PtexCache> cache, std::string filename,
    visual_t gamma);

ReferenceCounted<ReflectorTexture> MakePtexTexture(
    std::shared_ptr<Ptex::PtexCache> cache, std::string filename,
    visual_t gamma, ReferenceCounted<ReflectorTexture> r,
    ReferenceCounted<ReflectorTexture> g, ReferenceCounted<ReflectorTexture> b);

}  // namespace textures
}  // namespace iris

#endif  // _IRIS_TEXTURES_PTEX_TEXTURE_
