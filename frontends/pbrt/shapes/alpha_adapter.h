#ifndef _FRONTENDS_PBRT_SHAPES_PARSE_
#define _FRONTENDS_PBRT_SHAPES_PARSE_

#include "iris/reference_counted.h"
#include "iris/textures/texture2d.h"

namespace iris {
namespace pbrt_frontend {
namespace shapes {

class AlphaAdapter : public textures::ValueTexture2D<bool> {
 public:
  AlphaAdapter(ReferenceCounted<textures::ValueTexture2D<visual>> alpha)
      : alpha_(std::move(alpha)) {}

  bool Evaluate(const TextureCoordinates& texture_coordinates) const override {
    return alpha_->Evaluate(texture_coordinates) > static_cast<visual>(0.0);
  }

 private:
  ReferenceCounted<textures::ValueTexture2D<visual>> alpha_;
};

}  // namespace shapes
}  // namespace pbrt_frontend
}  // namespace iris

#endif  // _FRONTENDS_PBRT_SHAPES_PARSE_
