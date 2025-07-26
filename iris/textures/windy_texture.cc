#include "iris/textures/windy_texture.h"

#include <cmath>
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

class WindyFloatTexture final : public FloatTexture {
 public:
  WindyFloatTexture(const Matrix& world_to_texture)
      : world_to_texture_(world_to_texture) {}

  visual_t Evaluate(const TextureCoordinates& coordinates) const override {
    Point p = world_to_texture_.Multiply(coordinates.p);
    Vector dp_dx = world_to_texture_.Multiply(coordinates.dp_dx);
    Vector dp_dy = world_to_texture_.Multiply(coordinates.dp_dy);

    visual_t wind_strength = FractionalBrownianMotion(
        static_cast<geometric>(0.1) * p, static_cast<geometric>(0.1) * dp_dx,
        static_cast<geometric>(0.1) * dp_dy, static_cast<visual_t>(0.5), 3u);
    visual_t wave_height = FractionalBrownianMotion(
        p, dp_dx, dp_dy, static_cast<visual_t>(0.5), 6u);
    return std::abs(wind_strength) * wave_height;
  }

 private:
  Matrix world_to_texture_;
};

class WindyReflectorTexture final : public ReflectorTexture {
 public:
  WindyReflectorTexture(const Matrix& world_to_texture,
                        ReferenceCounted<ReflectorTexture> reflectance)
      : reflectance_(std::move(reflectance)),
        world_to_texture_(world_to_texture) {}

  const Reflector* Evaluate(const TextureCoordinates& coordinates,
                            SpectralAllocator& allocator) const override {
    const Reflector* reflectance = nullptr;
    if (reflectance_) {
      reflectance = reflectance_->Evaluate(coordinates, allocator);
    }

    Point p = world_to_texture_.Multiply(coordinates.p);
    Vector dp_dx = world_to_texture_.Multiply(coordinates.dp_dx);
    Vector dp_dy = world_to_texture_.Multiply(coordinates.dp_dy);

    visual_t wind_strength = FractionalBrownianMotion(
        static_cast<geometric>(0.1) * p, static_cast<geometric>(0.1) * dp_dx,
        static_cast<geometric>(0.1) * dp_dy, static_cast<visual_t>(0.5), 3u);
    visual_t wave_height = FractionalBrownianMotion(
        p, dp_dx, dp_dy, static_cast<visual_t>(0.5), 6u);

    return allocator.UnboundedScale(reflectance,
                                    std::abs(wind_strength) * wave_height);
  }

 private:
  ReferenceCounted<ReflectorTexture> reflectance_;
  Matrix world_to_texture_;
};

}  // namespace

ReferenceCounted<FloatTexture> MakeWindyTexture(
    const Matrix& world_to_texture) {
  return MakeReferenceCounted<WindyFloatTexture>(world_to_texture);
}

ReferenceCounted<ReflectorTexture> MakeWindyTexture(
    const Matrix& world_to_texture,
    ReferenceCounted<ReflectorTexture> reflectance) {
  if (!reflectance) {
    return ReferenceCounted<ReflectorTexture>();
  }

  return MakeReferenceCounted<WindyReflectorTexture>(world_to_texture,
                                                     std::move(reflectance));
}

}  // namespace textures
}  // namespace iris
