#ifndef _IRIS_ENVIRONMENTAL_LIGHTS_IMAGE_ENVIRONMENTAL_LIGHT_
#define _IRIS_ENVIRONMENTAL_LIGHTS_IMAGE_ENVIRONMENTAL_LIGHT_

#include <span>
#include <vector>

#include "iris/environmental_light.h"
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
  ImageEnvironmentalLight(
      std::span<const std::pair<ReferenceCounted<Spectrum>, visual>>
          spectra_and_luminance,
      std::pair<size_t, size_t> size, ReferenceCounted<Spectrum> scalar,
      const Matrix& model_to_world);

  std::optional<SampleResult> Sample(
      Sampler& sampler, SpectralAllocator& allocator) const override;

  const Spectrum* Emission(const Vector& to_light, SpectralAllocator& allocator,
                           visual_t* pdf) const override;

 private:
  Matrix model_to_world_;
  ReferenceCounted<Spectrum> scalar_;
  std::pair<size_t, size_t> size_;
  std::vector<ReferenceCounted<Spectrum>> spectra_;
  std::vector<visual> pdf_;
  std::vector<visual> cdf_;
  std::vector<size_t> spectra_indices_;
};

}  // namespace environmental_lights
}  // namespace iris

#endif  // _IRIS_ENVIRONMENTAL_LIGHTS_IMAGE_ENVIRONMENTAL_LIGHT_