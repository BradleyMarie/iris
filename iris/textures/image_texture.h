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
      : image_(std::move(image)), size_(size) {
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
    u = std::fmod(std::floor(u) + u, static_cast<geometric_t>(1.0));
    if (u < 0.0) {
      u += 1.0;
    }

    v = std::fmod(std::floor(v) + v, static_cast<geometric_t>(1.0));
    if (v < 0.0) {
      v += 1.0;
    }

    return Get(v * size_.first, u * size_.second);
  }

  std::pair<size_t, size_t> Size() const { return size_; }

 private:
  const std::vector<T> image_;
  const std::pair<size_t, size_t> size_;
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
    return image_->GetBordered(coordinates.uv[0], coordinates.uv[1], border_);
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
    return image_->GetClamped(coordinates.uv[0], coordinates.uv[1]);
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
    return image_->GetRepeated(coordinates.uv[0], coordinates.uv[1]);
  }

 private:
  const std::shared_ptr<Image2D<T>> image_;
};

}  // namespace textures
}  // namespace iris

#endif  // _IRIS_TEXTURES_IMAGE_TEXTURE_