#include "iris/textures/scaled_texture.h"

#include <cassert>
#include <utility>

#include "iris/float.h"
#include "iris/reference_counted.h"
#include "iris/reflector.h"
#include "iris/spectral_allocator.h"
#include "iris/texture_coordinates.h"
#include "iris/textures/float_texture.h"
#include "iris/textures/reflector_texture.h"

namespace iris {
namespace textures {
namespace {

class ScaledFloatTexture final : public FloatTexture {
 public:
  ScaledFloatTexture(ReferenceCounted<FloatTexture> texture0,
                     ReferenceCounted<FloatTexture> texture1)
      : texture0_(std::move(texture0)), texture1_(std::move(texture1)) {
    assert(texture0_);
    assert(texture1_);
  }

  visual_t Evaluate(const TextureCoordinates& coordinates) const override {
    return texture0_->Evaluate(coordinates) * texture1_->Evaluate(coordinates);
  }

 private:
  ReferenceCounted<FloatTexture> texture0_;
  ReferenceCounted<FloatTexture> texture1_;
};

class ScaledReflectorTexture final : public ReflectorTexture {
 public:
  ScaledReflectorTexture(ReferenceCounted<ReflectorTexture> texture0,
                         ReferenceCounted<ReflectorTexture> texture1)
      : texture0_(std::move(texture0)), texture1_(std::move(texture1)) {
    assert(texture0_);
    assert(texture1_);
  }

  const Reflector* Evaluate(const TextureCoordinates& coordinates,
                            SpectralAllocator& allocator) const override {
    const Reflector* reflector0 = texture0_->Evaluate(coordinates, allocator);
    const Reflector* reflector1 = texture1_->Evaluate(coordinates, allocator);
    return allocator.Scale(reflector0, reflector1);
  }

 private:
  ReferenceCounted<ReflectorTexture> texture0_;
  ReferenceCounted<ReflectorTexture> texture1_;
};

}  // namespace

ReferenceCounted<FloatTexture> MakeScaledTexture(
    ReferenceCounted<FloatTexture> texture0,
    ReferenceCounted<FloatTexture> texture1) {
  if (!texture0 || !texture1) {
    return ReferenceCounted<FloatTexture>();
  }

  return MakeReferenceCounted<ScaledFloatTexture>(std::move(texture0),
                                                  std::move(texture1));
}

ReferenceCounted<ReflectorTexture> MakeScaledTexture(
    ReferenceCounted<ReflectorTexture> texture0,
    ReferenceCounted<ReflectorTexture> texture1) {
  if (!texture0 || !texture1) {
    return ReferenceCounted<ReflectorTexture>();
  }

  return MakeReferenceCounted<ScaledReflectorTexture>(std::move(texture0),
                                                      std::move(texture1));
}

}  // namespace textures
}  // namespace iris
