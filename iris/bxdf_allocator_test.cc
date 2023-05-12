#include "iris/bxdf_allocator.h"

#define _USE_MATH_CONSTANTS
#include <cmath>

#include "googletest/include/gtest/gtest.h"
#include "iris/bxdfs/lambertian_brdf.h"
#include "iris/internal/arena.h"
#include "iris/reflectors/mock_reflector.h"

TEST(BxdfAllocatorTest, Allocate) {
  iris::internal::Arena arena;
  iris::BxdfAllocator allocator(arena);
  iris::reflectors::MockReflector reflector;
  const iris::Bxdf& bxdf =
      allocator.Allocate<iris::bxdfs::LambertianBrdf>(reflector);
  EXPECT_NEAR(
      M_1_PI,
      bxdf.Pdf(iris::Vector(0.0, 0.0, -1.0), iris::Vector(0.0, 0.0, 1.0),
               iris::Bxdf::SampleSource::BXDF)
          .value(),
      0.01);
}