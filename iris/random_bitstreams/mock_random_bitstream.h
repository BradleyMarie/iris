#ifndef _IRIS_RANDOM_MOCK_RANDOM_BITSTREAM_
#define _IRIS_RANDOM_MOCK_RANDOM_BITSTREAM_

#include <cstdint>
#include <memory>

#include "googlemock/include/gmock/gmock.h"
#include "iris/random_bitstream.h"

namespace iris {
namespace random_bitstreams {

class MockRandomBitstream : public RandomBitstream {
 public:
  MOCK_METHOD(uint32_t, Next, (), (override));
  MOCK_METHOD(std::unique_ptr<RandomBitstream>, Replicate, (), (override));
};

}  // namespace random_bitstreams
}  // namespace iris

#endif  // _IRIS_RANDOM_MOCK_RANDOM_BITSTREAM_
