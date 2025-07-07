#include "iris/renderer.h"

#include <algorithm>
#include <array>
#include <atomic>
#include <cassert>
#include <cmath>
#include <functional>
#include <memory>
#include <optional>
#include <thread>
#include <utility>
#include <vector>

#include "iris/albedo_matcher.h"
#include "iris/camera.h"
#include "iris/color.h"
#include "iris/color_matcher.h"
#include "iris/environmental_light.h"
#include "iris/float.h"
#include "iris/framebuffer.h"
#include "iris/image_sampler.h"
#include "iris/integrator.h"
#include "iris/internal/arena.h"
#include "iris/internal/ray_tracer.h"
#include "iris/internal/visibility_tester.h"
#include "iris/light_sample_allocator.h"
#include "iris/light_sampler.h"
#include "iris/light_scene.h"
#include "iris/power_matcher.h"
#include "iris/random.h"
#include "iris/ray_differential.h"
#include "iris/ray_tracer.h"
#include "iris/scene.h"
#include "iris/scene_objects.h"
#include "iris/spectral_allocator.h"
#include "iris/spectrum.h"

namespace iris {
namespace {

static const size_t kChunkSize = 32;

struct Chunk {
  std::unique_ptr<ImageSampler> image_sampler;
  std::unique_ptr<Random> rng;
  size_t chunk_start_x, chunk_end_x;
  size_t pixel_start;
};

void RenderChunk(
    const Scene& scene, const EnvironmentalLight* environmental_light,
    const LightScene& light_scene, const Camera& camera,
    std::unique_ptr<Integrator> integrator, const AlbedoMatcher& albedo_matcher,
    const ColorMatcher& color_matcher, std::vector<std::vector<Chunk>>& chunks,
    std::atomic<size_t>& chunk_counter, size_t num_chunks, size_t num_pixels,
    std::optional<visual_t> maximum_sample_luminance,
    geometric_t minimum_distance, Framebuffer& framebuffer,
    const Renderer::SkipPixelFn& skip_pixel_callback,
    const Renderer::ProgressCallbackFn& progress_callback) {
  bool has_lens = camera.HasLens();
  Color::Space color_space = color_matcher.ColorSpace();

  internal::Arena arena;
  LightSampleAllocator light_sample_allocator(arena);
  SpectralAllocator spectral_allocator(arena);

  internal::RayTracer internal_tracer;
  internal::VisibilityTester visibility_tester(scene, minimum_distance,
                                               internal_tracer, arena);
  RayTracer ray_tracer(scene, environmental_light, minimum_distance,
                       internal_tracer, arena);

  std::pair<size_t, size_t> image_dimensions = framebuffer.Size();

  if (progress_callback) {
    progress_callback(0, num_pixels);
  }

  if (maximum_sample_luminance.has_value() &&
      *maximum_sample_luminance <= static_cast<visual_t>(0.0)) {
    *maximum_sample_luminance = static_cast<visual_t>(0.0);
  }

  for (size_t chunk_index = chunk_counter.fetch_add(1);
       chunk_index < num_chunks; chunk_index = chunk_counter.fetch_add(1)) {
    size_t y = chunk_index % chunks.size();
    Chunk& chunk = chunks[y][chunk_index / chunks.size()];

    size_t pixel_index = 0;
    for (size_t x = chunk.chunk_start_x; x < chunk.chunk_end_x; x++) {
      std::array<visual_t, 3> pixel_components = {0.0, 0.0, 0.0};
      if (!skip_pixel_callback ||
          !skip_pixel_callback({y, x}, image_dimensions)) {
        if (progress_callback && (x != 0 || y != 0)) {
          progress_callback(chunk.pixel_start + pixel_index, num_pixels);
        }

        pixel_index += 1;

        chunk.image_sampler->StartPixel(framebuffer.Size(),
                                        std::make_pair(y, x), *chunk.rng);

        for (;;) {
          std::optional<ImageSampler::Sample> image_sample =
              chunk.image_sampler->NextSample(has_lens, *chunk.rng);
          if (!image_sample) {
            break;
          }

          RayDifferential ray = camera.Compute(image_sample->image_uv,
                                               image_sample->image_uv_dxdy,
                                               image_sample->lens_uv);

          LightSampler light_sampler(light_scene, image_sample->rng,
                                     light_sample_allocator);
          const Spectrum* spectrum = integrator->Integrate(
              ray, ray_tracer, light_sampler, visibility_tester, albedo_matcher,
              spectral_allocator, image_sample->rng);

          if (spectrum) {
            std::array<visual_t, 3UL> sample_components =
                color_matcher.Match(*spectrum);
            assert(std::isfinite(sample_components[0]) &&
                   sample_components[0] >= static_cast<visual_t>(0.0));
            assert(std::isfinite(sample_components[1]) &&
                   sample_components[0] >= static_cast<visual_t>(0.0));
            assert(std::isfinite(sample_components[2]) &&
                   sample_components[0] >= static_cast<visual_t>(0.0));

            if (std::isfinite(sample_components[0]) &&
                std::isfinite(sample_components[0]) &&
                std::isfinite(sample_components[0])) {
              sample_components[0] =
                  std::max(static_cast<visual_t>(0.0), sample_components[0]);
              sample_components[1] =
                  std::max(static_cast<visual_t>(0.0), sample_components[1]);
              sample_components[2] =
                  std::max(static_cast<visual_t>(0.0), sample_components[2]);

              if (maximum_sample_luminance.has_value()) {
                Color color(sample_components[0], sample_components[1],
                            sample_components[2], color_space);

                if (Color xyz = color.ConvertTo(Color::CIE_XYZ);
                    xyz.y > static_cast<visual_t>(0.0) &&
                    xyz.y > *maximum_sample_luminance) {
                  visual_t scale = *maximum_sample_luminance / xyz.y;
                  sample_components[0] *= scale;
                  sample_components[1] *= scale;
                  sample_components[2] *= scale;
                }
              }

              pixel_components[0] +=
                  sample_components[0] * image_sample->weight;
              pixel_components[1] +=
                  sample_components[1] * image_sample->weight;
              pixel_components[2] +=
                  sample_components[2] * image_sample->weight;
            }
          }

          arena.Clear();
        }
      }

      framebuffer.Set(y, x,
                      Color(pixel_components[0], pixel_components[1],
                            pixel_components[2], color_space));
    }
  }

  if (progress_callback) {
    progress_callback(num_pixels, num_pixels);
  }
}

}  // namespace

Renderer::Renderer(const Scene::Builder& scene_builder,
                   const LightScene::Builder& light_scene_builder,
                   SceneObjects scene_objects,
                   const PowerMatcher& power_matcher)
    : scene_objects_(std::make_unique<SceneObjects>(std::move(scene_objects))),
      scene_(scene_builder.Build(*scene_objects_)),
      light_scene_(light_scene_builder.Build(*scene_objects_, power_matcher)) {}

Framebuffer Renderer::Render(const Camera& camera,
                             const ImageSampler& image_sampler,
                             const Integrator& integrator,
                             const AlbedoMatcher& albedo_matcher,
                             const ColorMatcher& color_matcher, Random& rng,
                             const std::pair<size_t, size_t>& image_dimensions,
                             const AdditionalOptions& options) const {
  Framebuffer result(image_dimensions);

  size_t num_x_chunks = image_dimensions.second / kChunkSize + 1u;
  size_t num_chunks = image_dimensions.first * num_x_chunks;

  std::vector<std::vector<Chunk>> chunks;
  for (size_t y = 0; y < image_dimensions.first; y++) {
    chunks.emplace_back();
    for (size_t x = 0; x < num_x_chunks; x++) {
      Chunk& chunk = chunks.back().emplace_back();
      chunk.image_sampler = image_sampler.Replicate();
      chunk.rng = rng.Replicate();
    }
  }

  size_t num_pixels = 0;
  for (size_t x_chunk = 0; x_chunk < num_x_chunks; x_chunk++) {
    for (size_t y = 0; y < image_dimensions.first; y++) {
      Chunk& chunk = chunks.at(y).at(x_chunk);

      for (size_t x_offset = 0; x_offset < kChunkSize; x_offset++) {
        size_t x = x_chunk * kChunkSize + x_offset;
        if (x >= image_dimensions.second) {
          break;
        }

        chunk.chunk_start_x = x - x % kChunkSize;
        chunk.chunk_end_x = x + 1;

        if (x % kChunkSize == 0) {
          chunk.pixel_start = num_pixels;
        }

        if (!options.skip_pixel_callback ||
            !options.skip_pixel_callback({y, x}, image_dimensions)) {
          num_pixels += 1;
        }
      }
    }
  }

  unsigned num_threads = (options.num_threads == 0)
                             ? std::thread::hardware_concurrency()
                             : options.num_threads;

  std::vector<std::thread> threads;
  std::atomic<size_t> chunk_counter = 0;
  for (unsigned i = 1; i < num_threads; i++) {
    threads.push_back(std::thread(
        RenderChunk, std::cref(*scene_),
        scene_objects_->GetEnvironmentalLight(), std::cref(*light_scene_),
        std::cref(camera), integrator.Duplicate(), std::cref(albedo_matcher),
        std::cref(color_matcher), std::ref(chunks), std::ref(chunk_counter),
        num_chunks, num_pixels, options.maximum_sample_luminance,
        options.minimum_distance, std::ref(result), options.skip_pixel_callback,
        nullptr));
  }

  RenderChunk(*scene_, scene_objects_->GetEnvironmentalLight(), *light_scene_,
              camera, integrator.Duplicate(), albedo_matcher, color_matcher,
              chunks, chunk_counter, num_chunks, num_pixels,
              options.maximum_sample_luminance, options.minimum_distance,
              result, options.skip_pixel_callback, options.progress_callback);

  for (std::thread& thread : threads) {
    thread.join();
  }

  return result;
}

}  // namespace iris
