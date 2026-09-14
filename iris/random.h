#ifndef _IRIS_RANDOM_
#define _IRIS_RANDOM_

#include "iris/float.h"

namespace iris {

class Random {
 public:
  virtual geometric_t Next() = 0;
  virtual ~Random() {}
};

}  // namespace iris

#endif  // _IRIS_RANDOM_
