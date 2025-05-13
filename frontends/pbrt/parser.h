#ifndef _FRONTENDS_PBRT_PARSER_
#define _FRONTENDS_PBRT_PARSER_

#include <filesystem>
#include <fstream>
#include <functional>
#include <memory>
#include <optional>
#include <string_view>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "frontends/pbrt/image_manager.h"
#include "frontends/pbrt/material_manager.h"
#include "frontends/pbrt/materials/material_builder.h"
#include "frontends/pbrt/matrix_manager.h"
#include "frontends/pbrt/renderable.h"
#include "frontends/pbrt/spectrum_manager.h"
#include "frontends/pbrt/texture_manager.h"
#include "iris/camera.h"
#include "iris/image_sampler.h"
#include "iris/integrator.h"
#include "iris/light_scene.h"
#include "iris/power_matcher.h"

namespace iris {
namespace pbrt_frontend {

class Parser {
 public:
  Parser(std::unique_ptr<SpectrumManager> spectrum_manager,
         std::unique_ptr<PowerMatcher> power_matcher)
      : spectrum_manager_(std::move(spectrum_manager)),
        power_matcher_(std::move(power_matcher)) {
    spectrum_manager_->Clear();
  }

  struct Result {
    Renderable renderable;
    Renderer::SkipPixelFn skip_pixel_callback;
    std::filesystem::path output_filename;
    std::function<void(Framebuffer&, std::ofstream&)> output_write_function;
    std::optional<visual_t> maximum_sample_luminance;
  };

  std::optional<Result> ParseFrom(Tokenizer& tokenizer,
                                  const std::filesystem::path& search_root);

 private:
  std::optional<std::string_view> PeekToken();
  std::string_view NextToken();

  void InitializeDefault();

  bool AreaLightSource();
  bool AttributeBegin();
  bool AttributeEnd();
  bool Camera();
  bool Film();
  bool Include();
  bool Integrator();
  bool LightSource();
  bool MakeNamedMaterial();
  bool Material();
  bool NamedMaterial();
  bool ObjectBegin();
  bool ObjectEnd();
  bool ObjectInstance();
  bool PixelFilter();
  bool ReverseOrientation();
  bool Sampler();
  bool Shape();
  bool Texture();
  bool WorldBegin();
  bool WorldEnd();

  struct TokenizerEntry {
    Tokenizer* tokenizer;
    std::unique_ptr<Tokenizer> owned_tokenizer;
    std::unique_ptr<std::ifstream> file;
  };

  std::vector<TokenizerEntry> tokenizers_;
  const std::filesystem::path* search_root_;

  std::string texture_name_;

  struct AttributeEntry {
    std::shared_ptr<materials::NestedMaterialBuilder> material;
    std::pair<ReferenceCounted<EmissiveMaterial>,
              ReferenceCounted<EmissiveMaterial>>
        emissive_material;
    bool reverse_orientation;
  };

  std::vector<AttributeEntry> attributes_;

  std::unordered_map<std::string, ReferenceCounted<Geometry>> objects_;
  std::optional<std::string> current_object_name_;
  std::vector<ReferenceCounted<Geometry>> current_object_geometry_;

  std::unique_ptr<SpectrumManager> spectrum_manager_;
  std::unique_ptr<PowerMatcher> power_matcher_;
  std::unique_ptr<MaterialManager> material_manager_;
  std::unique_ptr<MatrixManager> matrix_manager_;
  std::unique_ptr<TextureManager> texture_manager_;
  std::unique_ptr<ImageManager> image_manager_;

  std::function<std::unique_ptr<iris::Camera>(const std::pair<size_t, size_t>&)>
      camera_;
  SceneObjects::Builder scene_objects_builder_;
  std::pair<size_t, size_t> image_dimensions_;
  Renderer::SkipPixelFn skip_pixel_callback_;
  std::optional<visual_t> maximum_sample_luminance_;
  std::unique_ptr<ImageSampler> image_sampler_;
  std::unique_ptr<iris::Integrator> integrator_;
  std::unique_ptr<LightScene::Builder> light_scene_builder_;
  std::filesystem::path output_filename_;
  std::function<void(Framebuffer&, std::ofstream&)> write_function_;

  bool camera_encountered_ = false;
  bool film_encountered_ = false;
  bool sampler_encountered_ = false;
  bool integrator_encountered_ = false;
  bool pixel_filter_encountered_ = false;
  bool world_begin_encountered_ = false;
};

}  // namespace pbrt_frontend
}  // namespace iris

#endif  // _FRONTENDS_PBRT_PARSER_
