#ifndef _IRIS_COLOR_MATCHERS_CIE_COLOR_MATCHER_
#define _IRIS_COLOR_MATCHERS_CIE_COLOR_MATCHER_

#include <memory>

#include "iris/color_matcher.h"
#include "iris/float.h"

namespace iris {
namespace color_matchers {

std::unique_ptr<ColorMatcher> MakeCieColorMatcher();

extern const visual kCieYIntegral;

}  // namespace color_matchers
}  // namespace iris

#endif  // _IRIS_COLOR_MATCHERS_CIE_COLOR_MATCHER_
