#include "iris/renderer.h"

#include <iostream>
#include <thread>

#include "googletest/include/gtest/gtest.h"
#include "iris/albedo_matchers/mock_albedo_matcher.h"
#include "iris/cameras/mock_camera.h"
#include "iris/color_matchers/mock_color_matcher.h"
#include "iris/image_samplers/mock_image_sampler.h"
#include "iris/integrators/mock_integrator.h"
#include "iris/light_scenes/all_light_scene.h"
#include "iris/power_matchers/mock_power_matcher.h"
#include "iris/random/mock_random.h"
#include "iris/scenes/list_scene.h"
#include "iris/spectra/mock_spectrum.h"

void RunTestBody(
    unsigned num_threads_requested, unsigned actual_num_threads,
    std::function<void(size_t, size_t)> progress_callback,
    std::function<bool(std::pair<size_t, size_t>, std::pair<size_t, size_t>)>
        skip_pixel_callback,
    std::optional<iris::visual_t> max_sample_luminance) {
  iris::albedo_matchers::MockAlbedoMatcher albedo_matcher;
  iris::power_matchers::MockPowerMatcher power_matcher;
  auto scene_builder = iris::scenes::ListScene::Builder::Create();
  auto light_scene_builder =
      iris::light_scenes::AllLightScene::Builder::Create();
  auto scene_objects = iris::SceneObjects::Builder().Build();
  iris::Renderer renderer(*scene_builder, *light_scene_builder,
                          std::move(scene_objects), power_matcher);

  iris::RayDifferential trace_ray(
      iris::Ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(0.0, 0.0, 1.0)));
  std::pair<size_t, size_t> image_dimensions = std::make_pair(32, 33);

  size_t num_pixels = 0;
  for (size_t y = 0; y < image_dimensions.first; y++) {
    for (size_t x = 0; x < image_dimensions.second; x++) {
      if (!skip_pixel_callback ||
          !skip_pixel_callback({y, x}, image_dimensions)) {
        num_pixels += 1;
      }
    }
  }

  uint32_t samples_per_pixel = 2;
  size_t samples = num_pixels * samples_per_pixel;
  size_t chunks = 64;

  iris::random::MockRandom rng;
  EXPECT_CALL(rng, Replicate())
      .Times(chunks)
      .WillRepeatedly(testing::Invoke(
          []() { return std::make_unique<iris::random::MockRandom>(); }));

  size_t sampler_index = 0;
  iris::image_samplers::MockImageSampler image_sampler;
  EXPECT_CALL(image_sampler, Replicate())
      .Times(chunks)
      .WillRepeatedly(testing::Invoke([&]() {
        auto result =
            std::make_unique<iris::image_samplers::MockImageSampler>();

        if (sampler_index % 2 == 0) {
          {
            testing::InSequence s;

            for (size_t i = 0; i < 32; i++) {
              if (skip_pixel_callback &&
                  skip_pixel_callback({sampler_index / 2, i},
                                      image_dimensions)) {
                continue;
              }

              EXPECT_CALL(*result,
                          StartPixel(std::make_pair(static_cast<size_t>(32),
                                                    static_cast<size_t>(33)),
                                     std::make_pair(
                                         static_cast<size_t>(sampler_index / 2),
                                         static_cast<size_t>(i))));
              EXPECT_CALL(*result, NextSample(testing::_, testing::_))
                  .Times(samples_per_pixel)
                  .WillRepeatedly(testing::Return(iris::ImageSampler::Sample{
                      {0.0, 0.0},
                      {1.0, 1.0},
                      std::nullopt,
                      static_cast<iris::visual_t>(1.0) /
                          static_cast<iris::visual_t>(samples_per_pixel),
                      rng}));
              EXPECT_CALL(*result, NextSample(testing::_, testing::_))
                  .WillOnce(testing::Return(std::nullopt));
            }
          }
        } else {
          if (!skip_pixel_callback ||
              !skip_pixel_callback({sampler_index / 2, 32}, image_dimensions)) {
            EXPECT_CALL(*result,
                        StartPixel(std::make_pair(static_cast<size_t>(32),
                                                  static_cast<size_t>(33)),
                                   std::make_pair(
                                       static_cast<size_t>(sampler_index / 2),
                                       static_cast<size_t>(32))));
            {
              testing::InSequence s;
              EXPECT_CALL(*result, NextSample(testing::_, testing::_))
                  .Times(samples_per_pixel)
                  .WillRepeatedly(testing::Return(iris::ImageSampler::Sample{
                      {0.0, 0.0},
                      {1.0, 1.0},
                      std::nullopt,
                      static_cast<iris::visual_t>(1.0) /
                          static_cast<iris::visual_t>(samples_per_pixel),
                      rng}));
              EXPECT_CALL(*result, NextSample(testing::_, testing::_))
                  .WillOnce(testing::Return(std::nullopt));
            }
          }
        }

        sampler_index += 1;

        return result;
      }));

  iris::cameras::MockCamera camera;
  EXPECT_CALL(camera, HasLens())
      .Times(actual_num_threads)
      .WillRepeatedly(testing::Return(false));

  EXPECT_CALL(camera, Compute(testing::_, testing::_, testing::_))
      .Times(samples)
      .WillRepeatedly(testing::Return(trace_ray));

  iris::spectra::MockSpectrum spectrum;
  iris::integrators::MockIntegrator integrator;
  EXPECT_CALL(integrator, Duplicate())
      .Times(actual_num_threads)
      .WillRepeatedly(testing::Invoke([&]() {
        auto result = std::make_unique<iris::integrators::MockIntegrator>();
        EXPECT_CALL(*result,
                    Integrate(trace_ray, testing::_, testing::_, testing::_,
                              testing::_, testing::_, testing::_))
            .WillRepeatedly(testing::Return(&spectrum));
        return result;
      }));

  iris::Color color(1.0, 1.0, 1.0, iris::Color::CIE_XYZ);
  iris::color_matchers::MockColorMatcher color_matcher;
  EXPECT_CALL(color_matcher, Match(testing::_))
      .Times(samples)
      .WillRepeatedly(
          testing::Return(std::array<iris::visual_t, 3>({1.0, 1.0, 1.0})));
  EXPECT_CALL(color_matcher, ColorSpace())
      .Times(actual_num_threads)
      .WillRepeatedly(testing::Return(iris::Color::CIE_XYZ));

  iris::Renderer::AdditionalOptions options;
  options.num_threads = num_threads_requested;
  options.progress_callback = progress_callback;
  options.skip_pixel_callback = skip_pixel_callback;
  options.maximum_sample_luminance = max_sample_luminance;

  auto framebuffer =
      renderer.Render(camera, image_sampler, integrator, albedo_matcher,
                      color_matcher, rng, image_dimensions, options);
  EXPECT_EQ(image_dimensions, framebuffer.Size());

  iris::Color black(0.0, 0.0, 0.0, iris::Color::CIE_XYZ);
  for (size_t y = 0; y < image_dimensions.first; y++) {
    for (size_t x = 0; x < image_dimensions.second; x++) {
      if (!options.skip_pixel_callback ||
          !options.skip_pixel_callback({y, x}, image_dimensions)) {
        if (max_sample_luminance.has_value() &&
            color.y > *max_sample_luminance) {
          iris::visual_t scale = *max_sample_luminance / color.y;
          EXPECT_NEAR(color.x * scale, framebuffer.Get(y, x).x, 0.001);
          EXPECT_NEAR(color.y * scale, framebuffer.Get(y, x).y, 0.001);
          EXPECT_NEAR(color.z * scale, framebuffer.Get(y, x).z, 0.001);
          EXPECT_EQ(color.space, framebuffer.Get(y, x).space);
        } else {
          EXPECT_EQ(color, framebuffer.Get(y, x));
        }
      } else {
        EXPECT_EQ(black, framebuffer.Get(y, x));
      }
    }
  }
}

