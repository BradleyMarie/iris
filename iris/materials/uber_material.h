#ifndef _IRIS_MATERIALS_UBER_MATERIAL_
#define _IRIS_MATERIALS_UBER_MATERIAL_

#include "iris/float.h"
#include "iris/material.h"
#include "iris/reference_counted.h"
#include "iris/reflector.h"
#include "iris/spectral_allocator.h"
#include "iris/textures/texture2d.h"

namespace iris {
namespace materials {

ReferenceCounted<Material> MakeUberMaterial(
    ReferenceCounted<textures::PointerTexture2D<Reflector, SpectralAllocator>>
        reflectance,
    ReferenceCounted<textures::PointerTexture2D<Reflector, SpectralAllocator>>
        transmittance,
    ReferenceCounted<textures::PointerTexture2D<Reflector, SpectralAllocator>>
        diffuse,
    ReferenceCounted<textures::PointerTexture2D<Reflector, SpectralAllocator>>
        specular,
    ReferenceCounted<textures::ValueTexture2D<visual>> opacity,
    ReferenceCounted<textures::ValueTexture2D<visual>> eta_incident,
    ReferenceCounted<textures::ValueTexture2D<visual>> eta_transmitted,
    ReferenceCounted<textures::ValueTexture2D<visual>> roughness_u,
    ReferenceCounted<textures::ValueTexture2D<visual>> roughness_v,
    bool remap_roughness);

}  // namespace materials
}  // namespace iris

#endif  // _IRIS_MATERIALS_TRANSLUCENT_MATERIAL_
