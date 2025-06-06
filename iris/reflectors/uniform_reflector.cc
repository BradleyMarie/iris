#include "iris/reflectors/uniform_reflector.h"

#include <algorithm>
#include <cmath>

#include "iris/float.h"
#include "iris/reference_counted.h"
#include "iris/reflector.h"

namespace iris {
namespace reflectors {
namespace {

class UniformReflector final : public Reflector {
 public:
  UniformReflector(visual reflectance) : reflectance_(reflectance) {}

  visual_t Reflectance(visual_t wavelength) const override;

 private:
  visual_t reflectance_;
};

visual_t UniformReflector::Reflectance(visual_t wavelength) const {
  return reflectance_;
}

}  // namespace

ReferenceCounted<Reflector> CreateUniformReflector(visual reflectance) {
  if (reflectance <= static_cast<visual>(0.0) || std::isnan(reflectance)) {
    return ReferenceCounted<Reflector>();
  }

  return MakeReferenceCounted<UniformReflector>(
      std::min(static_cast<visual>(1.0), reflectance));
}

}  // namespace reflectors
}  // namespace iris
