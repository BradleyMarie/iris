#include "frontends/pbrt/shapes/alpha_mask.h"

#include <cassert>
#include <utility>

#include "frontends/pbrt/texture_manager.h"
#include "iris/float.h"
#include "iris/reference_counted.h"
#include "iris/textures/texture2d.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace {

using ::iris::textures::ValueTexture2D;
using ::pbrt_proto::v3::FloatTextureParameter;
using ::pbrt_proto::v3::FloatTextureParameter;

class AlphaMask : public ValueTexture2D<bool> {
 public:
  AlphaMask(ReferenceCounted<ValueTexture2D<visual>> alpha)
      : alpha_(std::move(alpha)) {
    assert(alpha_);
  }

  bool Evaluate(const TextureCoordinates& texture_coordinates) const override {
    return alpha_->Evaluate(texture_coordinates) != static_cast<visual>(0.0);
  }

 private:
  ReferenceCounted<ValueTexture2D<visual>> alpha_;
};

}  // namespace

bool MakeAlphaMask(TextureManager& texture_manager,
                   const FloatTextureParameter& alpha,
                   ReferenceCounted<ValueTexture2D<bool>>& result) {
  if (alpha.float_value() != 0.0) {
    result = ReferenceCounted<ValueTexture2D<bool>>();
    return true;
  }

  TextureManager::FloatTexturePtr texture =
      texture_manager.AllocateFloatTexture(alpha);
  if (!texture) {
    return false;
  }

  result = MakeReferenceCounted<AlphaMask>(std::move(texture));

  return true;
}

}  // namespace pbrt_frontend
}  // namespace iris
