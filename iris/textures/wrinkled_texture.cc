#include "iris/textures/wrinkled_texture.h"

#include <cstdint>
#include <utility>

#include "iris/float.h"
#include "iris/reference_counted.h"
#include "iris/reflector.h"
#include "iris/texture_coordinates.h"
#include "iris/textures/float_texture.h"
#include "iris/textures/internal/math.h"
#include "iris/textures/reflector_texture.h"

namespace iris {
namespace textures {
namespace {

using ::iris::textures::internal::Turbulence;

std::pair<Vector, Vector> GetDifferentials(
    const TextureCoordinates& coordinates) {
  if (coordinates.hit_point_differentials.has_value()) {
    return std::make_pair(
        Vector(static_cast<geometric>(0.0), static_cast<geometric>(0.0),
               static_cast<geometric>(0.0)),
        Vector(static_cast<geometric>(0.0), static_cast<geometric>(0.0),
               static_cast<geometric>(0.0)));
  }

  return std::make_pair(coordinates.hit_point_differentials->dhit_point_dx,
                        coordinates.hit_point_differentials->dhit_point_dy);
}

class WrinkledFloatTexture final : public FloatTexture {
 public:
  WrinkledFloatTexture(uint8_t octaves, visual_t roughness)
      : octaves_(octaves), roughness_(roughness) {}

  visual_t Evaluate(const TextureCoordinates& coordinates) const override {
    auto [dx, dy] = GetDifferentials(coordinates);
    return Turbulence(coordinates.hit_point, dx, dy, roughness_, octaves_);
  }

 private:
  uint8_t octaves_;
  visual_t roughness_;
};

class WrinkledReflectorTexture final : public ReflectorTexture {
 public:
  WrinkledReflectorTexture(ReferenceCounted<Reflector> reflectance,
                           uint8_t octaves, visual_t roughness)
      : reflectance_(std::move(reflectance)),
        octaves_(octaves),
        roughness_(roughness) {}

  const Reflector* Evaluate(const TextureCoordinates& coordinates,
                            SpectralAllocator& allocator) const override {
    auto [dx, dy] = GetDifferentials(coordinates);
    return allocator.UnboundedScale(
        reflectance_.Get(),
        Turbulence(coordinates.hit_point, dx, dy, roughness_, octaves_));
  }

 private:
  ReferenceCounted<Reflector> reflectance_;
  uint8_t octaves_;
  visual_t roughness_;
};

}  // namespace

ReferenceCounted<FloatTexture> MakeWrinkledTexture(uint8_t octaves,
                                                   visual_t roughness) {
  if (!std::isfinite(roughness) || roughness < static_cast<visual_t>(0.0)) {
    return ReferenceCounted<FloatTexture>();
  }

  return MakeReferenceCounted<WrinkledFloatTexture>(octaves, roughness);
}

ReferenceCounted<ReflectorTexture> MakeWrinkledTexture(
    ReferenceCounted<Reflector> reflectance, uint8_t octaves,
    visual_t roughness) {
  if (!reflectance || !std::isfinite(roughness) ||
      roughness < static_cast<visual_t>(0.0)) {
    return ReferenceCounted<ReflectorTexture>();
  }

  return MakeReferenceCounted<WrinkledReflectorTexture>(std::move(reflectance),
                                                        octaves, roughness);
}

}  // namespace textures
}  // namespace iris
