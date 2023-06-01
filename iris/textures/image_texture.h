#ifndef _IRIS_TEXTURES_IMAGE_TEXTURE_
#define _IRIS_TEXTURES_IMAGE_TEXTURE_

#include <cassert>
#include <cmath>
#include <memory>
#include <vector>

#include "iris/reference_counted.h"
#include "iris/spectral_allocator.h"
#include "iris/texture_coordinates.h"
#include "iris/textures/uv_texture.h"

namespace iris {
namespace textures {

template <typename T>
class Image2D final {
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
    if (u < 0.0 || u > 1.0 || v < 0.0 || v > 1.0) {
      return border;
    }

    return Get(v * size_.first, u * size_.second);
  }

  const T& GetClamped(geometric_t u, geometric_t v) {
    static constexpr geometric_t max_value = std::nextafter(
        static_cast<geometric_t>(1.0), static_cast<geometric_t>(0.0));
    u = std::max(static_cast<geometric_t>(0.0), std::min(u, max_value));
    v = std::max(static_cast<geometric_t>(0.0), std::min(v, max_value));
    return Get(v * size_.first, u * size_.second);
  }

  const T& GetRepeated(geometric_t u, geometric_t v) {
    u = u - std::floor(u);
    v = v - std::floor(v);
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
  const std::vector<T> image_;
  const std::pair<size_t, size_t> size_;
  const std::pair<geometric_t, geometric_t> texel_size_;
};

template <typename Return, typename... Args>
class BorderedSpectralImageTexture2D final
    : public UVPointerTexture2D<Return, Args...> {
 public:
  BorderedSpectralImageTexture2D(
      std::shared_ptr<Image2D<iris::ReferenceCounted<Return>>> image,
      iris::ReferenceCounted<Return> border, std::optional<geometric> u_scale,
      std::optional<geometric> v_scale, std::optional<geometric> u_offset,
      std::optional<geometric> v_offset)
      : UVPointerTexture2D<Return, Args...>(u_scale, v_scale, u_offset,
                                            v_offset),
        image_(std::move(image)),
        border_(std::move(border)) {}

 protected:
  const Return* NestedEvaluate(const TextureCoordinates& coordinates,
                               Args&... args) const override {
    return image_->GetBordered(coordinates.uv[0], coordinates.uv[1], border_)
        .Get();
  }

 private:
  const std::shared_ptr<Image2D<iris::ReferenceCounted<Return>>> image_;
  const iris::ReferenceCounted<Return> border_;
};

template <typename Return, typename... Args>
class ClampedSpectralImageTexture2D final
    : public UVPointerTexture2D<Return, Args...> {
 public:
  ClampedSpectralImageTexture2D(
      std::shared_ptr<Image2D<iris::ReferenceCounted<Return>>> image,
      std::optional<geometric> u_scale, std::optional<geometric> v_scale,
      std::optional<geometric> u_offset, std::optional<geometric> v_offset)
      : UVPointerTexture2D<Return, Args...>(u_scale, v_scale, u_offset,
                                            v_offset),
        image_(std::move(image)) {}

 protected:
  const Return* NestedEvaluate(const TextureCoordinates& coordinates,
                               Args&... args) const override {
    return image_->GetClamped(coordinates.uv[0], coordinates.uv[1]).Get();
  }

 private:
  const std::shared_ptr<Image2D<iris::ReferenceCounted<Return>>> image_;
};

template <typename Return, typename... Args>
class RepeatedSpectralImageTexture2D final
    : public UVPointerTexture2D<Return, Args...> {
 public:
  RepeatedSpectralImageTexture2D(
      std::shared_ptr<Image2D<iris::ReferenceCounted<Return>>> image,
      std::optional<geometric> u_scale, std::optional<geometric> v_scale,
      std::optional<geometric> u_offset, std::optional<geometric> v_offset)
      : UVPointerTexture2D<Return, Args...>(u_scale, v_scale, u_offset,
                                            v_offset),
        image_(std::move(image)) {}

 protected:
  const Return* NestedEvaluate(const TextureCoordinates& coordinates,
                               Args&... args) const override {
    return image_->GetRepeated(coordinates.uv[0], coordinates.uv[1]).Get();
  }

 private:
  const std::shared_ptr<Image2D<iris::ReferenceCounted<Return>>> image_;
};

template <typename T>
class BorderedImageTexture2D final : public UVValueTexture2D<T> {
 public:
  BorderedImageTexture2D(std::shared_ptr<Image2D<T>> image, T border,
                         std::optional<geometric> u_scale,
                         std::optional<geometric> v_scale,
                         std::optional<geometric> u_offset,
                         std::optional<geometric> v_offset)
      : UVValueTexture2D<T>(u_scale, v_scale, u_offset, v_offset),
        image_(std::move(image)),
        border_(border) {}

