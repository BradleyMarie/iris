#ifndef _IRIS_RENDERER_
#define _IRIS_RENDERER_

#include <functional>
#include <memory>
#include <optional>
#include <utility>

#include "iris/albedo_matcher.h"
#include "iris/camera.h"
#include "iris/color_matcher.h"
#include "iris/float.h"
#include "iris/framebuffer.h"
#include "iris/image_sampler.h"
#include "iris/integrator.h"
#include "iris/light_scene.h"
#include "iris/power_matcher.h"
#include "iris/random_bitstream.h"
#include "iris/scene.h"
#include "iris/scene_objects.h"

namespace iris {

class Renderer final {
 public:
  Renderer(const Scene::Builder& scene_builder,
           const LightScene::Builder& light_scene_builder,
           SceneObjects scene_objects, const PowerMatcher& power_matcher);

  typedef std::function<bool(std::pair<size_t, size_t>,
                             std::pair<size_t, size_t>)>
      SkipPixelFn;
  typedef std::function<void(size_t, size_t)> ProgressCallbackFn;

  struct AdditionalOptions {
    AdditionalOptions() {}  // Workaround compiler bug
    SkipPixelFn skip_pixel_callback = nullptr;
    ProgressCallbackFn progress_callback = nullptr;
    std::optional<visual_t> maximum_sample_luminance = std::nullopt;
    geometric_t minimum_distance = 0.0;
    unsigned num_threads = 0;
  };

  Framebuffer Render(
      const Camera& camera, const ImageSampler& image_sampler,
      const Integrator& integrator, const AlbedoMatcher& albedo_matcher,
      const ColorMatcher& color_matcher, RandomBitstream& rng,
      const std::pair<size_t, size_t>& image_dimensions,
      const AdditionalOptions& options = AdditionalOptions()) const;

 private:
  std::unique_ptr<SceneObjects> scene_objects_;
  std::unique_ptr<const Scene> scene_;
  std::unique_ptr<const LightScene> light_scene_;
};

}  // namespace iris

#endif  // _IRIS_RENDERER_
