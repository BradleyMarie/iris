#ifndef _IRIS_TEXTURES_INTERNAL_UV_TEXTURE_
#define _IRIS_TEXTURES_INTERNAL_UV_TEXTURE_

#include <cassert>

#include "iris/float.h"
#include "iris/reflector.h"
#include "iris/spectral_allocator.h"
#include "iris/texture_coordinates.h"
#include "iris/textures/float_texture.h"
#include "iris/textures/reflector_texture.h"

namespace iris {
namespace textures {
namespace internal {

class UVFloatTexture : public FloatTexture {
 public:
  UVFloatTexture(geometric u_scale, geometric v_scale, geometric u_offset,
                 geometric v_offset)
      : u_scale_(u_scale),
        v_scale_(v_scale),
        u_offset_(u_offset),
        v_offset_(v_offset) {
    assert(std::isfinite(u_scale_) && u_scale_ != 0.0);
    assert(std::isfinite(v_scale_) && v_scale_ != 0.0);
    assert(std::isfinite(u_offset_));
    assert(std::isfinite(v_offset_));
  }

  visual_t Evaluate(
      const TextureCoordinates& coordinates) const final override {
    return NestedEvaluate(
        coordinates.Scale(u_scale_, v_scale_, u_offset_, v_offset_));
  }

 protected:
  virtual visual_t NestedEvaluate(
      const TextureCoordinates& coordinates) const = 0;

 private:
  geometric u_scale_;
  geometric v_scale_;
  geometric u_offset_;
  geometric v_offset_;
};

class UVReflectorTexture : public ReflectorTexture {
 public:
  UVReflectorTexture(geometric u_scale, geometric v_scale, geometric u_offset,
                     geometric v_offset)
      : u_scale_(u_scale),
        v_scale_(v_scale),
        u_offset_(u_offset),
        v_offset_(v_offset) {
    assert(std::isfinite(u_scale_) && u_scale_ != 0.0);
    assert(std::isfinite(v_scale_) && v_scale_ != 0.0);
    assert(std::isfinite(u_offset_));
    assert(std::isfinite(v_offset_));
  }

  const Reflector* Evaluate(const TextureCoordinates& coordinates,
                            SpectralAllocator& allocator) const final override {
    return NestedEvaluate(
        coordinates.Scale(u_scale_, v_scale_, u_offset_, v_offset_), allocator);
  }

 protected:
  virtual const Reflector* NestedEvaluate(
      const TextureCoordinates& coordinates,
      SpectralAllocator& allocator) const = 0;

 private:
  geometric u_scale_;
  geometric v_scale_;
  geometric u_offset_;
  geometric v_offset_;
};

}  // namespace internal
}  // namespace textures
}  // namespace iris

#endif  // _IRIS_TEXTURES_INTERNAL_UV_TEXTURE_
