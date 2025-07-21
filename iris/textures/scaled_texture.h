#ifndef _IRIS_TEXTURES_SCALED_TEXTURE_
#define _IRIS_TEXTURES_SCALED_TEXTURE_

#include <cassert>
#include <utility>

#include "iris/reference_counted.h"
#include "iris/spectral_allocator.h"
#include "iris/texture_coordinates.h"
#include "iris/textures/float_texture.h"
#include "iris/textures/reflector_texture.h"
#include "iris/textures/texture2d.h"

namespace iris {
namespace textures {

template <typename Return>
class ScaledSpectralTexture2D final
    : public PointerTexture2D<Return, SpectralAllocator> {
 public:
  ScaledSpectralTexture2D(
      ReferenceCounted<PointerTexture2D<Return, SpectralAllocator>> texture0,
      ReferenceCounted<PointerTexture2D<Return, SpectralAllocator>> texture1)
      : texture0_(std::move(texture0)), texture1_(std::move(texture1)) {
    assert(texture0_);
    assert(texture1_);
  }

  const Return* Evaluate(const TextureCoordinates& coordinates,
                         SpectralAllocator& allocator) const override {
    const Return* v0 = texture0_->Evaluate(coordinates, allocator);
    const Return* v1 = texture1_->Evaluate(coordinates, allocator);
    return allocator.Scale(v0, v1);
  }

 private:
  const ReferenceCounted<PointerTexture2D<Return, SpectralAllocator>> texture0_;
  const ReferenceCounted<PointerTexture2D<Return, SpectralAllocator>> texture1_;
};

template <typename Return>
ReferenceCounted<ScaledSpectralTexture2D<Return>> MakeScaledSpectralTexture2D(
    ReferenceCounted<PointerTexture2D<Return, SpectralAllocator>> texture0,
    ReferenceCounted<PointerTexture2D<Return, SpectralAllocator>> texture1) {
  if (!texture0) {
    return ReferenceCounted<ScaledSpectralTexture2D<Return>>();
  }

  if (!texture1) {
    return ReferenceCounted<ScaledSpectralTexture2D<Return>>();
  }

  return MakeReferenceCounted<ScaledSpectralTexture2D<Return>>(
      std::move(texture0), std::move(texture1));
}

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

template <typename T>
ReferenceCounted<ScaledValueTexture2D<T>> MakeScaledValueTexture2D(
    ReferenceCounted<ValueTexture2D<T>> texture0,
    ReferenceCounted<ValueTexture2D<T>> texture1) {
  if (!texture0) {
    return ReferenceCounted<ScaledValueTexture2D<T>>();
  }

  if (!texture1) {
    return ReferenceCounted<ScaledValueTexture2D<T>>();
  }

  return MakeReferenceCounted<ScaledValueTexture2D<T>>(std::move(texture0),
                                                       std::move(texture1));
}

ReferenceCounted<FloatTexture> MakeScaledTexture(
    ReferenceCounted<FloatTexture> texture0,
    ReferenceCounted<FloatTexture> texture1);

ReferenceCounted<ReflectorTexture> MakeScaledTexture(
    ReferenceCounted<ReflectorTexture> texture0,
    ReferenceCounted<ReflectorTexture> texture1);

}  // namespace textures
}  // namespace iris

#endif  // _IRIS_TEXTURES_SCALED_TEXTURE_
