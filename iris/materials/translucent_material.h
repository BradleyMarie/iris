#ifndef _IRIS_MATERIALS_TRANSLUCENT_MATERIAL_
#define _IRIS_MATERIALS_TRANSLUCENT_MATERIAL_

#include "iris/float.h"
#include "iris/material.h"
#include "iris/reference_counted.h"
#include "iris/reflector.h"
#include "iris/textures/float_texture.h"
#include "iris/textures/reflector_texture.h"

namespace iris {
namespace materials {

ReferenceCounted<Material> MakeTranslucentMaterial(
    ReferenceCounted<textures::ReflectorTexture> reflectance,
    ReferenceCounted<textures::ReflectorTexture> transmittance,
    ReferenceCounted<textures::ReflectorTexture> diffuse,
    ReferenceCounted<textures::ReflectorTexture> specular,
    ReferenceCounted<textures::FloatTexture> eta_incident,
    ReferenceCounted<textures::FloatTexture> eta_transmitted,
    ReferenceCounted<textures::FloatTexture> roughness, bool remap_roughness);

}  // namespace materials
}  // namespace iris

#endif  // _IRIS_MATERIALS_TRANSLUCENT_MATERIAL_
