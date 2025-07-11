#ifndef _IRIS_IMAGE_SAMPLERS_MOCK_IMAGE_SAMPLER_
#define _IRIS_IMAGE_SAMPLERS_MOCK_IMAGE_SAMPLER_

#include <memory>
#include <optional>
#include <utility>

#include "googlemock/include/gmock/gmock.h"
#include "iris/image_sampler.h"
#include "iris/random_bitstream.h"

namespace iris {
namespace image_samplers {

class MockImageSampler final : public ImageSampler {
 public:
  MOCK_METHOD(void, StartPixel,
              ((std::pair<size_t, size_t>), (std::pair<size_t, size_t>),
               RandomBitstream&),
              (override));
  MOCK_METHOD(std::optional<Sample>, NextSample, (bool, RandomBitstream&),
              (override));
  MOCK_METHOD(std::unique_ptr<ImageSampler>, Replicate, (), (const override));
};

}  // namespace image_samplers
}  // namespace iris

#endif  // _IRIS_IMAGE_SAMPLERS_MOCK_IMAGE_SAMPLER_
