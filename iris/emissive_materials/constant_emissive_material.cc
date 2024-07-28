#include "iris/emissive_materials/constant_emissive_material.h"

namespace iris {
namespace emissive_materials {

const Spectrum* ConstantEmissiveMaterial::Evaluate(
    const TextureCoordinates& texture_coordinates,
    SpectralAllocator& spectral_allocator) const {
  return spectrum_.Get();
}

visual_t ConstantEmissiveMaterial::UnitPower(
    const PowerMatcher& power_matcher) const {
  return power_matcher.Match(*spectrum_);
}

}  // namespace emissive_materials
}  // namespace iris