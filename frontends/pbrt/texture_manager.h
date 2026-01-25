#ifndef _FRONTENDS_PBRT_TEXTURE_MANAGER_
#define _FRONTENDS_PBRT_TEXTURE_MANAGER_

#include <stack>
#include <string>
#include <unordered_map>

#include "frontends/pbrt/spectrum_manager.h"
#include "iris/reference_counted.h"
#include "iris/reflector.h"
#include "iris/spectral_allocator.h"
#include "iris/textures/float_texture.h"
#include "iris/textures/reflector_texture.h"
#include "pbrt_proto/v3/v3.pb.h"

namespace iris {
namespace pbrt_frontend {

class TextureManager {
 public:
  explicit TextureManager(SpectrumManager& spectrum_manager)
      : spectrum_manager_(spectrum_manager) {
    float_textures_.emplace();
    reflector_textures_.emplace();
  }

  void AttributeBegin();
  void AttributeEnd();

  ReferenceCounted<textures::FloatTexture> AllocateFloatTexture(visual value);
  ReferenceCounted<textures::FloatTexture> AllocateFloatTexture(
      const pbrt_proto::v3::FloatTextureParameter& parameter);

  ReferenceCounted<textures::ReflectorTexture> AllocateReflectorTexture(
      visual value);
  ReferenceCounted<textures::ReflectorTexture> AllocateReflectorTexture(
      const pbrt_proto::v3::Spectrum& spectrum);
  ReferenceCounted<textures::ReflectorTexture> AllocateReflectorTexture(
      const pbrt_proto::v3::SpectrumTextureParameter& parameter);

  ReferenceCounted<textures::FloatTexture> GetFloatTexture(
      const std::string& name) const;
  ReferenceCounted<textures::ReflectorTexture> GetReflectorTexture(
      const std::string& name) const;

  void Put(const std::string& name,
           ReferenceCounted<textures::FloatTexture> texture);
  void Put(const std::string& name,
           ReferenceCounted<textures::ReflectorTexture> texture);

 private:
  std::unordered_map<visual, ReferenceCounted<textures::FloatTexture>>
      constant_float_textures_;
  std::unordered_map<const Reflector*,
                     ReferenceCounted<textures::ReflectorTexture>>
      constant_reflector_textures_;
  std::stack<
      std::unordered_map<std::string, ReferenceCounted<textures::FloatTexture>>>
      float_textures_;
  std::stack<std::unordered_map<std::string,
                                ReferenceCounted<textures::ReflectorTexture>>>
      reflector_textures_;
  SpectrumManager& spectrum_manager_;
};

}  // namespace pbrt_frontend
}  // namespace iris

#endif  // _FRONTENDS_PBRT_TEXTURE_MANAGER_
