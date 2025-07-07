#include "frontends/pbrt/renderable.h"

#include <cassert>
#include <functional>
#include <memory>

#include "iris/camera.h"
#include "iris/color_matcher.h"
#include "iris/framebuffer.h"
#include "iris/image_sampler.h"
#include "iris/integrator.h"
#include "iris/light_scene.h"
#include "iris/random_bitstream.h"
#include "iris/renderer.h"
#include "iris/scene.h"
#include "iris/scene_objects.h"

namespace iris {
namespace pbrt_frontend {

Renderable::Renderable(Renderer renderer, std::unique_ptr<Camera> camera,
                       std::unique_ptr<ImageSampler> image_sampler,
                       std::unique_ptr<Integrator> integrator,
                       std::unique_ptr<AlbedoMatcher> albedo_matcher,
                       std::unique_ptr<ColorMatcher> color_matcher,
                       std::pair<size_t, size_t> image_dimensions) noexcept
    : renderer_(std::move(renderer)),
      camera_(std::move(camera)),
      image_sampler_(std::move(image_sampler)),
      integrator_(std::move(integrator)),
      albedo_matcher_(std::move(albedo_matcher)),
      color_matcher_(std::move(color_matcher)),
      image_dimensions_(image_dimensions) {
  assert(camera_);
  assert(image_sampler_);
  assert(integrator_);
  assert(albedo_matcher_);
  assert(color_matcher_);
  assert(image_dimensions.first != 0 && image_dimensions.second != 0);
}

Framebuffer Renderable::Render(
    RandomBitstream& rng, const Renderer::AdditionalOptions& options) const {
  return renderer_.Render(*camera_, *image_sampler_, *integrator_,
                          *albedo_matcher_, *color_matcher_, rng,
                          image_dimensions_, options);
}

}  // namespace pbrt_frontend
}  // namespace iris
