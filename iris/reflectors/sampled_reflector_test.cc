#include "iris/reflectors/sampled_reflector.h"

#include "googletest/include/gtest/gtest.h"

TEST(SampledReflector, SingleSample) {
  std::map<iris::visual, iris::visual> samples = {
      {static_cast<iris::visual>(1.0), static_cast<iris::visual>(1.0)}};
  iris::reflectors::SampledReflector reflector(samples);
  EXPECT_EQ(1.0, reflector.Reflectance(0.5));
  EXPECT_EQ(1.0, reflector.Reflectance(1.0));
  EXPECT_EQ(1.0, reflector.Reflectance(1.5));
  EXPECT_EQ(1.0, reflector.Reflectance(2.0));
  EXPECT_EQ(1.0, reflector.Reflectance(2.5));
  EXPECT_EQ(1.0, reflector.Reflectance(3.0));
  EXPECT_EQ(1.0, reflector.Reflectance(3.5));
}

TEST(SampledReflector, Reflectance) {
  std::map<iris::visual, iris::visual> samples = {
      {static_cast<iris::visual>(1.0), static_cast<iris::visual>(0.0)},
      {static_cast<iris::visual>(2.0), static_cast<iris::visual>(1.0)},
      {static_cast<iris::visual>(3.0), static_cast<iris::visual>(0.0)}};
  iris::reflectors::SampledReflector reflector(samples);
  EXPECT_EQ(0.0, reflector.Reflectance(0.5));
  EXPECT_EQ(0.0, reflector.Reflectance(1.0));
  EXPECT_EQ(0.5, reflector.Reflectance(1.5));
  EXPECT_EQ(1.0, reflector.Reflectance(2.0));
  EXPECT_EQ(0.5, reflector.Reflectance(2.5));
  EXPECT_EQ(0.0, reflector.Reflectance(3.0));
  EXPECT_EQ(0.0, reflector.Reflectance(3.5));
}