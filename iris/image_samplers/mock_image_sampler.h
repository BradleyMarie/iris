#ifndef _IRIS_IMAGE_SAMPLERS_MOCK_IMAGE_SAMPLER_
#define _IRIS_IMAGE_SAMPLERS_MOCK_IMAGE_SAMPLER_

#include <array>
#include <memory>
#include <optional>
#include <utility>

#include "googlemock/include/gmock/gmock.h"
#include "iris/float.h"
#include "iris/image_sampler.h"
#include "iris/random.h"

namespace iris {
namespace image_samplers {

class MockImageSampler final : public ImageSampler {
 public:
  MOCK_METHOD(void, StartPixel,
              ((std::pair<size_t, size_t>), (std::pair<size_t, size_t>)),
              (override));
  MOCK_METHOD(std::optional<Sample>, NextSample, (bool, Random&), (override));
  MOCK_METHOD(std::unique_ptr<ImageSampler>, Replicate, (), (const override));
};

}  // namespace image_samplers
}  // namespace iris

#endif  // _IRIS_IMAGE_SAMPLERS_MOCK_IMAGE_SAMPLER_