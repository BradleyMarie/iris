#include "iris/textures/marble_texture.h"

#include <cmath>
#include <cstdint>
#include <utility>

#include "iris/float.h"
#include "iris/matrix.h"
#include "iris/reference_counted.h"
#include "iris/reflector.h"
#include "iris/texture_coordinates.h"
#include "iris/textures/internal/math.h"
#include "iris/textures/reflector_texture.h"

namespace iris {
namespace textures {
namespace {

using ::iris::textures::internal::FractionalBrownianMotion;

constexpr visual colors[9][3] = {
    {static_cast<visual>(0.58), static_cast<visual>(0.58),
     static_cast<visual>(0.6)},
    {static_cast<visual>(0.58), static_cast<visual>(0.58),
     static_cast<visual>(0.6)},
    {static_cast<visual>(0.58), static_cast<visual>(0.58),
     static_cast<visual>(0.6)},
    {static_cast<visual>(0.5), static_cast<visual>(0.5),
     static_cast<visual>(0.5)},
    {static_cast<visual>(0.6), static_cast<visual>(0.59),
     static_cast<visual>(0.58)},
    {static_cast<visual>(0.58), static_cast<visual>(0.58),
     static_cast<visual>(0.6)},
    {static_cast<visual>(0.58), static_cast<visual>(0.58),
     static_cast<visual>(0.6)},
    {static_cast<visual>(0.2), static_cast<visual>(0.2),
     static_cast<visual>(0.33)},
    {static_cast<visual>(0.58), static_cast<visual>(0.58),
     static_cast<visual>(0.6)},
};

class MarbleReflectorTexture final : public ReflectorTexture {
 public:
  MarbleReflectorTexture(const Matrix& world_to_texture, uint8_t octaves,
                         visual_t roughness, visual_t scale, visual_t variation,
                         ReferenceCounted<ReflectorTexture> r,
                         ReferenceCounted<ReflectorTexture> g,
                         ReferenceCounted<ReflectorTexture> b)
      : world_to_texture_(world_to_texture),
        r_(std::move(r)),
        g_(std::move(g)),
        b_(std::move(b)),
        roughness_(roughness),
        scale_(scale),
        variation_(variation),
        octaves_(octaves) {}

  const Reflector* Evaluate(const TextureCoordinates& coordinates,
                            SpectralAllocator& allocator) const override {
    const Reflector* r = nullptr;
    if (r_) {
      r = r_->Evaluate(coordinates, allocator);
    }

    const Reflector* g = nullptr;
    if (g_) {
      g = g_->Evaluate(coordinates, allocator);
    }

    const Reflector* b = nullptr;
    if (b_) {
      b = b_->Evaluate(coordinates, allocator);
    }

    Point p = world_to_texture_.Multiply(coordinates.p) * scale_;
    Vector dp_dx = world_to_texture_.Multiply(coordinates.dp_dx) * scale_;
    Vector dp_dy = world_to_texture_.Multiply(coordinates.dp_dy) * scale_;

    visual_t marble = static_cast<visual_t>(p.y) +
                      variation_ * FractionalBrownianMotion(
                                       p, dp_dx, dp_dy, roughness_, octaves_);

    visual_t t = static_cast<visual_t>(0.5) +
                 static_cast<visual_t>(0.5) * std::sin(marble);

    size_t first = std::min(
        static_cast<size_t>(1),
        static_cast<size_t>(std::floor(t * static_cast<visual_t>(6.0))));
    t = t * static_cast<visual_t>(6.0) - static_cast<visual_t>(first);

    const Reflector* c0 = allocator.UnboundedAdd(
        allocator.UnboundedScale(r, colors[first + 0][0]),
        allocator.UnboundedScale(g, colors[first + 0][1]),
        allocator.UnboundedScale(b, colors[first + 0][2]));
    const Reflector* c1 = allocator.UnboundedAdd(
        allocator.UnboundedScale(r, colors[first + 1][0]),
        allocator.UnboundedScale(g, colors[first + 1][1]),
        allocator.UnboundedScale(b, colors[first + 1][2]));
    const Reflector* c2 = allocator.UnboundedAdd(
        allocator.UnboundedScale(r, colors[first + 2][0]),
        allocator.UnboundedScale(g, colors[first + 2][1]),
        allocator.UnboundedScale(b, colors[first + 2][2]));
    const Reflector* c3 = allocator.UnboundedAdd(
        allocator.UnboundedScale(r, colors[first + 3][0]),
        allocator.UnboundedScale(g, colors[first + 3][1]),
        allocator.UnboundedScale(b, colors[first + 3][2]));

    const Reflector* s0 = allocator.UnboundedAdd(
        allocator.UnboundedScale(c0, static_cast<visual_t>(1.0) - t),
        allocator.UnboundedScale(c1, t));
    const Reflector* s1 = allocator.UnboundedAdd(
        allocator.UnboundedScale(c1, static_cast<visual_t>(1.0) - t),
        allocator.UnboundedScale(c2, t));
    const Reflector* s2 = allocator.UnboundedAdd(
        allocator.UnboundedScale(c2, static_cast<visual_t>(1.0) - t),
        allocator.UnboundedScale(c3, t));

    s0 = allocator.UnboundedAdd(
        allocator.UnboundedScale(s0, static_cast<visual_t>(1.0) - t),
        allocator.UnboundedScale(s1, t));
    s1 = allocator.UnboundedAdd(
        allocator.UnboundedScale(s1, static_cast<visual_t>(1.0) - t),
        allocator.UnboundedScale(s2, t));

    const Reflector* result = allocator.UnboundedAdd(
        allocator.UnboundedScale(s0, static_cast<visual_t>(1.0) - t),
        allocator.UnboundedScale(s1, t));

    return allocator.UnboundedScale(result, static_cast<visual_t>(1.5));
  }

 private:
  Matrix world_to_texture_;
  ReferenceCounted<ReflectorTexture> r_;
  ReferenceCounted<ReflectorTexture> g_;
  ReferenceCounted<ReflectorTexture> b_;
  visual_t roughness_;
  visual_t scale_;
  visual_t variation_;
  uint8_t octaves_;
};

}  // namespace

ReferenceCounted<ReflectorTexture> MakeMarbleTexture(
    const Matrix& world_to_texture, uint8_t octaves, visual_t roughness,
    visual_t scale, visual_t variation, ReferenceCounted<ReflectorTexture> r,
    ReferenceCounted<ReflectorTexture> g,
    ReferenceCounted<ReflectorTexture> b) {
  if (!std::isfinite(roughness) || roughness < static_cast<visual_t>(0.0) ||
      !std::isfinite(scale) || !std::isfinite(variation)) {
    return ReferenceCounted<ReflectorTexture>();
  }

  if (!r && !g && !b) {
    return ReferenceCounted<ReflectorTexture>();
  }

  return MakeReferenceCounted<MarbleReflectorTexture>(
      world_to_texture, octaves, roughness, scale, variation, std::move(r),
      std::move(g), std::move(b));
}

}  // namespace textures
}  // namespace iris
