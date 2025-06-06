#include "iris/reflectors/sampled_reflector.h"

#include <limits>
#include <map>

#include "googletest/include/gtest/gtest.h"
#include "iris/float.h"
#include "iris/reference_counted.h"
#include "iris/reflector.h"

namespace iris {
namespace reflectors {
namespace {

TEST(SampledReflector, NoSamples) {
  std::map<visual, visual> samples;
  ReferenceCounted<Reflector> reflector = CreateSampledReflector(samples);
  EXPECT_FALSE(reflector);
}

TEST(SampledReflector, Black) {
  std::map<visual, visual> samples = {
      {static_cast<visual>(1.0), static_cast<visual>(0.0)}};
  ReferenceCounted<Reflector> reflector = CreateSampledReflector(samples);
  EXPECT_FALSE(reflector);
}

TEST(SampledReflector, NegativeIsBlack) {
  std::map<visual, visual> samples = {
      {static_cast<visual>(1.0), static_cast<visual>(-1.0)}};
  ReferenceCounted<Reflector> reflector = CreateSampledReflector(samples);
  EXPECT_FALSE(reflector);
}

TEST(SampledReflector, IgnoresInfinite) {
  std::map<visual, visual> samples = {
      {std::numeric_limits<visual>::infinity(), static_cast<visual>(1.0)},
      {-std::numeric_limits<visual>::infinity(), static_cast<visual>(1.0)}};
  ReferenceCounted<Reflector> reflector = CreateSampledReflector(samples);
  EXPECT_FALSE(reflector);
}

TEST(SampledReflector, SingleSample) {
  std::map<visual, visual> samples = {
      {static_cast<visual>(1.0), static_cast<visual>(1.0)}};
  ReferenceCounted<Reflector> reflector = CreateSampledReflector(samples);
  ASSERT_TRUE(reflector);
  EXPECT_EQ(1.0, reflector->Reflectance(0.5));
  EXPECT_EQ(1.0, reflector->Reflectance(1.0));
  EXPECT_EQ(1.0, reflector->Reflectance(1.5));
  EXPECT_EQ(1.0, reflector->Reflectance(2.0));
  EXPECT_EQ(1.0, reflector->Reflectance(2.5));
  EXPECT_EQ(1.0, reflector->Reflectance(3.0));
  EXPECT_EQ(1.0, reflector->Reflectance(3.5));
}

TEST(SampledReflector, SingleLargeSample) {
  std::map<visual, visual> samples = {
      {static_cast<visual>(1.0), static_cast<visual>(2.0)}};
  ReferenceCounted<Reflector> reflector = CreateSampledReflector(samples);
  ASSERT_TRUE(reflector);
  EXPECT_EQ(1.0, reflector->Reflectance(0.5));
  EXPECT_EQ(1.0, reflector->Reflectance(1.0));
  EXPECT_EQ(1.0, reflector->Reflectance(1.5));
  EXPECT_EQ(1.0, reflector->Reflectance(2.0));
  EXPECT_EQ(1.0, reflector->Reflectance(2.5));
  EXPECT_EQ(1.0, reflector->Reflectance(3.0));
  EXPECT_EQ(1.0, reflector->Reflectance(3.5));
}

TEST(SampledReflector, Reflectance) {
  std::map<visual, visual> samples = {
      {static_cast<visual>(1.0), static_cast<visual>(0.0)},
      {static_cast<visual>(2.0), static_cast<visual>(1.0)},
      {static_cast<visual>(3.0), static_cast<visual>(0.0)}};
  ReferenceCounted<Reflector> reflector = CreateSampledReflector(samples);
  ASSERT_TRUE(reflector);
  EXPECT_EQ(0.0, reflector->Reflectance(0.5));
  EXPECT_EQ(0.0, reflector->Reflectance(1.0));
  EXPECT_EQ(0.5, reflector->Reflectance(1.5));
  EXPECT_EQ(1.0, reflector->Reflectance(2.0));
  EXPECT_EQ(0.5, reflector->Reflectance(2.5));
  EXPECT_EQ(0.0, reflector->Reflectance(3.0));
  EXPECT_EQ(0.0, reflector->Reflectance(3.5));
}

}  // namespace
}  // namespace reflectors
}  // namespace iris
