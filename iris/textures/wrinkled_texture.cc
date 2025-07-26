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

class WrinkledFloatTexture final : public FloatTexture {
 public:
  WrinkledFloatTexture(uint8_t octaves, visual_t roughness)
      : octaves_(octaves), roughness_(roughness) {}

  visual_t Evaluate(const TextureCoordinates& coordinates) const override {
    return Turbulence(coordinates.p, coordinates.dp_dx, coordinates.dp_dy,
                      roughness_, octaves_);
  }

 private:
  uint8_t octaves_;
  visual_t roughness_;
};

class WrinkledReflectorTexture final : public ReflectorTexture {
 public:
  WrinkledReflectorTexture(ReferenceCounted<ReflectorTexture> reflectance,
                           uint8_t octaves, visual_t roughness)
      : reflectance_(std::move(reflectance)),
        octaves_(octaves),
        roughness_(roughness) {}

  const Reflector* Evaluate(const TextureCoordinates& coordinates,
                            SpectralAllocator& allocator) const override {
    const Reflector* reflectance = nullptr;
    if (reflectance_) {
      reflectance = reflectance_->Evaluate(coordinates, allocator);
    }

    return allocator.UnboundedScale(
        reflectance, Turbulence(coordinates.p, coordinates.dp_dx,
                                coordinates.dp_dy, roughness_, octaves_));
  }

 private:
  ReferenceCounted<ReflectorTexture> reflectance_;
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
    ReferenceCounted<ReflectorTexture> reflectance, uint8_t octaves,
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
