#ifndef _IRIS_RANDOM_MOCK_RANDOM_
#define _IRIS_RANDOM_MOCK_RANDOM_

#include <cstddef>

#include "googlemock/include/gmock/gmock.h"
#include "iris/float.h"
#include "iris/random.h"

namespace iris {
namespace random {

class MockRandom : public Random {
 public:
  MOCK_METHOD(geometric, Next, (), (override));
};

}  // namespace random
}  // namespace iris

#endif  // _IRIS_RANDOM_MOCK_RANDOM_
