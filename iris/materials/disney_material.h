#ifndef _IRIS_MATERIALS_DISNEY_MATERIAL_
#define _IRIS_MATERIALS_DISNEY_MATERIAL_

#include "iris/float.h"
#include "iris/material.h"
#include "iris/reference_counted.h"
#include "iris/reflector.h"
#include "iris/textures/float_texture.h"
#include "iris/textures/reflector_texture.h"
#include "iris/textures/spectrum_texture.h"

namespace iris {
namespace materials {

ReferenceCounted<Material> MakeDisneyMaterial(
    ReferenceCounted<textures::ReflectorTexture> color,
    ReferenceCounted<textures::FloatTexture> metallic,
    ReferenceCounted<textures::FloatTexture> eta_front,
    ReferenceCounted<textures::FloatTexture> eta_back,
    ReferenceCounted<textures::FloatTexture> roughness,
    ReferenceCounted<textures::FloatTexture> specular_tint,
    ReferenceCounted<textures::FloatTexture> anisotropic,
    ReferenceCounted<textures::FloatTexture> sheen,
    ReferenceCounted<textures::FloatTexture> sheen_tint,
    ReferenceCounted<textures::FloatTexture> clearcoat,
    ReferenceCounted<textures::FloatTexture> clearcoat_gloss,
    ReferenceCounted<textures::FloatTexture> specular_transmission,
    ReferenceCounted<textures::FloatTexture> flatness,
    ReferenceCounted<textures::FloatTexture> diffuse_transmission,
    ReferenceCounted<textures::SpectrumTexture> scatter_distance, bool thin);

}  // namespace materials
}  // namespace iris

#endif  // _IRIS_MATERIALS_DISNEY_MATERIAL_
