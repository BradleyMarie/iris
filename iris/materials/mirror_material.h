#ifndef _IRIS_MATERIALS_MIRROR_MATERIAL_
#define _IRIS_MATERIALS_MIRROR_MATERIAL_

#include "iris/float.h"
#include "iris/material.h"
#include "iris/reference_counted.h"
#include "iris/reflector.h"
#include "iris/textures/reflector_texture.h"

namespace iris {
namespace materials {

ReferenceCounted<Material> MakeMirrorMaterial(
    ReferenceCounted<textures::ReflectorTexture> reflectance);

}  // namespace materials
}  // namespace iris

#endif  // _IRIS_MATERIALS_MIRROR_MATERIAL_
