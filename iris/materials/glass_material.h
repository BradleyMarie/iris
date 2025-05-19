#ifndef _IRIS_MATERIALS_GLASS_MATERIAL_
#define _IRIS_MATERIALS_GLASS_MATERIAL_

#include "iris/float.h"
#include "iris/material.h"
#include "iris/reference_counted.h"
#include "iris/reflector.h"
#include "iris/spectral_allocator.h"
#include "iris/textures/texture2d.h"

namespace iris {
namespace materials {

ReferenceCounted<Material> MakeGlassMaterial(
    ReferenceCounted<textures::PointerTexture2D<Reflector, SpectralAllocator>>
        reflectance,
    ReferenceCounted<textures::PointerTexture2D<Reflector, SpectralAllocator>>
        transmittance,
    ReferenceCounted<textures::ValueTexture2D<visual>> eta_incident,
    ReferenceCounted<textures::ValueTexture2D<visual>> eta_transmitted);

}  // namespace materials
}  // namespace iris

#endif  // _IRIS_MATERIALS_GLASS_MATERIAL_
