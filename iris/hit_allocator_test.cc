#include "iris/hit_allocator.h"

#include <array>

#include "googletest/include/gtest/gtest.h"
#include "iris/hit.h"
#include "iris/internal/hit_arena.h"
#include "iris/point.h"
#include "iris/ray.h"
#include "iris/vector.h"

namespace iris {
namespace {

TEST(HitAllocator, Allocate) {
  Ray ray(Point(0.0, 0.0, 0.0), Vector(1.0, 1.0, 1.0));

  internal::HitArena arena;
  HitAllocator allocator(ray, arena);

  Hit& hit0 = allocator.Allocate(nullptr, 2.0, 3.0, 1, 2);
  EXPECT_EQ(nullptr, hit0.next);
  EXPECT_EQ(2.0, hit0.distance);

  internal::Hit& full_hit0 = static_cast<internal::Hit&>(hit0);
  EXPECT_EQ(3.0, full_hit0.distance_error);
  EXPECT_EQ(nullptr, full_hit0.geometry);
  EXPECT_EQ(nullptr, full_hit0.model_to_world);
  EXPECT_EQ(1u, full_hit0.front);
  EXPECT_EQ(2u, full_hit0.back);
  EXPECT_EQ(nullptr, full_hit0.additional_data);

  std::array<double, 3> array = {1.0, 2.0, 3.0};
  Hit& hit1 = allocator.Allocate(&hit0, 1.0, -5.0, 3, 4, array);
  EXPECT_EQ(&hit0, hit1.next);
  EXPECT_EQ(1.0, hit1.distance);

  internal::Hit& full_hit1 = static_cast<internal::Hit&>(hit1);
  EXPECT_EQ(5.0, full_hit1.distance_error);
  EXPECT_EQ(nullptr, full_hit1.geometry);
  EXPECT_EQ(nullptr, full_hit1.model_to_world);
  EXPECT_EQ(3u, full_hit1.front);
  EXPECT_EQ(4u, full_hit1.back);
  ASSERT_NE(nullptr, full_hit1.additional_data);
  const std::array<double, 3>* array_copy =
      static_cast<const std::array<double, 3>*>(full_hit1.additional_data);
  EXPECT_EQ(1.0, (*array_copy)[0]);
  EXPECT_EQ(2.0, (*array_copy)[1]);
  EXPECT_EQ(3.0, (*array_copy)[2]);
}

}  // namespace
}  // namespace iris
