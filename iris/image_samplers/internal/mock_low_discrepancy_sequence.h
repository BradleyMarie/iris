#ifndef _IRIS_IMAGE_SAMPLERS_INTERNAL_MOCK_LOW_DISCREPANCY_SEQUENCE_
#define _IRIS_IMAGE_SAMPLERS_INTERNAL_MOCK_LOW_DISCREPANCY_SEQUENCE_

#include "googlemock/include/gmock/gmock.h"
#include "iris/image_samplers/internal/low_discrepancy_sequence.h"

namespace iris {
namespace image_samplers {
namespace internal {

class MockLowDiscrepancySequence final : public LowDiscrepancySequence {
 public:
  MOCK_METHOD(bool, Start,
              ((std::pair<size_t, size_t>), (std::pair<size_t, size_t>),
               unsigned),
              (override));
  MOCK_METHOD(std::optional<geometric_t>, Next, (), (override));
  MOCK_METHOD(void, Discard, (size_t), (override));
  MOCK_METHOD(visual_t, SampleWeight, (uint32_t), (const override));
  MOCK_METHOD(std::unique_ptr<LowDiscrepancySequence>, Duplicate, (),
              (override));
};

}  // namespace internal
}  // namespace image_samplers
}  // namespace iris

#endif  // _IRIS_IMAGE_SAMPLERS_INTERNAL_MOCK_LOW_DISCREPANCY_SEQUENCE_