#ifndef _FRONTENDS_PBRT_IMAGE_MANAGER_
#define _FRONTENDS_PBRT_IMAGE_MANAGER_

#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>

#include "frontends/pbrt/spectrum_manager.h"
#include "frontends/pbrt/texture_manager.h"
#include "iris/reference_counted.h"
#include "iris/reflector.h"
#include "iris/textures/image_texture.h"

namespace iris {
namespace pbrt_frontend {

class ImageManager {
 public:
  ImageManager(const std::filesystem::path& search_root,
               TextureManager& texture_manager,
               SpectrumManager& spectrum_manager)
      : search_root_(search_root),
        texture_manager_(texture_manager),
        spectrum_manager_(spectrum_manager) {}

  std::shared_ptr<textures::Image2D<visual>> LoadFloatImageFromSDR(
      const std::string& filename, bool gamma_correct);

  std::shared_ptr<textures::Image2D<ReferenceCounted<Reflector>>>
  LoadReflectorImageFromSDR(const std::string& filename, bool gamma_correct);

  std::shared_ptr<textures::Image2D<visual>> LoadFloatImageFromHDR(
      const std::string& filename, bool gamma_correct);

  std::shared_ptr<textures::Image2D<ReferenceCounted<Reflector>>>
  LoadReflectorImageFromHDR(const std::string& filename, bool gamma_correct);

 private:
  std::unordered_map<std::string, std::shared_ptr<textures::Image2D<visual>>>
      float_images_;
  std::unordered_map<std::string, std::shared_ptr<textures::Image2D<visual>>>
      gamma_corrected_float_images_;

  std::unordered_map<
      std::string,
      std::shared_ptr<textures::Image2D<ReferenceCounted<Reflector>>>>
      reflector_images_;
  std::unordered_map<
      std::string,
      std::shared_ptr<textures::Image2D<ReferenceCounted<Reflector>>>>
      gamma_corrected_reflector_images_;

  std::unordered_map<uint32_t, ReferenceCounted<Reflector>> reflectors_;
  std::unordered_map<uint32_t, ReferenceCounted<Reflector>>
      gamma_corrected_reflectors_;

  std::filesystem::path search_root_;
  TextureManager& texture_manager_;
  SpectrumManager& spectrum_manager_;
};

}  // namespace pbrt_frontend
}  // namespace iris

#endif  // _FRONTENDS_PBRT_IMAGE_MANAGER_
