#ifndef _IRIS_TEXTURES_SCALED_TEXTURE_
#define _IRIS_TEXTURES_SCALED_TEXTURE_

#include <cassert>

#include "iris/reference_counted.h"
#include "iris/texture_coordinates.h"
#include "iris/textures/texture2d.h"

namespace iris {
namespace textures {
namespace internal {

static inline TextureCoordinates Scale(const TextureCoordinates& coordinates,
                                       geometric u_scale, geometric v_scale) {
  assert(std::isfinite(u_scale) && u_scale != 0.0);
  assert(std::isfinite(v_scale) && v_scale != 0.0);

  std::array<geometric_t, 2> uv;
  uv[0] = coordinates.uv[0] * u_scale;
  uv[1] = coordinates.uv[1] * v_scale;

  if (!coordinates.derivatives) {
    return {uv};
  }

  TextureCoordinates::Derivatives derivatives{
      (*coordinates.derivatives).du_dx * u_scale,
      (*coordinates.derivatives).du_dy * u_scale,
      (*coordinates.derivatives).dv_dx * v_scale,
      (*coordinates.derivatives).dv_dy * v_scale,
      (*coordinates.derivatives).dp_du / u_scale,
      (*coordinates.derivatives).dp_dv / v_scale};

  return {uv, derivatives};
}

}  // namespace internal

template <typename T, typename... Args>
class ScaledPointerTexture2D final : public PointerTexture2D<T, Args...> {
 public:
  ScaledPointerTexture2D(ReferenceCounted<PointerTexture2D<T, Args...>> texture,
                         geometric u_scale, geometric v_scale)
      : texture_(std::move(texture)), u_scale_(u_scale), v_scale_(v_scale) {
    assert(texture_);
    assert(std::isfinite(u_scale) && u_scale != 0.0);
    assert(std::isfinite(v_scale) && v_scale != 0.0);
  }

  const T* Evaluate(const TextureCoordinates& coordinates,
                    Args&... args) const override {
    return texture_->Evaluate(internal::Scale(coordinates, u_scale_, v_scale_),
                              args...);
  }

 private:
  ReferenceCounted<PointerTexture2D<T, Args...>> texture_;
  geometric u_scale_;
  geometric v_scale_;
};

template <typename T>
class ScaledValueTexture2D final : public ValueTexture2D<T> {
 public:
  ScaledValueTexture2D(ReferenceCounted<ValueTexture2D<T>> texture,
                       geometric u_scale, geometric v_scale)
      : texture_(std::move(texture)), u_scale_(u_scale), v_scale_(v_scale) {
    assert(texture_);
    assert(std::isfinite(u_scale) && u_scale != 0.0);
    assert(std::isfinite(v_scale) && v_scale != 0.0);
  }

  T Evaluate(const TextureCoordinates& coordinates) const override {
    return texture_->Evaluate(internal::Scale(coordinates, u_scale_, v_scale_));
  }

 private:
  ReferenceCounted<ValueTexture2D<T>> texture_;
  geometric u_scale_;
  geometric v_scale_;
};

}  // namespace textures
}  // namespace iris

#endif  // _IRIS_TEXTURES_SCALED_TEXTURE_