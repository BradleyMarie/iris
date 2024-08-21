#include "iris/reflectors/uniform_reflector.h"

#include <algorithm>
#include <cassert>
#include <cmath>

namespace iris {
namespace reflectors {
namespace {

class UniformReflector final : public Reflector {
 public:
  UniformReflector(visual reflectance);

  visual_t Reflectance(visual_t wavelength) const override;

 private:
  visual_t reflectance_;
};

UniformReflector::UniformReflector(visual reflectance)
    : reflectance_(reflectance) {
  assert(std::isfinite(reflectance));
  assert(reflectance > 0.0);
  assert(reflectance <= 1.0);
}

visual_t UniformReflector::Reflectance(visual_t wavelength) const {
  return reflectance_;
}

}  // namespace

ReferenceCounted<Reflector> CreateUniformReflector(visual reflectance) {
  if (reflectance <= static_cast<visual>(0.0)) {
    return ReferenceCounted<Reflector>();
  }

  return MakeReferenceCounted<UniformReflector>(
      std::min(static_cast<visual>(1.0), reflectance));
}

}  // namespace reflectors
}  // namespace iris