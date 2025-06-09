#include "iris/internal/arena.h"

#include "googletest/include/gtest/gtest.h"

namespace iris {
namespace internal {
namespace {

TEST(Arena, AllocateZero) {
  Arena arena;

  void* alloc0 = arena.Allocate(0);
  EXPECT_EQ(nullptr, alloc0);
}

TEST(Arena, Allocate) {
  Arena arena;

  char& alloc0 = arena.Allocate<char>('c');
  char& alloc1 = arena.Allocate<char>('b');
  EXPECT_EQ('c', alloc0);
  EXPECT_EQ('b', alloc1);
}

TEST(Arena, AllocateAndClear) {
  Arena arena;

  char& alloc0 = arena.Allocate<char>('c');
  EXPECT_EQ('c', alloc0);

  arena.Clear();

  char& alloc1 = arena.Allocate<char>('b');
  EXPECT_EQ('b', alloc1);

  EXPECT_EQ(&alloc0, &alloc1);
}

}  // namespace
}  // namespace internal
}  // namespace iris
