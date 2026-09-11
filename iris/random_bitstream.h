#ifndef _IRIS_RANDOM_BITSTREAM_
#define _IRIS_RANDOM_BITSTREAM_

#include <cstddef>
#include <cstdint>
#include <memory>

#include "iris/float.h"
#include "iris/random.h"

namespace iris {

class RandomBitstream : public Random {
 public:
  virtual ~RandomBitstream() {}

  virtual uint32_t Next() = 0;
  virtual std::unique_ptr<RandomBitstream> Replicate() = 0;

  // RandomBitstream fully implements the Random interface; however, it assumes
  // that all values returned by the derived class are completely random and as
  // such it treats Discard calls as a no-op.
  geometric NextGeometric() final override;
  void DiscardGeometric(size_t num_to_discard) final override;

  visual NextVisual() final override;
  size_t NextIndex(size_t size) final override;
};

}  // namespace iris

#endif  // _IRIS_RANDOM_BITSTREAM_
