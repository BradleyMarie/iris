#ifndef _IRIS_MATERIALS_MATTE_MATERIAL_
#define _IRIS_MATERIALS_MATTE_MATERIAL_

#include "iris/float.h"
#include "iris/material.h"
#include "iris/reference_counted.h"
#include "iris/reflector.h"
#include "iris/textures/float_texture.h"
#include "iris/textures/reflector_texture.h"

namespace iris {
namespace materials {

ReferenceCounted<Material> MakeMatteMaterial(
    ReferenceCounted<textures::ReflectorTexture> reflectance,
    ReferenceCounted<textures::FloatTexture> sigma);

}  // namespace materials
}  // namespace iris

#endif  // _IRIS_MATERIALS_MATTE_MATERIAL_
