#ifndef _IRIS_ENVIRONMENTAL_LIGHTS_IMAGE_ENVIRONMENTAL_LIGHT_
#define _IRIS_ENVIRONMENTAL_LIGHTS_IMAGE_ENVIRONMENTAL_LIGHT_

#include <span>

#include "iris/environmental_light.h"
#include "iris/environmental_lights/internal/distribution_2d.h"
#include "iris/float.h"
#include "iris/matrix.h"
#include "iris/reference_counted.h"
#include "iris/sampler.h"
#include "iris/spectral_allocator.h"
#include "iris/spectrum.h"
#include "iris/vector.h"

namespace iris {
namespace environmental_lights {

class ImageEnvironmentalLight final : public EnvironmentalLight {
 public:
  ImageEnvironmentalLight(std::span<const ReferenceCounted<Spectrum>> spectra,
                          std::span<const visual> luma,
                          std::pair<size_t, size_t> size,
                          ReferenceCounted<Spectrum> scalar,
                          const Matrix& model_to_world);

  std::optional<SampleResult> Sample(
      Sampler& sampler, SpectralAllocator& allocator) const override;

  const Spectrum* Emission(const Vector& to_light, SpectralAllocator& allocator,
                           visual_t* pdf) const override;

 private:
  std::vector<ReferenceCounted<Spectrum>> spectra_;
  internal::Distribution2D distribution_;
  std::pair<size_t, size_t> size_;
  ReferenceCounted<Spectrum> scalar_;
  Matrix model_to_world_;
};

}  // namespace environmental_lights
}  // namespace iris

#endif  // _IRIS_ENVIRONMENTAL_LIGHTS_IMAGE_ENVIRONMENTAL_LIGHT_