#ifndef _IRIS_TEXTURES_IMAGE_TEXTURE_
#define _IRIS_TEXTURES_IMAGE_TEXTURE_

#include <cassert>
#include <cmath>
#include <memory>
#include <utility>
#include <vector>

#include "iris/float.h"
#include "iris/reference_counted.h"
#include "iris/spectral_allocator.h"
#include "iris/texture_coordinates.h"
#include "iris/textures/float_texture.h"
#include "iris/textures/reflector_texture.h"

namespace iris {
namespace textures {

template <typename T>
class Image2D final {
 private:
  static const geometric_t max_value_;

 public:
  Image2D(std::vector<T> image, std::pair<size_t, size_t> size)
      : image_(std::move(image)),
        size_(size),
        texel_size_(static_cast<geometric_t>(1.0) /
                        static_cast<geometric_t>(size.first),
                    static_cast<geometric_t>(1.0) /
                        static_cast<geometric_t>(size.second)) {
    assert(!image_.empty());
    assert(size_.first != 0);
    assert(size_.second != 0);
    assert(image_.size() % size.first == 0);
    assert(image_.size() % size.second == 0);
    assert(image_.size() / size.first == size.second);
  }

  const T& Get(size_t y, size_t x) const {
    assert(y < size_.first);
    assert(x < size_.second);
    return image_[y * size_.second + x];
  }

  const T& GetBordered(geometric_t u, geometric_t v, const T& border) {
    if (u < static_cast<geometric_t>(0.0) ||
        u > static_cast<geometric_t>(1.0) ||
        v < static_cast<geometric_t>(0.0) ||
        v > static_cast<geometric_t>(1.0)) {
      return border;
    }

    return Get(v * size_.first, u * size_.second);
  }

  const T& GetClamped(geometric_t u, geometric_t v) {
    u = std::max(static_cast<geometric_t>(0.0), std::min(u, max_value_));
    v = std::max(static_cast<geometric_t>(0.0), std::min(v, max_value_));
    return Get(v * size_.first, u * size_.second);
  }

  const T& GetRepeated(geometric_t u, geometric_t v) {
    u = std::min(max_value_, u - std::floor(u));
    v = std::min(max_value_, v - std::floor(v));
    return Get(v * size_.first, u * size_.second);
  }

  const std::pair<size_t, size_t>& Size() const { return size_; }

  struct SampleCoordinates {
    geometric_t low_coordinates[2][2];
    geometric_t high_coordinates[2][2];
    geometric_t left_right_interpolation;
    geometric_t low_high_interpolation;
  };

  SampleCoordinates ComputeSampleCoordinates(geometric_t u, geometric_t v) {
    geometric_t base_u = u - texel_size_.second * 0.5;
    geometric_t base_v = v - texel_size_.first * 0.5;

    SampleCoordinates coords;
    coords.low_coordinates[0][0] = base_u;
    coords.low_coordinates[0][1] = base_v;
    coords.low_coordinates[1][0] = base_u + texel_size_.second;
    coords.low_coordinates[1][1] = base_v;
    coords.high_coordinates[0][0] = base_u;
    coords.high_coordinates[0][1] = base_v + texel_size_.first;
    coords.high_coordinates[1][0] = base_u + texel_size_.second;
    coords.high_coordinates[1][1] = base_v + texel_size_.first;
    coords.left_right_interpolation = std::fmod(
        (u - std ::floor(u)) * static_cast<geometric_t>(size_.second) + 0.5,
        1.0);
    coords.low_high_interpolation = std::fmod(
        (v - std ::floor(v)) * static_cast<geometric_t>(size_.first) + 0.5,
        1.0);

    return coords;
  }

 private:
  std::vector<T> image_;
  std::pair<size_t, size_t> size_;
  std::pair<geometric_t, geometric_t> texel_size_;
};

template <typename T>
const geometric_t Image2D<T>::max_value_ = std::nextafter(
    static_cast<geometric_t>(1.0), static_cast<geometric_t>(0.0));

ReferenceCounted<FloatTexture> MakeBorderedImageTexture(
    std::shared_ptr<Image2D<visual>> image, visual_t border, geometric u_scale,
    geometric v_scale, geometric u_offset, geometric v_offset);
ReferenceCounted<FloatTexture> MakeClampedImageTexture(
    std::shared_ptr<Image2D<visual>> image, geometric u_scale,
    geometric v_scale, geometric u_offset, geometric v_offset);
ReferenceCounted<FloatTexture> MakeRepeatedImageTexture(
    std::shared_ptr<Image2D<visual>> image, geometric u_scale,
    geometric v_scale, geometric u_offset, geometric v_offset);

ReferenceCounted<ReflectorTexture> MakeBorderedImageTexture(
    std::shared_ptr<Image2D<ReferenceCounted<Reflector>>> image,
    ReferenceCounted<Reflector> border, geometric u_scale, geometric v_scale,
    geometric u_offset, geometric v_offset);
ReferenceCounted<ReflectorTexture> MakeClampedImageTexture(
    std::shared_ptr<Image2D<ReferenceCounted<Reflector>>> image,
    geometric u_scale, geometric v_scale, geometric u_offset,
    geometric v_offset);
ReferenceCounted<ReflectorTexture> MakeRepeatedImageTexture(
    std::shared_ptr<Image2D<ReferenceCounted<Reflector>>> image,
    geometric u_scale, geometric v_scale, geometric u_offset,
    geometric v_offset);

}  // namespace textures
}  // namespace iris

#endif  // _IRIS_TEXTURES_IMAGE_TEXTURE_
