#include "iris/light_sample_allocator.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/float.h"
#include "iris/internal/arena.h"
#include "iris/light_sample.h"
#include "iris/lights/mock_light.h"

namespace iris {
namespace {

using ::iris::lights::MockLight;

TEST(LightSampleAllocatorTest, Allocate) {
  internal::Arena arena;
  LightSampleAllocator allocator(arena);

  MockLight light0;
  LightSample& light_sample0 =
      allocator.Allocate(light0, static_cast<visual_t>(0.5));
  EXPECT_EQ(&light0, &light_sample0.light);
  EXPECT_EQ(0.5, *light_sample0.pdf);
  EXPECT_EQ(nullptr, light_sample0.next);

  MockLight light1;
  LightSample& light_sample1 =
      allocator.Allocate(light1, std::nullopt, &light_sample0);
  EXPECT_EQ(&light1, &light_sample1.light);
  EXPECT_FALSE(light_sample1.pdf);
  EXPECT_EQ(&light_sample0, light_sample1.next);
}

}  // namespace
}  // namespace iris
