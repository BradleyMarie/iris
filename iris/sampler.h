#ifndef _IRIS_SAMPLER_
#define _IRIS_SAMPLER_

#include <cstddef>
#include <utility>
#include <vector>

#include "iris/float.h"
#include "iris/random.h"
#include "iris/random_bitstream.h"

namespace iris {

class Sampler {
 public:
  Sampler(RandomBitstream& visual_rng, Random* geometric_rng,
          std::vector<visual_t>& visual_storage,
          std::vector<std::pair<geometric_t, geometric_t>>&
              geometric_storage) noexcept;

  Sampler(const Sampler&) = delete;
  Sampler& operator=(const Sampler&) = delete;
  Sampler& operator=(Sampler&& from) = delete;

  Sampler Claim(size_t visual_samples, size_t geometric_samples);
  Sampler Clone() const;

  visual_t NextLinear1D();
  size_t NextIndex1D(size_t size);

  std::pair<geometric_t, geometric_t> NextLinear2D();
  std::pair<geometric_t, geometric_t> NextPolar();

 private:
  Sampler(RandomBitstream& visual_rng, Random* geometric_rng,
          std::vector<visual_t>& visual_storage,
          std::vector<std::pair<geometric_t, geometric_t>>& geometric_storage,
          size_t visual_start, size_t visual_end, size_t geometric_start,
          size_t geometric_end) noexcept;

  RandomBitstream& visual_rng_;
  Random* geometric_rng_;
  std::vector<visual_t>& visual_storage_;
  std::vector<std::pair<geometric_t, geometric_t>>& geometric_storage_;
  size_t visual_indices_[2];
  size_t geometric_indices_[2];
};

}  // namespace iris

#endif  // _IRIS_SAMPLER_
