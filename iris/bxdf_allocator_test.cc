#include "iris/bxdf_allocator.h"

#include <numbers>

#include "googletest/include/gtest/gtest.h"
#include "iris/bxdfs/lambertian_bxdf.h"
#include "iris/internal/arena.h"
#include "iris/reflectors/mock_reflector.h"

namespace iris {
namespace {

using ::iris::bxdfs::MakeLambertianBrdf;
using ::iris::internal::Arena;
using ::iris::reflectors::MockReflector;

TEST(BxdfAllocatorTest, Allocate) {
  Arena arena;
  BxdfAllocator allocator(arena);
  MockReflector reflector;
  const Bxdf* bxdf = MakeLambertianBrdf(allocator, &reflector);
  ASSERT_TRUE(bxdf);
  EXPECT_NEAR(std::numbers::inv_pi,
              bxdf->PdfDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0),
                               Vector(0.0, 0.0, 1.0), Bxdf::Hemisphere::BRDF),
              0.01);
}

}  // namespace
}  // namespace iris