#ifndef _IRIS_MATERIALS_FOURIER_MATERIAL_
#define _IRIS_MATERIALS_FOURIER_MATERIAL_

#include <cstddef>
#include <utility>
#include <vector>

#include "iris/float.h"
#include "iris/material.h"
#include "iris/reference_counted.h"
#include "iris/reflector.h"

namespace iris {
namespace materials {

ReferenceCounted<Material> MakeFourierMaterial(
    std::vector<geometric> elevational_samples, std::vector<visual> cdf,
    std::vector<std::pair<size_t, size_t>> coefficient_extents,
    std::vector<visual> y_coefficients, visual eta, bool front_face);

ReferenceCounted<Material> MakeFourierMaterial(
    ReferenceCounted<Reflector> r, ReferenceCounted<Reflector> g,
    ReferenceCounted<Reflector> b, std::vector<geometric> elevational_samples,
    std::vector<visual> cdf,
    std::vector<std::pair<size_t, size_t>> coefficient_extents,
    std::vector<visual> y_coefficients, std::vector<visual> r_coefficients,
    std::vector<visual> b_coefficients, visual eta, bool front_face);

}  // namespace materials
}  // namespace iris

#endif  // _IRIS_MATERIALS_FOURIER_MATERIAL_
