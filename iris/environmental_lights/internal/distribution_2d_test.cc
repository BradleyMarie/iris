#include "iris/environmental_lights/internal/distribution_2d.h"

#include <span>
#include <utility>
#include <vector>

#include "googletest/include/gtest/gtest.h"
#include "iris/float.h"
#include "iris/sampler.h"
#include "iris/testing/sampler.h"

namespace iris {
namespace environmental_lights {
namespace internal {
namespace {

using ::iris::testing::MakeSampler;

TEST(Distribution2D, Sample) {
  std::vector<visual> values = {1.0, 1.0, 3.0, 3.0};
  Distribution2D dist(values, {2, 2});

  Sampler sampler0 = MakeSampler({0.125, 0.5}, {});
  auto [u0, v0] = dist.Sample(sampler0);
  EXPECT_EQ(0.5, u0);
  EXPECT_EQ(0.25, v0);

  Sampler sampler1 = MakeSampler({0.25, 0.75}, {});
  auto [u1, v1] = dist.Sample(sampler1);
  EXPECT_EQ(0.75, u1);
  EXPECT_EQ(0.5, v1);
}

TEST(Distribution2D, SampleAll) {
  std::vector<visual> values = {1.0, 1.0, 3.0, 3.0};
  Distribution2D dist(values, {2, 2});

  Sampler sampler0 = MakeSampler({0.125, 0.5}, {});
  visual_t pdf;
  size_t offset;
  auto [u0, v0] = dist.Sample(sampler0, &pdf, &offset);
  EXPECT_EQ(0.5, u0);
  EXPECT_EQ(0.25, v0);
  EXPECT_EQ(0.5, pdf);
  EXPECT_EQ(1u, offset);

  Sampler sampler1 = MakeSampler({0.25, 0.75}, {});
  auto [u1, v1] = dist.Sample(sampler1, &pdf, &offset);
  EXPECT_EQ(0.75, u1);
  EXPECT_EQ(0.5, v1);
  EXPECT_EQ(1.5, pdf);
  EXPECT_EQ(3u, offset);
}

TEST(Distribution2D, Pdf) {
  std::vector<visual> values = {1.0, 1.0, 3.0, 3.0};
  Distribution2D dist(values, {2, 2});

  EXPECT_EQ(0.5, dist.Pdf(0.5, 0.25));
  EXPECT_EQ(1.5, dist.Pdf(0.75, 0.5));
}

}  // namespace
}  // namespace internal
}  // namespace environmental_lights
}  // namespace iris
