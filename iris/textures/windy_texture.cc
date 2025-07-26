#include "iris/textures/windy_texture.h"

#include <cmath>
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

using ::iris::textures::internal::FractionalBrownianMotion;
using ::iris::textures::internal::ScalePoint;

class WindyFloatTexture final : public FloatTexture {
 public:
  visual_t Evaluate(const TextureCoordinates& coordinates) const override {
    visual_t wind_strength = FractionalBrownianMotion(
        ScalePoint(coordinates.p, static_cast<geometric_t>(0.1)),
        static_cast<geometric_t>(0.1) * coordinates.dp_dx,
        static_cast<geometric_t>(0.1) * coordinates.dp_dy,
        static_cast<visual_t>(0.5), 3u);
    visual_t wave_height = FractionalBrownianMotion(
        coordinates.p, coordinates.dp_dx, coordinates.dp_dy,
        static_cast<visual_t>(0.5), 6u);
    return std::abs(wind_strength) * wave_height;
  }
};

class WindyReflectorTexture final : public ReflectorTexture {
 public:
  WindyReflectorTexture(ReferenceCounted<ReflectorTexture> reflectance)
      : reflectance_(std::move(reflectance)) {}

  const Reflector* Evaluate(const TextureCoordinates& coordinates,
                            SpectralAllocator& allocator) const override {
    const Reflector* reflectance = nullptr;
    if (reflectance_) {
      reflectance = reflectance_->Evaluate(coordinates, allocator);
    }

    visual_t wind_strength = FractionalBrownianMotion(
        ScalePoint(coordinates.p, static_cast<geometric_t>(0.1)),
        static_cast<geometric_t>(0.1) * coordinates.dp_dx,
        static_cast<geometric_t>(0.1) * coordinates.dp_dy,
        static_cast<visual_t>(0.5), 3u);
    visual_t wave_height = FractionalBrownianMotion(
        coordinates.p, coordinates.dp_dx, coordinates.dp_dy,
        static_cast<visual_t>(0.5), 6u);

    return allocator.UnboundedScale(reflectance,
                                    std::abs(wind_strength) * wave_height);
  }

 private:
  ReferenceCounted<ReflectorTexture> reflectance_;
  uint8_t octaves_;
  visual_t roughness_;
};

}  // namespace

ReferenceCounted<FloatTexture> MakeWindyTexture() {
  return MakeReferenceCounted<WindyFloatTexture>();
}

ReferenceCounted<ReflectorTexture> MakeWindyTexture(
    ReferenceCounted<ReflectorTexture> reflectance) {
  if (!reflectance) {
    return ReferenceCounted<ReflectorTexture>();
  }

  return MakeReferenceCounted<WindyReflectorTexture>(std::move(reflectance));
}

}  // namespace textures
}  // namespace iris
