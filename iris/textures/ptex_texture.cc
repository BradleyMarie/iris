#include "iris/textures/ptex_texture.h"

#undef NDEBUG  // Enable assertions at runtime
#include <cassert>
#include <cstdint>
#include <memory>
#include <string>

#include "iris/float.h"
#include "iris/reference_counted.h"
#include "iris/textures/float_texture.h"
#include "iris/textures/reflector_texture.h"
#include "third_party/disney/ptex/Ptexture.h"

namespace iris {
namespace textures {
namespace {

class PtexFloatTexture final : public FloatTexture {
 public:
  PtexFloatTexture(std::shared_ptr<Ptex::PtexCache> cache, std::string filename,
                   visual_t gamma)
      : cache_(std::move(cache)),
        filename_(std::move(filename)),
        gamma_(gamma) {}

  visual_t Evaluate(const TextureCoordinates& coordinates) const override;

 private:
  std::shared_ptr<Ptex::PtexCache> cache_;
  std::string filename_;
  float gamma_;
};

class PtexReflectorTexture final : public ReflectorTexture {
 public:
  PtexReflectorTexture(std::shared_ptr<Ptex::PtexCache> cache,
                       std::string filename, visual_t gamma,
                       ReferenceCounted<ReflectorTexture> r,
                       ReferenceCounted<ReflectorTexture> g,
                       ReferenceCounted<ReflectorTexture> b)
      : cache_(std::move(cache)),
        r_(std::move(r)),
        g_(std::move(g)),
        b_(std::move(b)),
        filename_(std::move(filename)),
        gamma_(gamma) {}

  const Reflector* Evaluate(const TextureCoordinates& coordinates,
                            SpectralAllocator& allocator) const override;

 private:
  std::shared_ptr<Ptex::PtexCache> cache_;
  ReferenceCounted<ReflectorTexture> r_;
  ReferenceCounted<ReflectorTexture> g_;
  ReferenceCounted<ReflectorTexture> b_;
  std::string filename_;
  float gamma_;
};

visual_t PtexFloatTexture::Evaluate(
    const TextureCoordinates& coordinates) const {
  Ptex::String error;
  Ptex::PtexTexture* texture = cache_->get(filename_.c_str(), error);
  assert(texture != nullptr);

  int num_channels = texture->numChannels();
  assert(num_channels == 1 || num_channels == 3);

  Ptex::PtexFilter::Options opts(Ptex::PtexFilter::FilterType::f_bspline);
  Ptex::PtexFilter* filter = Ptex::PtexFilter::getFilter(texture, opts);

  float results[3];
  filter->eval(results, /*firstchan=*/0, num_channels, coordinates.face_index,
               coordinates.uv[0], coordinates.uv[1], coordinates.du_dx,
               coordinates.dv_dx, coordinates.du_dy, coordinates.dv_dy);
  filter->release();
  texture->release();

  if (gamma_ != 1.0f) {
    for (int i = 0; i < num_channels; ++i) {
      if (results[i] >= 0.0f && results[i] <= 1.0f) {
        results[i] = std::pow(results[i], gamma_);
      }
    }
  }

  if (num_channels == 3) {
    results[0] = (results[0] + results[1] + results[2]) / 3.0f;
  }

  return static_cast<visual_t>(results[0]);
}

const Reflector* PtexReflectorTexture::Evaluate(
    const TextureCoordinates& coordinates, SpectralAllocator& allocator) const {
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

  Ptex::String error;
  Ptex::PtexTexture* texture = cache_->get(filename_.c_str(), error);
  assert(texture != nullptr);

  int num_channels = texture->numChannels();
  assert(num_channels == 1 || num_channels == 3);

  Ptex::PtexFilter::Options opts(Ptex::PtexFilter::FilterType::f_bspline);
  Ptex::PtexFilter* filter = Ptex::PtexFilter::getFilter(texture, opts);

  float results[3];
  filter->eval(results, /*firstchan=*/0, num_channels, coordinates.face_index,
               coordinates.uv[0], coordinates.uv[1], coordinates.du_dx,
               coordinates.dv_dx, coordinates.du_dy, coordinates.dv_dy);
  filter->release();
  texture->release();

  if (gamma_ != 1.0f) {
    for (int i = 0; i < num_channels; ++i) {
      if (results[i] >= 0.0f && results[i] <= 1.0f) {
        results[i] = std::pow(results[i], gamma_);
      }
    }
  }

  if (num_channels == 1) {
    results[1] = results[0];
    results[2] = results[0];
  }

  return allocator.UnboundedAdd(
      allocator.UnboundedScale(r, static_cast<visual_t>(results[0])),
      allocator.UnboundedScale(g, static_cast<visual_t>(results[1])),
      allocator.UnboundedScale(b, static_cast<visual_t>(results[2])));
}

}  // namespace

ReferenceCounted<FloatTexture> MakePtexTexture(
    std::shared_ptr<Ptex::PtexCache> cache, std::string filename,
    visual_t gamma) {
  if (!cache || filename.empty() || !std::isfinite(gamma) ||
      gamma <= static_cast<visual_t>(0.0)) {
    return ReferenceCounted<FloatTexture>();
  }

  Ptex::String error;
  Ptex::PtexTexture* texture = cache->get(filename.c_str(), error);
  if (!texture) {
    return ReferenceCounted<FloatTexture>();
  }

  int num_channels = texture->numChannels();
  texture->release();

  if (num_channels != 1 && num_channels != 3) {
    return ReferenceCounted<FloatTexture>();
  }

  return MakeReferenceCounted<PtexFloatTexture>(
      std::move(cache), std::move(filename), std::move(gamma));
}

ReferenceCounted<ReflectorTexture> MakePtexTexture(
    std::shared_ptr<Ptex::PtexCache> cache, std::string filename,
    visual_t gamma, ReferenceCounted<ReflectorTexture> r,
    ReferenceCounted<ReflectorTexture> g,
    ReferenceCounted<ReflectorTexture> b) {
  if (!cache || filename.empty() || !std::isfinite(gamma) ||
      gamma <= static_cast<visual_t>(0.0)) {
    return ReferenceCounted<ReflectorTexture>();
  }

  if (!r && !g && !b) {
    return ReferenceCounted<ReflectorTexture>();
  }

  Ptex::String error;
  Ptex::PtexTexture* texture = cache->get(filename.c_str(), error);
  if (!texture) {
    return ReferenceCounted<ReflectorTexture>();
  }

  int num_channels = texture->numChannels();
  texture->release();

  if (num_channels != 1 && num_channels != 3) {
    return ReferenceCounted<ReflectorTexture>();
  }

  return MakeReferenceCounted<PtexReflectorTexture>(
      std::move(cache), std::move(filename), std::move(gamma), std::move(r),
      std::move(g), std::move(b));
}

}  // namespace textures
}  // namespace iris
