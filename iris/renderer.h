#ifndef _IRIS_RENDERER_
#define _IRIS_RENDERER_

#include <functional>
#include <memory>
#include <utility>

#include "iris/camera.h"
#include "iris/color_matcher.h"
#include "iris/framebuffer.h"
#include "iris/image_sampler.h"
#include "iris/integrator.h"
#include "iris/light_scene.h"
#include "iris/scene.h"
#include "iris/scene_objects.h"

namespace iris {

class Renderer {
 public:
  Renderer(const Scene::Builder& scene_builder,
           const LightScene::Builder& light_scene_builder,
           SceneObjects scene_objects)
      : scene_objects_(
            std::make_unique<SceneObjects>(std::move(scene_objects))),
        scene_(scene_builder.Build(*scene_objects_)),
        light_scene_(light_scene_builder.Build(*scene_objects_)) {}

  struct AdditionalOptions {
    std::function<bool(std::pair<size_t, size_t>, std::pair<size_t, size_t>)>
        skip_pixel_callback = nullptr;
    std::function<void(size_t, size_t)> progress_callback = nullptr;
    geometric_t minimum_distance = 0.0;
    unsigned num_threads = 0;

    // This constructor is required in order to work around a bug in GCC and
    // Clang
    AdditionalOptions() {}
  };

  Framebuffer Render(
      const Camera& camera, const ImageSampler& image_sampler,
      const Integrator& integrator, const ColorMatcher& color_matcher,
      Random& rng, const std::pair<size_t, size_t>& image_dimensions,
      const AdditionalOptions& options = AdditionalOptions()) const;

 private:
  std::unique_ptr<SceneObjects> scene_objects_;
  std::unique_ptr<const Scene> scene_;
  std::unique_ptr<const LightScene> light_scene_;
};

}  // namespace iris

#endif  // _IRIS_RENDERER_