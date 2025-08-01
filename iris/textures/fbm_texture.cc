#include "iris/textures/fbm_texture.h"

#include <cstdint>
#include <utility>

#include "iris/float.h"
#include "iris/matrix.h"
#include "iris/reference_counted.h"
#include "iris/reflector.h"
#include "iris/texture_coordinates.h"
#include "iris/textures/float_texture.h"
#include "iris/textures/internal/math.h"
#include "iris/textures/reflector_texture.h"

namespace iris {
namespace textures {
namespace {

using ::iris::textures::internal::FractionalBrownianMotion;

class FbmFloatTexture final : public FloatTexture {
 public:
  FbmFloatTexture(const Matrix& world_to_texture, uint8_t octaves,
                  visual_t roughness)
      : world_to_texture_(world_to_texture),
        roughness_(roughness),
        octaves_(octaves) {}

  visual_t Evaluate(const TextureCoordinates& coordinates) const override {
    return FractionalBrownianMotion(
        world_to_texture_.Multiply(coordinates.p),
        world_to_texture_.Multiply(coordinates.dp_dx),
        world_to_texture_.Multiply(coordinates.dp_dy), roughness_, octaves_);
  }

 private:
  Matrix world_to_texture_;
  visual_t roughness_;
  uint8_t octaves_;
};

class FbmReflectorTexture final : public ReflectorTexture {
 public:
  FbmReflectorTexture(const Matrix& world_to_texture,
                      ReferenceCounted<ReflectorTexture> reflectance,
                      uint8_t octaves, visual_t roughness)
      : world_to_texture_(world_to_texture),
        reflectance_(std::move(reflectance)),
        roughness_(roughness),
        octaves_(octaves) {}

  const Reflector* Evaluate(const TextureCoordinates& coordinates,
                            SpectralAllocator& allocator) const override {
    const Reflector* reflectance = nullptr;
    if (reflectance_) {
      reflectance = reflectance_->Evaluate(coordinates, allocator);
    }

    return allocator.UnboundedScale(
        reflectance,
        FractionalBrownianMotion(world_to_texture_.Multiply(coordinates.p),
                                 world_to_texture_.Multiply(coordinates.dp_dx),
                                 world_to_texture_.Multiply(coordinates.dp_dy),
                                 roughness_, octaves_));
  }

 private:
  Matrix world_to_texture_;
  ReferenceCounted<ReflectorTexture> reflectance_;
  visual_t roughness_;
  uint8_t octaves_;
};

}  // namespace

ReferenceCounted<FloatTexture> MakeFbmTexture(const Matrix& world_to_texture,
                                              uint8_t octaves,
                                              visual_t roughness) {
  if (!std::isfinite(roughness) || roughness < static_cast<visual_t>(0.0)) {
    return ReferenceCounted<FloatTexture>();
  }

  return MakeReferenceCounted<FbmFloatTexture>(world_to_texture, octaves,
                                               roughness);
}

ReferenceCounted<ReflectorTexture> MakeFbmTexture(
    const Matrix& world_to_texture, uint8_t octaves, visual_t roughness,
    ReferenceCounted<ReflectorTexture> reflectance) {
  if (!reflectance || !std::isfinite(roughness) ||
      roughness < static_cast<visual_t>(0.0)) {
    return ReferenceCounted<ReflectorTexture>();
  }

  return MakeReferenceCounted<FbmReflectorTexture>(
      world_to_texture, std::move(reflectance), octaves, roughness);
}

}  // namespace textures
}  // namespace iris