 protected:
  T NestedEvaluate(const TextureCoordinates& coordinates) const override {
    auto coords =
        image_->ComputeSampleCoordinates(coordinates.uv[0], coordinates.uv[1]);

    const auto& bottom_left = image_->GetBordered(
        coords.low_coordinates[0][0], coords.low_coordinates[0][1], border_);
    const auto& bottom_right = image_->GetBordered(
        coords.low_coordinates[1][0], coords.low_coordinates[1][1], border_);
    const auto& top_left = image_->GetBordered(
        coords.high_coordinates[0][0], coords.high_coordinates[0][1], border_);
    const auto& top_right = image_->GetBordered(
        coords.high_coordinates[1][0], coords.high_coordinates[1][1], border_);

    auto bottom =
        std::lerp(bottom_left, bottom_right, coords.left_right_interpolation);
    auto top = std::lerp(top_left, top_right, coords.left_right_interpolation);
    return std::lerp(bottom, top, coords.low_high_interpolation);
  }

 private:
  const std::shared_ptr<Image2D<T>> image_;
  const T border_;
};

template <typename T>
class ClampedImageTexture2D final : public UVValueTexture2D<T> {
 public:
  ClampedImageTexture2D(std::shared_ptr<Image2D<T>> image,
                        std::optional<geometric> u_scale,
                        std::optional<geometric> v_scale,
                        std::optional<geometric> u_offset,
                        std::optional<geometric> v_offset)
      : UVValueTexture2D<T>(u_scale, v_scale, u_offset, v_offset),
        image_(std::move(image)) {}

 protected:
  T NestedEvaluate(const TextureCoordinates& coordinates) const override {
    auto coords =
        image_->ComputeSampleCoordinates(coordinates.uv[0], coordinates.uv[1]);

    const auto& bottom_left = image_->GetClamped(coords.low_coordinates[0][0],
                                                 coords.low_coordinates[0][1]);
    const auto& bottom_right = image_->GetClamped(coords.low_coordinates[1][0],
                                                  coords.low_coordinates[1][1]);
    const auto& top_left = image_->GetClamped(coords.high_coordinates[0][0],
                                              coords.high_coordinates[0][1]);
    const auto& top_right = image_->GetClamped(coords.high_coordinates[1][0],
                                               coords.high_coordinates[1][1]);

    auto bottom =
        std::lerp(bottom_left, bottom_right, coords.left_right_interpolation);
    auto top = std::lerp(top_left, top_right, coords.left_right_interpolation);
    return std::lerp(bottom, top, coords.low_high_interpolation);
  }

 private:
  const std::shared_ptr<Image2D<T>> image_;
};

template <typename T>
class RepeatedImageTexture2D final : public UVValueTexture2D<T> {
 public:
  RepeatedImageTexture2D(std::shared_ptr<Image2D<T>> image,
                         std::optional<geometric> u_scale,
                         std::optional<geometric> v_scale,
                         std::optional<geometric> u_offset,
                         std::optional<geometric> v_offset)
      : UVValueTexture2D<T>(u_scale, v_scale, u_offset, v_offset),
        image_(std::move(image)) {}

 protected:
  T NestedEvaluate(const TextureCoordinates& coordinates) const override {
    auto coords =
        image_->ComputeSampleCoordinates(coordinates.uv[0], coordinates.uv[1]);

    const auto& bottom_left = image_->GetRepeated(coords.low_coordinates[0][0],
                                                  coords.low_coordinates[0][1]);
    const auto& bottom_right = image_->GetRepeated(
        coords.low_coordinates[1][0], coords.low_coordinates[1][1]);
    const auto& top_left = image_->GetRepeated(coords.high_coordinates[0][0],
                                               coords.high_coordinates[0][1]);
    const auto& top_right = image_->GetRepeated(coords.high_coordinates[1][0],
                                                coords.high_coordinates[1][1]);

    auto bottom =
        std::lerp(bottom_left, bottom_right, coords.left_right_interpolation);
    auto top = std::lerp(top_left, top_right, coords.left_right_interpolation);
    return std::lerp(bottom, top, coords.low_high_interpolation);
  }

 private:
  const std::shared_ptr<Image2D<T>> image_;
};

}  // namespace textures
}  // namespace iris

#endif  // _IRIS_TEXTURES_IMAGE_TEXTURE_