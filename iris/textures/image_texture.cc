#include "iris/textures/image_texture.h"

#include <memory>
#include <utility>

#include "iris/float.h"
#include "iris/reference_counted.h"
#include "iris/reflector.h"
#include "iris/spectral_allocator.h"
#include "iris/texture_coordinates.h"
#include "iris/textures/float_texture.h"
#include "iris/textures/internal/uv_texture.h"
#include "iris/textures/reflector_texture.h"

namespace iris {
namespace textures {
namespace {

class BorderedImageFloatTexture final : public internal::UVFloatTexture {
 public:
  BorderedImageFloatTexture(std::shared_ptr<Image2D<visual>> image,
                            visual_t border, geometric u_scale,
                            geometric v_scale, geometric u_offset,
                            geometric v_offset)
      : internal::UVFloatTexture(u_scale, v_scale, u_offset, v_offset),
        image_(std::move(image)),
        border_(border) {}

 protected:
  visual_t NestedEvaluate(
      const TextureCoordinates& coordinates) const override {
    Image2D<visual>::SampleCoordinates coords =
        image_->ComputeSampleCoordinates(coordinates.uv[0], coordinates.uv[1]);

    visual_t bottom_left = image_->GetBordered(
        coords.low_coordinates[0][0], coords.low_coordinates[0][1], border_);
    visual_t bottom_right = image_->GetBordered(
        coords.low_coordinates[1][0], coords.low_coordinates[1][1], border_);
    visual_t top_left = image_->GetBordered(
        coords.high_coordinates[0][0], coords.high_coordinates[0][1], border_);
    visual_t top_right = image_->GetBordered(
        coords.high_coordinates[1][0], coords.high_coordinates[1][1], border_);

    visual_t bottom =
        std::lerp(bottom_left, bottom_right, coords.left_right_interpolation);
    visual_t top =
        std::lerp(top_left, top_right, coords.left_right_interpolation);

    return std::lerp(bottom, top, coords.low_high_interpolation);
  }

 private:
  std::shared_ptr<Image2D<visual>> image_;
  visual_t border_;
};

class ClampedImageFloatTexture final : public internal::UVFloatTexture {
 public:
  ClampedImageFloatTexture(std::shared_ptr<Image2D<visual>> image,
                           geometric u_scale, geometric v_scale,
                           geometric u_offset, geometric v_offset)
      : internal::UVFloatTexture(u_scale, v_scale, u_offset, v_offset),
        image_(std::move(image)) {}

 protected:
  visual_t NestedEvaluate(
      const TextureCoordinates& coordinates) const override {
    Image2D<visual>::SampleCoordinates coords =
        image_->ComputeSampleCoordinates(coordinates.uv[0], coordinates.uv[1]);

    visual_t bottom_left = image_->GetClamped(coords.low_coordinates[0][0],
                                              coords.low_coordinates[0][1]);
    visual_t bottom_right = image_->GetClamped(coords.low_coordinates[1][0],
                                               coords.low_coordinates[1][1]);
    visual_t top_left = image_->GetClamped(coords.high_coordinates[0][0],
                                           coords.high_coordinates[0][1]);
    visual_t top_right = image_->GetClamped(coords.high_coordinates[1][0],
                                            coords.high_coordinates[1][1]);

    visual_t bottom =
        std::lerp(bottom_left, bottom_right, coords.left_right_interpolation);
    visual_t top =
        std::lerp(top_left, top_right, coords.left_right_interpolation);

    return std::lerp(bottom, top, coords.low_high_interpolation);
  }

 private:
  std::shared_ptr<Image2D<visual>> image_;
};

class RepeatedImageFloatTexture final : public internal::UVFloatTexture {
 public:
  RepeatedImageFloatTexture(std::shared_ptr<Image2D<visual>> image,
                            geometric u_scale, geometric v_scale,
                            geometric u_offset, geometric v_offset)
      : internal::UVFloatTexture(u_scale, v_scale, u_offset, v_offset),
        image_(std::move(image)) {}

