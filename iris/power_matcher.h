#ifndef _IRIS_POWER_MATCHER_
#define _IRIS_POWER_MATCHER_

#include "iris/float.h"
#include "iris/spectrum.h"

namespace iris {

class PowerMatcher {
 public:
  virtual visual_t Match(const Spectrum& spectrum) const = 0;
  virtual ~PowerMatcher() {}
};

}  // namespace iris

#endif  // _IRIS_POWER_MATCHER_