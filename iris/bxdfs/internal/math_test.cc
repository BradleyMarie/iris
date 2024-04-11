#include "iris/bxdfs/internal/math.h"

#define _USE_MATH_CONSTANTS
#include <cmath>

#include "googletest/include/gtest/gtest.h"

TEST(FesnelDielectricReflectance, Refracted) {
  EXPECT_NEAR(0.04,
              iris::bxdfs::internal::FesnelDielectricReflectance(1.0, 1.0, 1.5),
              0.0001);
}

TEST(FesnelDielectricReflectance, TotalInternalReflection) {
  EXPECT_EQ(1.0,
            iris::bxdfs::internal::FesnelDielectricReflectance(0.1, 1.5, 1.0));
}