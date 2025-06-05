#include "iris/emissive_materials/constant_emissive_material.h"

#include "iris/emissive_material.h"
#include "iris/power_matcher.h"
#include "iris/reference_counted.h"
#include "iris/spectral_allocator.h"
#include "iris/spectrum.h"
#include "iris/texture_coordinates.h"

namespace iris {
namespace emissive_materials {
namespace {

class ConstantEmissiveMaterial final : public EmissiveMaterial {
 public:
  ConstantEmissiveMaterial(ReferenceCounted<Spectrum> spectrum) noexcept
      : spectrum_(std::move(spectrum)) {}

  const Spectrum* Evaluate(
      const TextureCoordinates& texture_coordinates,
      SpectralAllocator& spectral_allocator) const override;

  visual_t UnitPower(const PowerMatcher& power_matcher) const override;

 private:
  ReferenceCounted<Spectrum> spectrum_;
};

const Spectrum* ConstantEmissiveMaterial::Evaluate(
    const TextureCoordinates& texture_coordinates,
    SpectralAllocator& spectral_allocator) const {
  return spectrum_.Get();
}

visual_t ConstantEmissiveMaterial::UnitPower(
    const PowerMatcher& power_matcher) const {
  return spectrum_ ? power_matcher.Match(*spectrum_)
                   : static_cast<visual_t>(0.0);
}

}  // namespace

ReferenceCounted<EmissiveMaterial> MakeConstantEmissiveMaterial(
    ReferenceCounted<Spectrum> spectrum) {
  if (!spectrum) {
    return ReferenceCounted<EmissiveMaterial>();
  }

  return MakeReferenceCounted<ConstantEmissiveMaterial>(std::move(spectrum));
}

}  // namespace emissive_materials
}  // namespace iris
