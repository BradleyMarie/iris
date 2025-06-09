#include "frontends/pbrt/file.h"

#include <cstdlib>
#include <filesystem>
#include <string>

#include "googletest/include/gtest/gtest.h"
#include "pbrt_proto/v3/pbrt.pb.h"
#include "tools/cpp/runfiles/runfiles.h"

namespace iris {
namespace pbrt_frontend {
namespace {

using ::bazel::tools::cpp::runfiles::Runfiles;
using ::pbrt_proto::v3::PbrtProto;
using ::testing::ExitedWithCode;

std::string RawRunfilePath(const std::string& path) {
  std::unique_ptr<Runfiles> runfiles(Runfiles::CreateForTest());
  const char* base_path = "_main/frontends/pbrt/test_data/";
  return runfiles->Rlocation(base_path + path);
}

TEST(LoadFile, NotAFile) {
  EXPECT_EXIT(LoadFile(std::filesystem::current_path(), "notarealfile"),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Failed to open file: notarealfile");
}

TEST(LoadFile, InvalidBinpb) {
  EXPECT_EXIT(LoadFile(std::filesystem::current_path(),
                       RawRunfilePath("invalid.pbrt.v3.binpb")),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Failed to parse binpb input:");
}

TEST(LoadFile, Binpb) {
  auto [proto, path] = LoadFile(std::filesystem::current_path(),
                                RawRunfilePath("cornell_box.pbrt.v3.binpb"));
  EXPECT_EQ(path,
            std::filesystem::path(RawRunfilePath("cornell_box.pbrt.v3.binpb")));
  ASSERT_EQ(proto.directives_size(), 36);
  EXPECT_TRUE(proto.directives(0).has_look_at());
}

TEST(LoadFile, InvalidTxtpb) {
  EXPECT_EXIT(LoadFile(std::filesystem::current_path(),
                       RawRunfilePath("invalid.pbrt.v3.txtpb")),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Failed to parse txtpb input:");
}

TEST(LoadFile, Txtpb) {
  auto [proto, path] = LoadFile(std::filesystem::current_path(),
                                RawRunfilePath("cornell_box.pbrt.v3.txtpb"));
  EXPECT_EQ(path,
            std::filesystem::path(RawRunfilePath("cornell_box.pbrt.v3.txtpb")));
  ASSERT_EQ(proto.directives_size(), 36);
  EXPECT_TRUE(proto.directives(0).has_look_at());
}

TEST(LoadFile, ConversionFailed) {
  EXPECT_EXIT(
      LoadFile(std::filesystem::current_path(), RawRunfilePath("rgba8.png")),
      ExitedWithCode(EXIT_FAILURE),
      "ERROR: Failed to parse pbrt file with error:");
}

TEST(LoadFile, Pbrt) {
  auto [proto, path] = LoadFile(std::filesystem::current_path(),
                                RawRunfilePath("cornell_box.pbrt"));
  EXPECT_EQ(path, std::filesystem::path(RawRunfilePath("cornell_box.pbrt")));
  ASSERT_EQ(proto.directives_size(), 36);
  EXPECT_TRUE(proto.directives(0).has_look_at());
}

}  // namespace
}  // namespace pbrt_frontend
}  // namespace iris
