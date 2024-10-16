#include "iris/bxdf_allocator.h"

#include <numbers>

#include "googletest/include/gtest/gtest.h"
#include "iris/bxdfs/lambertian_bxdf.h"
#include "iris/internal/arena.h"
#include "iris/reflectors/mock_reflector.h"

TEST(BxdfAllocatorTest, Allocate) {
  iris::internal::Arena arena;
  iris::BxdfAllocator allocator(arena);
  iris::reflectors::MockReflector reflector;
  const iris::Bxdf& bxdf =
      allocator.Allocate<iris::bxdfs::LambertianBrdf>(reflector);
  EXPECT_NEAR(
      std::numbers::inv_pi,
      bxdf.Pdf(iris::Vector(0.0, 0.0, 1.0), iris::Vector(0.0, 0.0, 1.0),
               iris::Vector(0.0, 0.0, 1.0), iris::Bxdf::Hemisphere::BRDF),
      0.01);
}