 protected:
  visual_t NestedEvaluate(
      const TextureCoordinates& coordinates) const override {
    Image2D<visual>::SampleCoordinates coords =
        image_->ComputeSampleCoordinates(coordinates.uv[0], coordinates.uv[1]);

    visual_t bottom_left = image_->GetRepeated(coords.low_coordinates[0][0],
                                               coords.low_coordinates[0][1]);
    visual_t bottom_right = image_->GetRepeated(coords.low_coordinates[1][0],
                                                coords.low_coordinates[1][1]);
    visual_t top_left = image_->GetRepeated(coords.high_coordinates[0][0],
                                            coords.high_coordinates[0][1]);
    visual_t top_right = image_->GetRepeated(coords.high_coordinates[1][0],
                                             coords.high_coordinates[1][1]);

    visual_t bottom =
        std::lerp(bottom_left, bottom_right, coords.left_right_interpolation);
    visual_t top =
        std::lerp(top_left, top_right, coords.left_right_interpolation);

    return std::lerp(bottom, top, coords.low_high_interpolation);
  }

 private:
  std::shared_ptr<Image2D<visual>> image_;
};

class BorderedImageReflectorTexture final
    : public internal::UVReflectorTexture {
 public:
  BorderedImageReflectorTexture(
      std::shared_ptr<Image2D<ReferenceCounted<Reflector>>> image,
      ReferenceCounted<Reflector> border, geometric u_scale, geometric v_scale,
      geometric u_offset, geometric v_offset)
      : internal::UVReflectorTexture(u_scale, v_scale, u_offset, v_offset),
        image_(std::move(image)),
        border_(std::move(border)) {}

 protected:
  const Reflector* NestedEvaluate(const TextureCoordinates& coordinates,
                                  SpectralAllocator& allocator) const override {
    return image_->GetBordered(coordinates.uv[0], coordinates.uv[1], border_)
        .Get();
  }

 private:
  std::shared_ptr<Image2D<ReferenceCounted<Reflector>>> image_;
  ReferenceCounted<Reflector> border_;
};

class ClampedImageReflectorTexture final : public internal::UVReflectorTexture {
 public:
  ClampedImageReflectorTexture(
      std::shared_ptr<Image2D<ReferenceCounted<Reflector>>> image,
      geometric u_scale, geometric v_scale, geometric u_offset,
      geometric v_offset)
      : internal::UVReflectorTexture(u_scale, v_scale, u_offset, v_offset),
        image_(std::move(image)) {}

 protected:
  const Reflector* NestedEvaluate(const TextureCoordinates& coordinates,
                                  SpectralAllocator& allocator) const override {
    return image_->GetClamped(coordinates.uv[0], coordinates.uv[1]).Get();
  }

