#include "frontends/pbrt/textures/ptex.h"

#include <limits>
#include <memory>

#include "frontends/pbrt/texture_manager.h"
#include "iris/reference_counted.h"
#include "iris/textures/float_texture.h"
#include "iris/textures/ptex_texture.h"
#include "iris/textures/reflector_texture.h"
#include "pbrt_proto/v3/pbrt.pb.h"
#include "third_party/disney/ptex/Ptexture.h"

namespace iris {
namespace pbrt_frontend {
namespace textures {
namespace {

using ::iris::textures::MakePtexTexture;
using ::pbrt_proto::v3::FloatTexture;
using ::pbrt_proto::v3::Spectrum;
using ::pbrt_proto::v3::SpectrumTexture;

static class ErrorReporter : public PtexErrorHandler {
 private:
  void reportError(const char* error) override {
    std::cerr << "ERROR: Ptex error: " << error << std::endl;
  }
} kPtexErrorHandler;

// NOTE: This is not thread-safe and the PtexCache should probably be owned
// at the parser level.
std::shared_ptr<Ptex::PtexCache> GetTextureCache() {
  static std::weak_ptr<Ptex::PtexCache> weak_cache;

  if (std::shared_ptr<Ptex::PtexCache> strong_cache = weak_cache.lock();
      strong_cache) {
    return strong_cache;
  }

  std::shared_ptr<Ptex::PtexCache> strong_cache(
      Ptex::PtexCache::create(/*maxFiles=*/100, /*maxMem=*/0x100000000ull,
                              /*premultiply=*/true, nullptr,
                              &kPtexErrorHandler),
      [](Ptex::PtexCache* cache) { cache->release(); });
  weak_cache = strong_cache;

  return strong_cache;
}

}  // namespace

ReferenceCounted<iris::textures::FloatTexture> MakePtex(
    const FloatTexture::Ptex& ptex, TextureManager& texture_manager) {
  std::shared_ptr<Ptex::PtexCache> cache = GetTextureCache();

  if (!std::isfinite(ptex.gamma()) || ptex.gamma() <= 0.0) {
    std::cerr << "ERROR: Out of range value for parameter: gamma" << std::endl;
    exit(EXIT_FAILURE);
  }

  Ptex::String error;
  Ptex::PtexTexture* texture = cache->get(ptex.filename().c_str(), error);
  if (!texture) {
    std::cerr << "ERROR: Failed to open ptex file with error: " << error
              << std::endl;
    exit(EXIT_FAILURE);
  }

  int num_channels = texture->numChannels();
  texture->release();

  if (num_channels != 1 && num_channels != 3) {
    std::cerr << "ERROR: Only ptex textures with one or three color channels "
                 "are supported"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  return MakePtexTexture(std::move(cache), ptex.filename(), ptex.gamma());
}

ReferenceCounted<iris::textures::ReflectorTexture> MakePtex(
    const SpectrumTexture::Ptex& ptex, TextureManager& texture_manager) {
  std::shared_ptr<Ptex::PtexCache> cache = GetTextureCache();

  if (!std::isfinite(ptex.gamma()) || ptex.gamma() <= 0.0) {
    std::cerr << "ERROR: Out of range value for parameter: gamma" << std::endl;
    exit(EXIT_FAILURE);
  }

  Ptex::String error;
  Ptex::PtexTexture* texture = cache->get(ptex.filename().c_str(), error);
  if (!texture) {
    std::cerr << "ERROR: Failed to open ptex file with error: " << error
              << std::endl;
    exit(EXIT_FAILURE);
  }

  int num_channels = texture->numChannels();
  texture->release();

  if (num_channels != 1 && num_channels != 3) {
    std::cerr << "ERROR: Only ptex textures with one or three color channels "
                 "are supported"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  Spectrum r;
  r.mutable_rgb_spectrum()->set_r(1.0);

  Spectrum g;
  g.mutable_rgb_spectrum()->set_g(1.0);

  Spectrum b;
  b.mutable_rgb_spectrum()->set_b(1.0);

  return MakePtexTexture(std::move(cache), ptex.filename(), ptex.gamma(),
                         texture_manager.AllocateReflectorTexture(r),
                         texture_manager.AllocateReflectorTexture(g),
                         texture_manager.AllocateReflectorTexture(b));
}

}  // namespace textures
}  // namespace pbrt_frontend
}  // namespace iris
