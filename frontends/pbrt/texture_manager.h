#ifndef _FRONTENDS_PBRT_TEXTURE_MANAGER_
#define _FRONTENDS_PBRT_TEXTURE_MANAGER_

#include <string>
#include <unordered_map>

#include "frontends/pbrt/spectrum_manager.h"
#include "iris/reference_counted.h"
#include "iris/reflector.h"
#include "iris/spectral_allocator.h"
#include "iris/textures/texture2d.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {

class TextureManager {
 public:
  using FloatTexturePtr = ReferenceCounted<textures::ValueTexture2D<visual>>;
  using ReflectorTexturePtr = ReferenceCounted<
      textures::PointerTexture2D<Reflector, SpectralAllocator>>;

  explicit TextureManager(SpectrumManager& spectrum_manager)
      : spectrum_manager_(spectrum_manager) {}

  FloatTexturePtr AllocateFloatTexture(visual value);
  FloatTexturePtr AllocateFloatTexture(
      const pbrt_proto::v3::FloatTextureParameter& parameter);

  ReflectorTexturePtr AllocateReflectorTexture(visual value);
  ReflectorTexturePtr AllocateReflectorTexture(
      const pbrt_proto::v3::Spectrum& spectrum);
  ReflectorTexturePtr AllocateReflectorTexture(
      const pbrt_proto::v3::SpectrumTextureParameter& parameter);

  FloatTexturePtr GetFloatTexture(const std::string& name) const;
  ReflectorTexturePtr GetReflectorTexture(const std::string& name) const;

  void Put(const std::string& name, FloatTexturePtr texture);
  void Put(const std::string& name, ReflectorTexturePtr texture);

 private:
  std::unordered_map<visual, FloatTexturePtr> constant_float_textures_;
  std::unordered_map<const Reflector*, ReflectorTexturePtr>
      constant_reflector_textures_;
  std::unordered_map<std::string, FloatTexturePtr> float_textures_;
  std::unordered_map<std::string, ReflectorTexturePtr> reflector_textures_;
  SpectrumManager& spectrum_manager_;
};

}  // namespace pbrt_frontend
}  // namespace iris

#endif  // _FRONTENDS_PBRT_TEXTURE_MANAGER_
