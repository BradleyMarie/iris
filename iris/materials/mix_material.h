#ifndef _IRIS_MATERIALS_MIX_MATERIAL_
#define _IRIS_MATERIALS_MIX_MATERIAL_

#include "iris/float.h"
#include "iris/material.h"
#include "iris/reference_counted.h"
#include "iris/reflector.h"
#include "iris/textures/reflector_texture.h"

namespace iris {
namespace materials {

ReferenceCounted<Material> MakeMixMaterial(
    ReferenceCounted<Material> material0, ReferenceCounted<Material> material1,
    ReferenceCounted<textures::ReflectorTexture> interpolation);

}  // namespace materials
}  // namespace iris

#endif  // _IRIS_MATERIALS_TRANSLUCENT_MATERIAL_
