#include "iris/materials/fourier_material.h"

#include <cstdint>
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
    std::vector<visual> coefficients,
    std::vector<std::pair<uint32_t, uint32_t>> y_coefficients, visual eta) {
  return ReferenceCounted<Material>();
}

ReferenceCounted<Material> MakeFourierMaterial(
    ReferenceCounted<Reflector> r, ReferenceCounted<Reflector> g,
    ReferenceCounted<Reflector> b, std::vector<geometric> elevational_samples,
    std::vector<visual> cdf, std::vector<visual> coefficients,
    std::vector<std::pair<uint32_t, uint32_t>> y_coefficients,
    std::vector<std::pair<uint32_t, uint32_t>> r_coefficients,
    std::vector<std::pair<uint32_t, uint32_t>> b_coefficients, visual eta) {
  return ReferenceCounted<Material>();
}

}  // namespace materials
}  // namespace iris
