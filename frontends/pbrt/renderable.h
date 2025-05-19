#ifndef _FRONTENDS_PBRT_RENDERABLE_
#define _FRONTENDS_PBRT_RENDERABLE_

#include <functional>
#include <memory>

#include "iris/camera.h"
#include "iris/color_matcher.h"
#include "iris/framebuffer.h"
#include "iris/image_sampler.h"
#include "iris/integrator.h"
#include "iris/light_scene.h"
#include "iris/random.h"
#include "iris/renderer.h"
#include "iris/scene.h"
#include "iris/scene_objects.h"

namespace iris {
namespace pbrt_frontend {

class Renderable {
 public:
  Renderable(Renderer renderer, std::unique_ptr<Camera> camera,
             std::unique_ptr<ImageSampler> image_sampler,
             std::unique_ptr<Integrator> integrator,
             std::unique_ptr<AlbedoMatcher> albedo_matcher,
             std::unique_ptr<ColorMatcher> color_matcher,
             std::pair<size_t, size_t> image_dimensions) noexcept;

  Framebuffer Render(Random& rng, const Renderer::AdditionalOptions& options =
                                      Renderer::AdditionalOptions()) const;

 private:
  Renderer renderer_;
  std::unique_ptr<Camera> camera_;
  std::unique_ptr<ImageSampler> image_sampler_;
  std::unique_ptr<Integrator> integrator_;
  std::unique_ptr<AlbedoMatcher> albedo_matcher_;
  std::unique_ptr<ColorMatcher> color_matcher_;
  std::pair<size_t, size_t> image_dimensions_;
};

}  // namespace pbrt_frontend
}  // namespace iris

#endif  // _FRONTENDS_PBRT_RENDERABLE_
