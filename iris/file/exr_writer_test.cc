#include "iris/file/exr_writer.h"

#include <fstream>
#include <sstream>

#include "googletest/include/gtest/gtest.h"

TEST(ExrWriterTest, Black) {
  iris::Framebuffer framebuffer(std::make_pair(5, 7));

  std::stringstream output;
  iris::file::WriteExr(framebuffer, output);

  std::ifstream reference("iris/file/test_data/black.exr",
                          std::ofstream::binary);
  std::ostringstream reference_string;
  reference_string << reference.rdbuf();

  EXPECT_EQ(reference_string.str(), output.str());
}

TEST(ExrWriterTest, White) {
  iris::Framebuffer framebuffer(std::make_pair(5, 7));
  for (size_t y = 0; y < 5; y++) {
    for (size_t x = 0; x < 7; x++) {
      iris::Color color(1.0, 1.0, 1.0, iris::Color::LINEAR_SRGB);
      framebuffer.Set(y, x, color);
    }
  }

  std::stringstream output;
  iris::file::WriteExr(framebuffer, output);

  std::ifstream reference("iris/file/test_data/white.exr",
                          std::ofstream::binary);
  std::ostringstream reference_string;
  reference_string << reference.rdbuf();

  EXPECT_EQ(reference_string.str(), output.str());
}