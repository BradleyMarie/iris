#include "iris/renderer.h"

#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <thread>
#include <vector>

#include "iris/color.h"
#include "iris/float.h"
#include "iris/internal/arena.h"
#include "iris/internal/ray_tracer.h"
#include "iris/internal/visibility_tester.h"
#include "iris/light_sample_allocator.h"
#include "iris/light_sampler.h"
#include "iris/ray_tracer.h"
#include "iris/spectral_allocator.h"

namespace iris {
namespace {

static const size_t kChunkSize = 32;

struct Chunk {
  std::unique_ptr<ImageSampler> image_sampler;
  std::unique_ptr<Random> rng;
  size_t chunk_start_x, chunk_end_x;
};

void RenderChunk(const Scene& scene,
                 const EnvironmentalLight* environmental_light,
                 const LightScene& light_scene, const Camera& camera,
                 std::unique_ptr<Integrator> integrator,
                 const ColorMatcher& color_matcher,
                 std::vector<std::vector<Chunk>>& chunks,
                 std::atomic<size_t>& chunk_counter, size_t num_chunks,
                 geometric_t minimum_distance, Framebuffer& framebuffer,
                 std::function<void(size_t, size_t)> progress_callback) {
  bool has_lens = camera.HasLens();
  auto color_space = color_matcher.ColorSpace();

  internal::Arena arena;
  LightSampleAllocator light_sample_allocator(arena);
  SpectralAllocator spectral_allocator(arena);

  internal::RayTracer internal_tracer;
  internal::VisibilityTester visibility_tester(scene, minimum_distance,
                                               internal_tracer, arena);
  RayTracer ray_tracer(scene, environmental_light, minimum_distance,
                       internal_tracer, arena);

  if (progress_callback) {
    progress_callback(0, num_chunks);
  }

  for (auto chunk_index = chunk_counter.fetch_add(1); chunk_index < num_chunks;
       chunk_index = chunk_counter.fetch_add(1)) {
    if (progress_callback && chunk_index != 0) {
      progress_callback(chunk_index, num_chunks);
    }

    auto y = chunk_index % chunks.size();
    auto& chunk = chunks[y][chunk_index / chunks.size()];

    for (auto x = chunk.chunk_start_x; x < chunk.chunk_end_x; x++) {
      chunk.image_sampler->StartPixel(framebuffer.Size(), std::make_pair(y, x));

      std::array<visual_t, 3> pixel_components = {0.0, 0.0, 0.0};
      for (;;) {
        auto image_sample =
            chunk.image_sampler->NextSample(has_lens, *chunk.rng);
        if (!image_sample) {
          break;
        }

        auto ray =
            camera.Compute(image_sample->image_uv, image_sample->image_uv_dxdy,
                           image_sample->lens_uv);

        LightSampler light_sampler(light_scene, image_sample->rng,
                                   light_sample_allocator);
        auto* spectrum = integrator->Integrate(
            ray, ray_tracer, light_sampler, visibility_tester,
            spectral_allocator, image_sample->rng);

        if (spectrum) {
          auto sample_components = color_matcher.Match(*spectrum);
          pixel_components[0] += sample_components[0] * image_sample->weight;
          pixel_components[1] += sample_components[1] * image_sample->weight;
          pixel_components[2] += sample_components[2] * image_sample->weight;
        }

        arena.Clear();
      }

      framebuffer.Set(
          y, x,
          Color(std::max(static_cast<visual_t>(0.0), pixel_components[0]),
                std::max(static_cast<visual_t>(0.0), pixel_components[1]),
                std::max(static_cast<visual_t>(0.0), pixel_components[2]),
                color_space));
    }
  }

  if (progress_callback) {
    progress_callback(num_chunks, num_chunks);
  }
}

}  // namespace

Framebuffer Renderer::Render(
    const Camera& camera, const ImageSampler& image_sampler,
    const Integrator& integrator, const ColorMatcher& color_matcher,
    Random& rng, std::pair<size_t, size_t> image_dimensions,
    geometric_t minimum_distance, unsigned num_threads,
    std::function<void(size_t, size_t)> progress_callback) const {
  Framebuffer result(image_dimensions);

  size_t num_chunks = 0;
  std::vector<std::vector<Chunk>> chunks;
  for (size_t y = 0; y < image_dimensions.first; y++) {
    chunks.emplace_back();
    for (size_t x = 0; x < image_dimensions.second; x++) {
      if (x % kChunkSize != 0) {
        chunks.back().back().chunk_end_x += 1;
        continue;
      }

      chunks.back().emplace_back(
          Chunk{image_sampler.Replicate(), rng.Replicate(), x, x + 1});
      num_chunks += 1;
    }
  }

  if (num_threads == 0) {
    num_threads = std::thread::hardware_concurrency();
  }

  std::vector<std::thread> threads;
  std::atomic<size_t> chunk_counter = 0;
  for (unsigned i = 1; i < num_threads; i++) {
    threads.push_back(std::thread(
        RenderChunk, std::cref(*scene_),
        scene_objects_->GetEnvironmentalLight(), std::cref(*light_scene_),
        std::cref(camera), integrator.Duplicate(), std::cref(color_matcher),
        std::ref(chunks), std::ref(chunk_counter), num_chunks, minimum_distance,
        std::ref(result), nullptr));
  }

  RenderChunk(*scene_, scene_objects_->GetEnvironmentalLight(), *light_scene_,
              camera, integrator.Duplicate(), color_matcher, chunks,
              chunk_counter, num_chunks, minimum_distance, result,
              progress_callback);

  for (auto& thread : threads) {
    thread.join();
  }

  return result;
}

}  // namespace iris