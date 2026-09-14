#include "iris/environmental_lights/internal/distribution_1d.h"

#include <span>
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

TEST(Distribution1D, SampleContinuous) {
  std::vector<visual> values = {1.0, 2.0, 3.0, 2.0};
  Distribution1D dist(values);

  Sampler sampler0 = MakeSampler({0.125}, {});
  geometric_t value0 = dist.SampleContinuous(sampler0);
  EXPECT_EQ(0.25, value0);

  Sampler sampler1 = MakeSampler({0.0625}, {});
  geometric_t value1 = dist.SampleContinuous(sampler1);
  EXPECT_EQ(0.125, value1);
}

TEST(Distribution1D, SampleContinuousAll) {
  std::vector<visual> values = {1.0, 2.0, 3.0, 2.0};
  Distribution1D dist(values);
  Sampler sampler0 = MakeSampler({0.125}, {});

  visual_t pdf0;
  size_t offset0;
  geometric_t value0 = dist.SampleContinuous(sampler0, &pdf0, &offset0);
  EXPECT_EQ(0.25, value0);
  EXPECT_EQ(1.0, pdf0);
  EXPECT_EQ(1u, offset0);

  Sampler sampler1 = MakeSampler({0.0625}, {});
  visual_t pdf1;
  size_t offset1;
  geometric_t value1 = dist.SampleContinuous(sampler1, &pdf1, &offset1);
  EXPECT_EQ(0.125, value1);
  EXPECT_EQ(0.5, pdf1);
  EXPECT_EQ(0u, offset1);
}

TEST(Distribution1D, PdfContinuous) {
  std::vector<visual> values = {1.0, 2.0, 3.0, 2.0};
  Distribution1D dist(values);

  geometric_t value0 = dist.PdfContinuous(0.25);
  EXPECT_EQ(1.0, value0);

  geometric_t value1 = dist.PdfContinuous(0.125);
  EXPECT_EQ(0.5, value1);
}

TEST(Distribution1D, SampleDiscrete) {
  std::vector<visual> values = {1.0, 2.0, 3.0, 2.0};
  Distribution1D dist(values);

  Sampler sampler0 = MakeSampler({0.125}, {});
  geometric_t value0 = dist.SampleDiscrete(sampler0);
  EXPECT_EQ(1u, value0);

  Sampler sampler1 = MakeSampler({0.0625}, {});
  geometric_t value1 = dist.SampleDiscrete(sampler1);
  EXPECT_EQ(0u, value1);
}

TEST(Distribution1D, SampleDiscreteAll) {
  std::vector<visual> values = {1.0, 2.0, 3.0, 2.0};
  Distribution1D dist(values);

  Sampler sampler0 = MakeSampler({0.125}, {});
  visual_t pdf0;
  geometric_t value0 = dist.SampleDiscrete(sampler0, &pdf0);
  EXPECT_EQ(1u, value0);
  EXPECT_EQ(0.25, pdf0);

  Sampler sampler1 = MakeSampler({0.0625}, {});
  visual_t pdf1;
  geometric_t value1 = dist.SampleDiscrete(sampler1, &pdf1);
  EXPECT_EQ(0u, value1);
  EXPECT_EQ(0.125, pdf1);
}

TEST(Distribution1D, PdfDiscrete) {
  std::vector<visual> values = {1.0, 2.0, 3.0, 2.0};
  Distribution1D dist(values);

  geometric_t value0 = dist.PdfDiscrete(1u);
  EXPECT_EQ(0.25, value0);

  geometric_t value1 = dist.PdfDiscrete(0u);
  EXPECT_EQ(0.125, value1);
}

