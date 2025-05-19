#ifndef _IRIS_MATERIALS_PLASTIC_MATERIAL_
#define _IRIS_MATERIALS_PLASTIC_MATERIAL_

#include "iris/float.h"
#include "iris/material.h"
#include "iris/reference_counted.h"
#include "iris/reflector.h"
#include "iris/spectral_allocator.h"
#include "iris/textures/texture2d.h"

namespace iris {
namespace materials {

ReferenceCounted<Material> MakePlasticMaterial(
    ReferenceCounted<textures::PointerTexture2D<Reflector, SpectralAllocator>>
        diffuse,
    ReferenceCounted<textures::PointerTexture2D<Reflector, SpectralAllocator>>
        specular,
    ReferenceCounted<textures::ValueTexture2D<visual>> eta_incident,
    ReferenceCounted<textures::ValueTexture2D<visual>> eta_transmitted,
    ReferenceCounted<textures::ValueTexture2D<visual>> roughness,
    bool remap_roughness);

}  // namespace materials
}  // namespace iris

#endif  // _IRIS_MATERIALS_PLASTIC_MATERIAL_
