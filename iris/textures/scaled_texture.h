#ifndef _IRIS_TEXTURES_SCALED_TEXTURE_
#define _IRIS_TEXTURES_SCALED_TEXTURE_

#include <cassert>

#include "iris/reference_counted.h"
#include "iris/texture_coordinates.h"
#include "iris/textures/texture2d.h"

namespace iris {
namespace textures {

template <typename T>
class ScaledValueTexture2D final : public ValueTexture2D<T> {
 public:
  ScaledValueTexture2D(ReferenceCounted<ValueTexture2D<T>> texture0,
                       ReferenceCounted<ValueTexture2D<T>> texture1)
      : texture0_(std::move(texture0)), texture1_(std::move(texture1)) {
    assert(texture0_);
    assert(texture1_);
  }

  T Evaluate(const TextureCoordinates& coordinates) const override {
    return texture0_->Evaluate(coordinates) * texture1_->Evaluate(coordinates);
  }

 private:
  ReferenceCounted<ValueTexture2D<T>> texture0_;
  ReferenceCounted<ValueTexture2D<T>> texture1_;
};

}  // namespace textures
}  // namespace iris

#endif  // _IRIS_TEXTURES_SCALED_TEXTURE_