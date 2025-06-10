#include "iris/bxdf_allocator.h"

#include <numbers>
#include <span>

#include "googletest/include/gtest/gtest.h"
#include "iris/bxdf.h"
#include "iris/bxdfs/lambertian_bxdf.h"
#include "iris/bxdfs/mock_bxdf.h"
#include "iris/internal/arena.h"
#include "iris/reflectors/mock_reflector.h"
#include "iris/vector.h"

namespace iris {
namespace {

using ::iris::bxdfs::MakeLambertianBrdf;
using ::iris::bxdfs::MockBxdf;
using ::iris::internal::Arena;
using ::iris::reflectors::MockReflector;
using ::testing::ElementsAre;
using ::testing::IsEmpty;

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

TEST(BxdfAllocatorTest, AllocateList) {
  Arena arena;
  BxdfAllocator allocator(arena);

  MockBxdf bxdf0;
  MockBxdf bxdf1;
  EXPECT_THAT(allocator.AllocateList({}), IsEmpty());
  EXPECT_THAT(allocator.AllocateList({nullptr}), IsEmpty());
  EXPECT_THAT(
      allocator.AllocateList({nullptr, &bxdf0, nullptr, &bxdf1, nullptr}),
      ElementsAre(&bxdf0, &bxdf1));
}

}  // namespace
}  // namespace iris
