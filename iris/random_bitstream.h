#ifndef _IRIS_RANDOM_BITSTREAM_
#define _IRIS_RANDOM_BITSTREAM_

#include <cstdint>
#include <memory>

#include "iris/float.h"

namespace iris {

class RandomBitstream {
 public:
  geometric_t NextGeometric();
  visual_t NextVisual();

  virtual ~RandomBitstream() {}
  virtual uint32_t Next() = 0;
  virtual std::unique_ptr<RandomBitstream> Replicate() = 0;
};

}  // namespace iris

#endif  // _IRIS_RANDOM_BITSTREAM_
