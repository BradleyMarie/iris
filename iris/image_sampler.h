#ifndef _IRIS_IMAGE_SAMPLER_
#define _IRIS_IMAGE_SAMPLER_

#include <array>
#include <memory>
#include <optional>
#include <utility>

#include "iris/float.h"
#include "iris/random.h"

namespace iris {

class ImageSampler {
 public:
  virtual void StartPixel(std::pair<size_t, size_t> image_dimensions,
                          std::pair<size_t, size_t> pixel) = 0;

  struct Sample {
    const std::array<geometric_t, 2> image_uv;
    const std::array<geometric_t, 2> image_uv_dxdy;
    const std::optional<std::array<geometric_t, 2>> lens_uv;
    const visual_t weight;
    Random& rng;
  };

  virtual std::optional<Sample> NextSample(bool sample_lens, Random& rng) = 0;

  virtual std::unique_ptr<ImageSampler> Replicate() const = 0;

  virtual ~ImageSampler() {}
};

}  // namespace iris

#endif  // _IRIS_IMAGE_SAMPLER_
