#include "iris/renderer.h"

#include <thread>

#include "googletest/include/gtest/gtest.h"
#include "iris/cameras/mock_camera.h"
#include "iris/color_matchers/mock_color_matcher.h"
#include "iris/image_samplers/mock_image_sampler.h"
#include "iris/integrators/mock_integrator.h"
#include "iris/light_scenes/all_light_scene.h"
#include "iris/random/mock_random.h"
#include "iris/scenes/list_scene.h"
#include "iris/spectra/mock_spectrum.h"

void RunTestBody(unsigned num_threads_requested, unsigned actual_num_threads,
                 std::function<void(size_t, size_t)> progress_callback) {
  auto scene_builder = iris::scenes::ListScene::Builder::Create();
  auto light_scene_builder =
      iris::light_scenes::AllLightScene::Builder::Create();
  auto scene_objects = iris::SceneObjects::Builder().Build();
  iris::Renderer renderer(*scene_builder, *light_scene_builder,
                          std::move(scene_objects));

  iris::RayDifferential trace_ray(
      iris::Ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(0.0, 0.0, 1.0)));
  std::pair<size_t, size_t> image_dimensions = std::make_pair(32, 33);
  uint32_t samples_per_pixel = 2;
  size_t pixels = image_dimensions.first * image_dimensions.second;
  size_t samples = pixels * samples_per_pixel;
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
          EXPECT_CALL(
              *result,
              StartPixel(std::make_pair(static_cast<size_t>(32),
                                        static_cast<size_t>(33)),
                         std::make_pair(static_cast<size_t>(sampler_index / 2),
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
        EXPECT_CALL(*result, Integrate(trace_ray, testing::_, testing::_,
                                       testing::_, testing::_, testing::_))
            .WillRepeatedly(testing::Return(&spectrum));
        return result;
      }));

  iris::Color color(1.0, 1.0, 1.0, iris::Color::LINEAR_SRGB);
  iris::color_matchers::MockColorMatcher color_matcher;
  EXPECT_CALL(color_matcher, Match(testing::_))
      .Times(samples)
      .WillRepeatedly(
          testing::Return(std::array<iris::visual_t, 3>({1.0, 1.0, 1.0})));
  EXPECT_CALL(color_matcher, ColorSpace())
      .Times(actual_num_threads)
      .WillRepeatedly(testing::Return(iris::Color::LINEAR_SRGB));

  auto framebuffer = renderer.Render(camera, image_sampler, integrator,
                                     color_matcher, rng, image_dimensions, 0.0,
                                     num_threads_requested, progress_callback);
  EXPECT_EQ(image_dimensions, framebuffer.Size());

  for (size_t y = 0; y < image_dimensions.first; y++) {
    for (size_t x = 0; x < image_dimensions.second; x++) {
      EXPECT_EQ(color, framebuffer.Get(y, x));
    }
  }
}

TEST(RendererTest, SingleThreaded) { RunTestBody(1u, 1u, nullptr); }

TEST(RendererTest, MultiThreaded) {
  RunTestBody(0u, std::thread::hardware_concurrency(), nullptr);
}

TEST(RendererTest, Progress) {
  size_t next_value = 0;
  auto progress_callback = [&](size_t current_chunk, size_t num_chunks) {
    EXPECT_EQ(next_value++, current_chunk);
    EXPECT_EQ(64u, num_chunks);
  };

  RunTestBody(1u, 1u, progress_callback);
  EXPECT_EQ(65u, next_value);
}