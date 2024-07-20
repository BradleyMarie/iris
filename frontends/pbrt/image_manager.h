#ifndef _FRONTENDS_PBRT_IMAGE_MANAGER_
#define _FRONTENDS_PBRT_IMAGE_MANAGER_

#include <filesystem>
#include <memory>
#include <unordered_map>

#include "frontends/pbrt/spectrum_manager.h"
#include "frontends/pbrt/texture_manager.h"
#include "iris/reference_counted.h"
#include "iris/reflector.h"
#include "iris/textures/image_texture.h"

namespace iris::pbrt_frontend {

class ImageManager {
 public:
  ImageManager(TextureManager& texture_manager,
               SpectrumManager& spectrum_manager)
      : texture_manager_(texture_manager),
        spectrum_manager_(spectrum_manager) {}

  std::shared_ptr<iris::textures::Image2D<visual>> LoadFloatImageFromSDR(
      const std::filesystem::path& filename, bool gamma_correct);

  std::shared_ptr<iris::textures::Image2D<ReferenceCounted<Reflector>>>
  LoadReflectorImageFromSDR(const std::filesystem::path& filename,
                            bool gamma_correct);

 private:
  std::unordered_map<std::string,
                     std::shared_ptr<iris::textures::Image2D<visual>>>
      float_images_;
  std::unordered_map<std::string,
                     std::shared_ptr<iris::textures::Image2D<visual>>>
      gamma_corrected_float_images_;

  std::unordered_map<
      std::string,
      std::shared_ptr<iris::textures::Image2D<ReferenceCounted<Reflector>>>>
      reflector_images_;
  std::unordered_map<
      std::string,
      std::shared_ptr<iris::textures::Image2D<ReferenceCounted<Reflector>>>>
      gamma_corrected_reflector_images_;

  std::unordered_map<uint32_t, ReferenceCounted<Reflector>> reflectors_;
  std::unordered_map<uint32_t, ReferenceCounted<Reflector>>
      gamma_corrected_reflectors_;

  TextureManager& texture_manager_;
  SpectrumManager& spectrum_manager_;
};

}  // namespace iris::pbrt_frontend

#endif  // _FRONTENDS_PBRT_IMAGE_MANAGER_