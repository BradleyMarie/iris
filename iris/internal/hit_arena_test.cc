#include "iris/internal/hit_arena.h"

#include <cstdint>

#include "googletest/include/gtest/gtest.h"
#include "iris/geometry/mock_geometry.h"
#include "iris/internal/hit.h"

namespace iris {
namespace internal {
namespace {

using ::iris::geometry::MockBasicGeometry;

TEST(HitArena, AllocateAndClear) {
  HitArena arena;

  Hit& hit0 = arena.Allocate(nullptr, 1.0, 3.0, 1, 2, true, nullptr, 0);
  EXPECT_EQ(nullptr, hit0.next);
  EXPECT_EQ(1.0, hit0.distance);
  EXPECT_EQ(3.0, hit0.error);
  EXPECT_EQ(nullptr, hit0.geometry);
  EXPECT_EQ(nullptr, hit0.model_to_world);
  EXPECT_EQ(1u, hit0.front);
  EXPECT_EQ(2u, hit0.back);
  EXPECT_TRUE(hit0.is_chiral);
  EXPECT_TRUE(hit0.allow_emissive);
  EXPECT_EQ(nullptr, hit0.additional_data);

  uint64_t value[3] = {5, 6, 1};
  Hit& hit1 =
      arena.Allocate(&hit0, 2.0, 5.0, 3, 4, false, value, sizeof(value));
  EXPECT_EQ(&hit0, hit1.next);
  EXPECT_EQ(2.0, hit1.distance);
  EXPECT_EQ(5.0, hit1.error);
  EXPECT_EQ(nullptr, hit1.geometry);
  EXPECT_EQ(nullptr, hit1.model_to_world);
  EXPECT_EQ(3u, hit1.front);
  EXPECT_EQ(4u, hit1.back);
  EXPECT_FALSE(hit1.is_chiral);
  EXPECT_TRUE(hit1.allow_emissive);
  ASSERT_NE(nullptr, hit1.additional_data);
  const uint64_t* copy = static_cast<const uint64_t*>(hit1.additional_data);
  EXPECT_EQ(5u, copy[0]);
  EXPECT_EQ(6u, copy[1]);
  EXPECT_EQ(1u, copy[2]);

  arena.Clear();

  uint64_t maxval = 0xFFFFFFFFu;
  Hit& hit2 =
      arena.Allocate(nullptr, 5.0, -9.0, 6, 7, false, &maxval, sizeof(maxval));
  EXPECT_EQ(nullptr, hit2.next);
  EXPECT_EQ(5.0, hit2.distance);
  EXPECT_EQ(9.0, hit2.error);
  EXPECT_EQ(nullptr, hit2.geometry);
  EXPECT_EQ(nullptr, hit2.model_to_world);
  EXPECT_EQ(6u, hit2.front);
  EXPECT_EQ(7u, hit2.back);
  EXPECT_FALSE(hit2.is_chiral);
  EXPECT_TRUE(hit2.allow_emissive);
  const uint64_t* copy2 = static_cast<const uint64_t*>(hit2.additional_data);
  EXPECT_EQ(0xFFFFFFFFu, copy2[0]);
}

TEST(HitArena, SetGeometry) {
  HitArena arena;
  EXPECT_EQ(nullptr, arena.GetGeometry());

  MockBasicGeometry geometry0;
  arena.SetGeometry(&geometry0);
  EXPECT_EQ(&geometry0, arena.GetGeometry());

  Hit& hit0 = arena.Allocate(nullptr, 1.0, 3.0, 1, 2, false, nullptr, 0);
  EXPECT_EQ(&geometry0, hit0.geometry);
  EXPECT_TRUE(hit0.allow_emissive);

  MockBasicGeometry geometry1;
  arena.SetGeometry(&geometry1);
  EXPECT_EQ(&geometry1, arena.GetGeometry());

  Hit& hit1 = arena.Allocate(nullptr, 1.0, 3.0, 1, 2, false, nullptr, 0);
  EXPECT_EQ(&geometry1, hit1.geometry);
  EXPECT_FALSE(hit1.allow_emissive);

  arena.SetGeometry(&geometry0);
  EXPECT_EQ(&geometry0, arena.GetGeometry());

  Hit& hit2 = arena.Allocate(nullptr, 1.0, 3.0, 1, 2, false, nullptr, 0);
  EXPECT_EQ(&geometry0, hit2.geometry);
  EXPECT_TRUE(hit2.allow_emissive);

  arena.SetGeometry(nullptr);
  EXPECT_EQ(nullptr, arena.GetGeometry());

  Hit& hit3 = arena.Allocate(nullptr, 1.0, 3.0, 1, 2, false, nullptr, 0);
  EXPECT_EQ(nullptr, hit3.geometry);
  EXPECT_TRUE(hit3.allow_emissive);

  arena.SetGeometry(&geometry1);
  EXPECT_EQ(&geometry1, arena.GetGeometry());

  Hit& hit4 = arena.Allocate(nullptr, 1.0, 3.0, 1, 2, false, nullptr, 0);
  EXPECT_EQ(&geometry1, hit4.geometry);
  EXPECT_TRUE(hit4.allow_emissive);
}

}  // namespace
}  // namespace internal
}  // namespace iris