 private:
  std::shared_ptr<Image2D<ReferenceCounted<Reflector>>> image_;
};

class RepeatedImageReflectorTexture final
    : public internal::UVReflectorTexture {
 public:
  RepeatedImageReflectorTexture(
      std::shared_ptr<Image2D<ReferenceCounted<Reflector>>> image,
      geometric u_scale, geometric v_scale, geometric u_offset,
      geometric v_offset)
      : internal::UVReflectorTexture(u_scale, v_scale, u_offset, v_offset),
        image_(std::move(image)) {}

 protected:
  const Reflector* NestedEvaluate(const TextureCoordinates& coordinates,
                                  SpectralAllocator& allocator) const override {
    return image_->GetRepeated(coordinates.uv[0], coordinates.uv[1]).Get();
  }

 private:
  std::shared_ptr<Image2D<ReferenceCounted<Reflector>>> image_;
};

}  // namespace

ReferenceCounted<FloatTexture> MakeBorderedImageTexture(
    std::shared_ptr<Image2D<visual>> image, visual_t border, geometric u_scale,
    geometric v_scale, geometric u_offset, geometric v_offset) {
  if (!image || !std::isfinite(border) || !std::isfinite(u_scale) ||
      u_scale == static_cast<visual>(0.0) || !std::isfinite(v_scale) ||
      v_scale == static_cast<visual>(0.0) || !std::isfinite(u_offset) ||
      !std::isfinite(v_offset)) {
    return ReferenceCounted<FloatTexture>();
  }

  return MakeReferenceCounted<BorderedImageFloatTexture>(
      std::move(image), border, u_scale, v_scale, u_offset, v_offset);
}

ReferenceCounted<FloatTexture> MakeClampedImageTexture(
    std::shared_ptr<Image2D<visual>> image, geometric u_scale,
    geometric v_scale, geometric u_offset, geometric v_offset) {
  if (!image || !std::isfinite(u_scale) ||
      u_scale == static_cast<visual>(0.0) || !std::isfinite(v_scale) ||
      v_scale == static_cast<visual>(0.0) || !std::isfinite(u_offset) ||
      !std::isfinite(v_offset)) {
    return ReferenceCounted<FloatTexture>();
  }

  return MakeReferenceCounted<ClampedImageFloatTexture>(
      std::move(image), u_scale, v_scale, u_offset, v_offset);
}

ReferenceCounted<FloatTexture> MakeRepeatedImageTexture(
    std::shared_ptr<Image2D<visual>> image, geometric u_scale,
    geometric v_scale, geometric u_offset, geometric v_offset) {
  if (!image || !std::isfinite(u_scale) ||
      u_scale == static_cast<visual>(0.0) || !std::isfinite(v_scale) ||
      v_scale == static_cast<visual>(0.0) || !std::isfinite(u_offset) ||
      !std::isfinite(v_offset)) {
    return ReferenceCounted<FloatTexture>();
  }

  return MakeReferenceCounted<RepeatedImageFloatTexture>(
      std::move(image), u_scale, v_scale, u_offset, v_offset);
}

ReferenceCounted<ReflectorTexture> MakeBorderedImageTexture(
    std::shared_ptr<Image2D<ReferenceCounted<Reflector>>> image,
    ReferenceCounted<Reflector> border, geometric u_scale, geometric v_scale,
    geometric u_offset, geometric v_offset) {
  if (!image || !std::isfinite(u_scale) ||
      u_scale == static_cast<visual>(0.0) || !std::isfinite(v_scale) ||
      v_scale == static_cast<visual>(0.0) || !std::isfinite(u_offset) ||
      !std::isfinite(v_offset)) {
    return ReferenceCounted<ReflectorTexture>();
  }

  return MakeReferenceCounted<BorderedImageReflectorTexture>(
      std::move(image), std::move(border), u_scale, v_scale, u_offset,
      v_offset);
}

ReferenceCounted<ReflectorTexture> MakeClampedImageTexture(
    std::shared_ptr<Image2D<ReferenceCounted<Reflector>>> image,
    geometric u_scale, geometric v_scale, geometric u_offset,
    geometric v_offset) {
  if (!image || !std::isfinite(u_scale) ||
      u_scale == static_cast<visual>(0.0) || !std::isfinite(v_scale) ||
      v_scale == static_cast<visual>(0.0) || !std::isfinite(u_offset) ||
      !std::isfinite(v_offset)) {
    return ReferenceCounted<ReflectorTexture>();
  }

  return MakeReferenceCounted<ClampedImageReflectorTexture>(
      std::move(image), u_scale, v_scale, u_offset, v_offset);
}

ReferenceCounted<ReflectorTexture> MakeRepeatedImageTexture(
    std::shared_ptr<Image2D<ReferenceCounted<Reflector>>> image,
    geometric u_scale, geometric v_scale, geometric u_offset,
    geometric v_offset) {
  if (!image || !std::isfinite(u_scale) ||
      u_scale == static_cast<visual>(0.0) || !std::isfinite(v_scale) ||
      v_scale == static_cast<visual>(0.0) || !std::isfinite(u_offset) ||
      !std::isfinite(v_offset)) {
    return ReferenceCounted<ReflectorTexture>();
  }

  return MakeReferenceCounted<RepeatedImageReflectorTexture>(
      std::move(image), u_scale, v_scale, u_offset, v_offset);
}

}  // namespace textures
}  // namespace iris
