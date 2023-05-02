#ifndef _IRIS_TEXTURES_UV_TEXTURE_
#define _IRIS_TEXTURES_UV_TEXTURE_

#include <cassert>
#include <optional>

#include "iris/reference_counted.h"
#include "iris/texture_coordinates.h"
#include "iris/textures/texture2d.h"

namespace iris {
namespace textures {

template <typename T, typename... Args>
class UVPointerTexture2D : public PointerTexture2D<T, Args...> {
 public:
  UVPointerTexture2D(std::optional<geometric> u_scale,
                     std::optional<geometric> v_scale,
                     std::optional<geometric> u_offset,
                     std::optional<geometric> v_offset)
      : u_scale_(u_scale.value_or(1.0)),
        v_scale_(v_scale.value_or(1.0)),
        u_offset_(u_offset.value_or(0.0)),
        v_offset_(v_offset.value_or(0.0)) {
    assert(std::isfinite(u_scale_) && u_scale_ != 0.0);
    assert(std::isfinite(v_scale_) && v_scale_ != 0.0);
    assert(std::isfinite(u_offset_));
    assert(std::isfinite(v_offset_));
  }

  const T* Evaluate(const TextureCoordinates& coordinates,
                    Args&... args) const final override {
    return NestedEvaluate(
        coordinates.Scale(u_scale_, v_scale_, u_offset_, v_offset_), args...);
  }

 protected:
  virtual const T* NestedEvaluate(const TextureCoordinates& coordinates,
                                  Args&... args) const = 0;

 private:
  geometric u_scale_;
  geometric v_scale_;
  geometric u_offset_;
  geometric v_offset_;
};

template <typename T>
class UVValueTexture2D : public ValueTexture2D<T> {
 public:
  UVValueTexture2D(std::optional<geometric> u_scale,
                   std::optional<geometric> v_scale,
                   std::optional<geometric> u_offset,
                   std::optional<geometric> v_offset)
      : u_scale_(u_scale.value_or(1.0)),
        v_scale_(v_scale.value_or(1.0)),
        u_offset_(u_offset.value_or(0.0)),
        v_offset_(v_offset.value_or(0.0)) {
    assert(std::isfinite(u_scale_) && u_scale_ != 0.0);
    assert(std::isfinite(v_scale_) && v_scale_ != 0.0);
    assert(std::isfinite(u_offset_));
    assert(std::isfinite(v_offset_));
  }

  T Evaluate(const TextureCoordinates& coordinates) const final override {
    return NestedEvaluate(
        coordinates.Scale(u_scale_, v_scale_, u_offset_, v_offset_));
  }

 protected:
  virtual T NestedEvaluate(const TextureCoordinates& coordinates) const = 0;

 private:
  geometric u_scale_;
  geometric v_scale_;
  geometric u_offset_;
  geometric v_offset_;
};

}  // namespace textures
}  // namespace iris

#endif  // _IRIS_TEXTURES_UV_TEXTURE_