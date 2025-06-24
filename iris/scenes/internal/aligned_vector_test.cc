#include "iris/scenes/internal/aligned_vector.h"

#include "googletest/include/gtest/gtest.h"

namespace iris {
namespace scenes {
namespace internal {
namespace {

TEST(AlignedVector, NestedGeometry) {
  AlignedVector<int> vector = MakeAlignedVector<int>(false);
  for (int i = 0; i < 1024 * 1024; i++) {
    vector.push_back(i);
  }

  for (int i = 0; i < 1024 * 1024; i++) {
    EXPECT_EQ(vector[i], i);
  }
}

TEST(AlignedVector, ForScene) {
  AlignedVector<int> vector = MakeAlignedVector<int>(true);
  for (int i = 0; i < 1024 * 1024; i++) {
    vector.push_back(i);
  }

  for (int i = 0; i < 1024 * 1024; i++) {
    EXPECT_EQ(vector[i], i);
  }
}

}  // namespace
}  // namespace internal
}  // namespace scenes
}  // namespace iris
