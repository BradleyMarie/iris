#ifndef _IRIS_RANDOM_
#define _IRIS_RANDOM_

#include <cstddef>

#include "iris/float.h"

namespace iris {

class Random {
 public:
  virtual size_t NextIndex(size_t size) = 0;
  virtual void DiscardIndex(size_t num_to_discard);

  virtual geometric NextGeometric() = 0;
  virtual void DiscardGeometric(size_t num_to_discard);

  virtual visual NextVisual() = 0;
  virtual void DiscardVisual(size_t num_to_discard);

  virtual ~Random() {}
};

}  // namespace iris

#endif  // _IRIS_RANDOM_
