#ifndef _IRIS_ALBEDO_MATCHERS_MOCK_ALBEDO_MATCHER_
#define _IRIS_ALBEDO_MATCHERS_MOCK_ALBEDO_MATCHER_

#include "googlemock/include/gmock/gmock.h"
#include "iris/albedo_matcher.h"
#include "iris/float.h"
#include "iris/reflector.h"

namespace iris {
namespace albedo_matchers {

class MockAlbedoMatcher final : public AlbedoMatcher {
 public:
  MOCK_METHOD((visual_t), Match, (const Reflector&), (const override));
};

}  // namespace albedo_matchers
}  // namespace iris

#endif  // _IRIS_ALBEDO_MATCHERS_MOCK_ALBEDO_MATCHER_