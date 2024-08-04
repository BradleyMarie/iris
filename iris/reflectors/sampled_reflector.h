#ifndef _IRIS_REFLECTORS_SAMPLED_REFLECTOR_
#define _IRIS_REFLECTORS_SAMPLED_REFLECTOR_

#include <map>
#include <vector>

#include "iris/float.h"
#include "iris/reflector.h"

namespace iris {
namespace reflectors {

class SampledReflector final : public Reflector {
 public:
  SampledReflector(const std::map<visual, visual>& samples);

  visual_t Reflectance(visual_t wavelength) const override;

 private:
  std::vector<visual> wavelengths_;
  std::vector<visual> intensitites_;
};

}  // namespace reflectors
}  // namespace iris

#endif  // _IRIS_REFLECTORS_SAMPLED_REFLECTOR_