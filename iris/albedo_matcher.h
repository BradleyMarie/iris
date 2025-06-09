#ifndef _IRIS_ALBEDO_MATCHER_
#define _IRIS_ALBEDO_MATCHER_

#include "iris/float.h"
#include "iris/reflector.h"

namespace iris {

class AlbedoMatcher {
 public:
  virtual visual_t Match(const Reflector& reflector) const = 0;
  virtual ~AlbedoMatcher() {}
};

}  // namespace iris

#endif  // _IRIS_ALBEDO_MATCHER_
