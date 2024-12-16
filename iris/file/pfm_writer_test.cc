#include "iris/file/pfm_writer.h"

#include <fstream>
#include <sstream>

#include "googletest/include/gtest/gtest.h"
#include "tools/cpp/runfiles/runfiles.h"

using bazel::tools::cpp::runfiles::Runfiles;

std::ifstream RunfilePath(const std::string& path) {
  std::unique_ptr<Runfiles> runfiles(Runfiles::CreateForTest());
  const char* base_path = "_main/iris/file/test_data/";
  return std::ifstream(runfiles->Rlocation(base_path + path),
                       std::ios::in | std::ofstream::binary);
}

TEST(PfmWriterTest, Black) {
  iris::Framebuffer framebuffer(std::make_pair(5, 7));

  std::stringstream output;
  iris::file::WritePfm(framebuffer, iris::Color::CIE_XYZ, output);

  std::ifstream reference = RunfilePath("black.pfm");
  std::ostringstream reference_string;
  reference_string << reference.rdbuf();

  EXPECT_EQ(reference_string.str(), output.str());
}

TEST(PfmWriterTest, White) {
  iris::Framebuffer framebuffer(std::make_pair(5, 7));
  for (size_t y = 0; y < 5; y++) {
    for (size_t x = 0; x < 7; x++) {
      iris::Color color(1.0, 1.0, 1.0, iris::Color::CIE_XYZ);
      framebuffer.Set(y, x, color);
    }
  }

  std::stringstream output;
  iris::file::WritePfm(framebuffer, iris::Color::CIE_XYZ, output);

  std::ifstream reference = RunfilePath("white.pfm");
  std::ostringstream reference_string;
  reference_string << reference.rdbuf();

  EXPECT_EQ(reference_string.str(), output.str());
}