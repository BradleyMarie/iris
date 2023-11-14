#ifndef _IRIS_RANDOM_MOCK_RANDOM_
#define _IRIS_RANDOM_MOCK_RANDOM_

#include <memory>

#include "googlemock/include/gmock/gmock.h"
#include "iris/float.h"
#include "iris/random.h"

namespace iris {
namespace random {

class MockBasicRandom : public Random {
 public:
  MOCK_METHOD(size_t, NextIndex, (size_t), (override));
  MOCK_METHOD(geometric, NextGeometric, (), (override));
  MOCK_METHOD(visual, NextVisual, (), (override));
  MOCK_METHOD(std::unique_ptr<Random>, Replicate, (), (override));
};

class MockRandom final : public MockBasicRandom {
 public:
  MOCK_METHOD(void, DiscardIndex, (size_t), (override));
  MOCK_METHOD(void, DiscardGeometric, (size_t), (override));
  MOCK_METHOD(void, DiscardVisual, (size_t), (override));
};

}  // namespace random
}  // namespace iris

#endif  // _IRIS_RANDOM_MOCK_RANDOM_