#ifndef _IRIS_MATERIALS_MATTE_MATERIAL_
#define _IRIS_MATERIALS_MATTE_MATERIAL_

#include "iris/float.h"
#include "iris/material.h"
#include "iris/reference_counted.h"
#include "iris/reflector.h"
#include "iris/spectral_allocator.h"
#include "iris/textures/texture2d.h"

namespace iris {
namespace materials {

ReferenceCounted<Material> MakeMatteMaterial(
    ReferenceCounted<textures::PointerTexture2D<Reflector, SpectralAllocator>>
        reflectance,
    ReferenceCounted<textures::ValueTexture2D<visual>> sigma);

}  // namespace materials
}  // namespace iris

#endif  // _IRIS_MATERIALS_MATTE_MATERIAL_
