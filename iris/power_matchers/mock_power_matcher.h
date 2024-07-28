#ifndef _IRIS_POWER_MATCHERS_MOCK_POWER_MATCHER_
#define _IRIS_POWER_MATCHERS_MOCK_POWER_MATCHER_

#include "googlemock/include/gmock/gmock.h"
#include "iris/float.h"
#include "iris/power_matcher.h"
#include "iris/spectrum.h"

namespace iris {
namespace power_matchers {

class MockPowerMatcher final : public PowerMatcher {
 public:
  MOCK_METHOD((visual_t), Match, (const Spectrum&), (const override));
};

}  // namespace power_matchers
}  // namespace iris

#endif  // _IRIS_POWER_MATCHERS_MOCK_POWER_MATCHER_