TEST(Distribution1D, AllZeroSampleContinuous) {
  std::vector<visual> values = {0.0, 0.0, 0.0, 0.0};
  Distribution1D dist(values);

  Sampler sampler0 = MakeSampler({0.25}, {});
  visual_t pdf0;
  size_t offset0;
  geometric_t value0 = dist.SampleContinuous(sampler0, &pdf0, &offset0);
  EXPECT_EQ(0.25, value0);
  EXPECT_EQ(0.0, pdf0);
  EXPECT_EQ(1u, offset0);

  Sampler sampler1 = MakeSampler({0.0625}, {});
  visual_t pdf1;
  size_t offset1;
  geometric_t value1 = dist.SampleContinuous(sampler1, &pdf1, &offset1);
  EXPECT_EQ(0.0625, value1);
  EXPECT_EQ(0.0, pdf1);
  EXPECT_EQ(0u, offset1);
}

TEST(Distribution1D, AllZeroPdfContinuous) {
  std::vector<visual> values = {0.0, 0.0, 0.0, 0.0};
  Distribution1D dist(values);

  geometric_t value0 = dist.PdfContinuous(0.25);
  EXPECT_EQ(0.0, value0);

  geometric_t value1 = dist.PdfContinuous(0.125);
  EXPECT_EQ(0.0, value1);
}

TEST(Distribution1D, AllZeroSampleDiscrete) {
  std::vector<visual> values = {0.0, 0.0, 0.0, 0.0};
  Distribution1D dist(values);

  Sampler sampler0 = MakeSampler({0.25}, {});
  visual_t pdf0;
  geometric_t value0 = dist.SampleDiscrete(sampler0, &pdf0);
  EXPECT_EQ(1u, value0);
  EXPECT_EQ(0.0, pdf0);

  Sampler sampler1 = MakeSampler({0.0625}, {});
  visual_t pdf1;
  geometric_t value1 = dist.SampleDiscrete(sampler1, &pdf1);
  EXPECT_EQ(0u, value1);
  EXPECT_EQ(0.0, pdf1);
}

TEST(Distribution1D, AllZeroPdfDiscrete) {
  std::vector<visual> values = {0.0, 0.0, 0.0, 0.0};
  Distribution1D dist(values);

  geometric_t value0 = dist.PdfDiscrete(1u);
  EXPECT_EQ(0.0, value0);

  geometric_t value1 = dist.PdfDiscrete(0u);
  EXPECT_EQ(0.0, value1);
}

TEST(Distribution1D, SampleContinuousWithZeroes) {
  std::vector<visual> values = {0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 0.0};
  Distribution1D dist(values);

  Sampler sampler0 = MakeSampler({0.0}, {});
  size_t offset;
  geometric_t sample0 = dist.SampleContinuous(sampler0, nullptr, &offset);
  EXPECT_EQ(0.125, sample0);
  EXPECT_EQ(1u, offset);

  Sampler sampler1 = MakeSampler({0.25}, {});
  geometric_t sample1 = dist.SampleContinuous(sampler1, nullptr, &offset);
  EXPECT_EQ(0.375, sample1);
  EXPECT_EQ(3u, offset);

  Sampler sampler2 = MakeSampler({0.9999999}, {});
  geometric_t sample2 = dist.SampleContinuous(sampler2, nullptr, &offset);
  EXPECT_NEAR(0.875, sample2, 0.001);
  EXPECT_EQ(6u, offset);
}

TEST(Distribution1D, SampleDiscreteWithZeroes) {
  std::vector<visual> values = {0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 0.0};
  Distribution1D dist(values);

  Sampler sampler0 = MakeSampler({0.0}, {});
  size_t sample0 = dist.SampleDiscrete(sampler0);
  EXPECT_EQ(1u, sample0);

  Sampler sampler1 = MakeSampler({0.25}, {});
  size_t sample1 = dist.SampleDiscrete(sampler1);
  EXPECT_EQ(3u, sample1);

  Sampler sampler2 = MakeSampler({0.9999999}, {});
  size_t sample2 = dist.SampleDiscrete(sampler2);
  EXPECT_EQ(6u, sample2);
}

}  // namespace
}  // namespace internal
}  // namespace environmental_lights
}  // namespace iris
