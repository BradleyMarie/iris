#ifndef _IRIS_HIT_
#define _IRIS_HIT_

#include "iris/float.h"

namespace iris {
namespace internal {
class Hit;
};

class Hit {
 public:
  Hit* next;
  const geometric_t distance;
  const geometric_t error;

 private:
  Hit(Hit* next, geometric_t distance, geometric_t error)
      : next(next), distance(distance), error(error) {}

  friend class internal::Hit;
};

}  // namespace iris

#endif  // _IRIS_HIT_