TEST(RendererTest, SingleThreaded) {
  RunTestBody(1u, 1u, nullptr, nullptr, std::nullopt);
}

TEST(RendererTest, MultiThreaded) {
  RunTestBody(0u, std::thread::hardware_concurrency(), nullptr, nullptr,
              std::nullopt);
}

TEST(RendererTest, Progress) {
  size_t next_value = 0;
  iris::Renderer::ProgressCallbackFn progress_callback =
      [&](size_t current_pixel, size_t num_pixels) {
        EXPECT_EQ(next_value++, current_pixel);
        EXPECT_EQ(1056u, num_pixels);
      };

  RunTestBody(1u, 1u, progress_callback, nullptr, std::nullopt);
  EXPECT_EQ(1057u, next_value);
}

TEST(RendererTest, WithSkips) {
  iris::Renderer::SkipPixelFn skip_pixel_callback =
      [&](std::pair<size_t, size_t> pixel,
          std::pair<size_t, size_t> image_dimensions) {
        EXPECT_EQ(32u, image_dimensions.first);
        EXPECT_EQ(33u, image_dimensions.second);
        return (pixel.first + pixel.second) % 2 != 0;
      };

  size_t next_value = 0;
  iris::Renderer::ProgressCallbackFn progress_callback =
      [&](size_t current_pixel, size_t num_pixels) {
        EXPECT_EQ(next_value++, current_pixel);
        EXPECT_EQ(528u, num_pixels);
      };

  RunTestBody(1u, 1u, progress_callback, skip_pixel_callback, std::nullopt);
  EXPECT_EQ(529u, next_value);
}

TEST(RendererTest, WithMaxLuma) { RunTestBody(1u, 1u, nullptr, nullptr, 0.5); }