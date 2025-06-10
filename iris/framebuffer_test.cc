#include "iris/framebuffer.h"

#include <utility>

#include "googletest/include/gtest/gtest.h"
#include "iris/color.h"

namespace iris {
namespace {

TEST(FramebufferTest, Create) {
  Framebuffer framebuffer(std::make_pair(2, 3));
  std::pair<size_t, size_t> size = framebuffer.Size();
  EXPECT_EQ(2u, size.first);
  EXPECT_EQ(3u, size.second);
}

TEST(FramebufferTest, Get) {
  Framebuffer framebuffer(std::make_pair(2, 2));
  Color expected_color(0.0, 0.0, 0.0, Color::CIE_XYZ);
  EXPECT_EQ(expected_color, framebuffer.Get(0, 0));
  EXPECT_EQ(expected_color, framebuffer.Get(0, 1));
  EXPECT_EQ(expected_color, framebuffer.Get(1, 0));
  EXPECT_EQ(expected_color, framebuffer.Get(1, 1));
}

TEST(FramebufferTest, Set) {
  Framebuffer framebuffer(std::make_pair(1, 1));
  EXPECT_EQ(Color(0.0, 0.0, 0.0, Color::CIE_XYZ), framebuffer.Get(0, 0));
  framebuffer.Set(0, 0, Color(1.0, 2.0, 3.0, Color::LINEAR_SRGB));
  EXPECT_EQ(Color(1.0, 2.0, 3.0, Color::LINEAR_SRGB), framebuffer.Get(0, 0));
}

}  // namespace
}  // namespace iris
