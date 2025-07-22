#include "iris/textures/mixed_texture.h"

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

class MixedFloatTexture final : public FloatTexture {
 public:
  MixedFloatTexture(ReferenceCounted<FloatTexture> texture0,
                    ReferenceCounted<FloatTexture> texture1,
                    ReferenceCounted<FloatTexture> amount)
      : texture0_(std::move(texture0)),
        texture1_(std::move(texture1)),
        amount_(std::move(amount)) {}

  visual_t Evaluate(const TextureCoordinates& coordinates) const override {
    visual_t v0 = static_cast<visual_t>(0.0);
    if (texture0_) {
      v0 = texture0_->Evaluate(coordinates);
    }

    visual_t v1 = static_cast<visual_t>(0.0);
    if (texture1_) {
      v1 = texture1_->Evaluate(coordinates);
    }

    visual_t amount = static_cast<visual_t>(0.0);
    if (amount_) {
      amount = amount_->Evaluate(coordinates);
    }

    return std::lerp(v0, v1, amount);
  }

 private:
  ReferenceCounted<FloatTexture> texture0_;
  ReferenceCounted<FloatTexture> texture1_;
  ReferenceCounted<FloatTexture> amount_;
};

class MixedReflectorTexture final : public ReflectorTexture {
 public:
  MixedReflectorTexture(ReferenceCounted<ReflectorTexture> texture0,
                        ReferenceCounted<ReflectorTexture> texture1,
                        ReferenceCounted<FloatTexture> amount)
      : texture0_(std::move(texture0)),
        texture1_(std::move(texture1)),
        amount_(std::move(amount)) {}

  const Reflector* Evaluate(const TextureCoordinates& coordinates,
                            SpectralAllocator& allocator) const override {
    const Reflector* r0 = nullptr;
    if (texture0_) {
      r0 = texture0_->Evaluate(coordinates, allocator);
    }

    const Reflector* r1 = nullptr;
    if (texture1_) {
      r1 = texture1_->Evaluate(coordinates, allocator);
    }

    visual_t v1_amount = static_cast<visual_t>(0.0);
    if (amount_) {
      v1_amount = amount_->Evaluate(coordinates);
    }

    visual_t v0_amount = static_cast<visual_t>(1.0) - v1_amount;

    return allocator.UnboundedAdd(allocator.UnboundedScale(r0, v0_amount),
                                  allocator.UnboundedScale(r1, v1_amount));
  }

 private:
  ReferenceCounted<ReflectorTexture> texture0_;
  ReferenceCounted<ReflectorTexture> texture1_;
  ReferenceCounted<FloatTexture> amount_;
};

}  // namespace

ReferenceCounted<FloatTexture> MakeMixedTexture(
    ReferenceCounted<FloatTexture> texture0,
    ReferenceCounted<FloatTexture> texture1,
    ReferenceCounted<FloatTexture> amount) {
  if (!texture0 && !texture1) {
    return ReferenceCounted<FloatTexture>();
  }

  if (!amount) {
    return texture0;
  }

  return MakeReferenceCounted<MixedFloatTexture>(
      std::move(texture0), std::move(texture1), std::move(amount));
}

ReferenceCounted<ReflectorTexture> MakeMixedTexture(
    ReferenceCounted<ReflectorTexture> texture0,
    ReferenceCounted<ReflectorTexture> texture1,
    ReferenceCounted<FloatTexture> amount) {
  if (!texture0 && !texture1) {
    return ReferenceCounted<ReflectorTexture>();
  }

  if (!amount) {
    return texture0;
  }

  return MakeReferenceCounted<MixedReflectorTexture>(
      std::move(texture0), std::move(texture1), std::move(amount));
}

}  // namespace textures
}  // namespace iris
