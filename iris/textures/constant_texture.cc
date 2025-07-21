#include "iris/textures/constant_texture.h"

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

class ConstantFloatTexture final : public FloatTexture {
 public:
  ConstantFloatTexture(visual value) : value_(value) {}

  visual_t Evaluate(const TextureCoordinates& coordinates) const override {
    return value_;
  }

 private:
  visual value_;
};

class ConstantReflectorTexture final : public ReflectorTexture {
 public:
  ConstantReflectorTexture(ReferenceCounted<Reflector> value)
      : value_(std::move(value)) {}

  const Reflector* Evaluate(const TextureCoordinates& coordinates,
                            SpectralAllocator& allocator) const override {
    return value_.Get();
  }

 private:
  ReferenceCounted<Reflector> value_;
};

}  // namespace

ReferenceCounted<FloatTexture> MakeConstantTexture(visual value) {
  if (value == static_cast<visual>(0.0)) {
    return ReferenceCounted<FloatTexture>();
  }

  return MakeReferenceCounted<ConstantFloatTexture>(value);
}

ReferenceCounted<ReflectorTexture> MakeConstantTexture(
    ReferenceCounted<Reflector> reflectance) {
  if (!reflectance) {
    return ReferenceCounted<ReflectorTexture>();
  }

  return MakeReferenceCounted<ConstantReflectorTexture>(std::move(reflectance));
}

}  // namespace textures
}  // namespace iris
