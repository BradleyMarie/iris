#ifndef _IRIS_EMISSIVE_MATERIALS_CONSTANT_EMISSIVE_MATERIAL_
#define _IRIS_EMISSIVE_MATERIALS_CONSTANT_EMISSIVE_MATERIAL_

#include "iris/emissive_material.h"
#include "iris/reference_counted.h"
#include "iris/spectrum.h"

namespace iris {
namespace emissive_materials {

ReferenceCounted<EmissiveMaterial> MakeConstantEmissiveMaterial(
    ReferenceCounted<Spectrum> spectrum);

}  // namespace emissive_materials
}  // namespace iris

#endif  // _IRIS_EMISSIVE_MATERIALS_CONSTANT_EMISSIVE_MATERIAL_