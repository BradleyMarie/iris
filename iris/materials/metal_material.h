#ifndef _IRIS_MATERIALS_METAL_MATERIAL_
#define _IRIS_MATERIALS_METAL_MATERIAL_

#include "iris/float.h"
#include "iris/material.h"
#include "iris/reference_counted.h"
#include "iris/reflector.h"
#include "iris/textures/texture2d.h"

namespace iris {
namespace materials {

ReferenceCounted<Material> MakeMetalMaterial(
    ReferenceCounted<textures::ValueTexture2D<visual>> eta_dielectric,
    ReferenceCounted<Spectrum> eta_conductor,
    ReferenceCounted<Spectrum> k_conductor,
    ReferenceCounted<textures::ValueTexture2D<visual>> roughness_u,
    ReferenceCounted<textures::ValueTexture2D<visual>> roughness_v,
    bool remap_roughness);

}  // namespace materials
}  // namespace iris

#endif  // _IRIS_MATERIALS_METAL_MATERIAL_
