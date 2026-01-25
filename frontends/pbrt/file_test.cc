#include "frontends/pbrt/file.h"

#include <cstdlib>
#include <filesystem>
#include <string>

#include "googletest/include/gtest/gtest.h"
#include "pbrt_proto/v3/v3.pb.h"
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
  PbrtProto proto;
  EXPECT_EXIT(LoadFile(std::filesystem::current_path(), "notarealfile", proto),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Failed to open file: notarealfile");
}

TEST(LoadFile, InvalidBinpb) {
  PbrtProto proto;
  EXPECT_EXIT(LoadFile(std::filesystem::current_path(),
                       RawRunfilePath("invalid.pbrt.v3.binpb"), proto),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Failed to parse binpb input:");
}

TEST(LoadFile, Binpb) {
  PbrtProto proto;
  std::filesystem::path path =
      LoadFile(std::filesystem::current_path(),
               RawRunfilePath("cornell_box.pbrt.v3.binpb"), proto);
  EXPECT_EQ(path,
            std::filesystem::path(RawRunfilePath("cornell_box.pbrt.v3.binpb")));
  ASSERT_EQ(proto.directives_size(), 36);
  EXPECT_TRUE(proto.directives(0).has_look_at());
}

TEST(LoadFile, InvalidTxtpb) {
  PbrtProto proto;
  EXPECT_EXIT(LoadFile(std::filesystem::current_path(),
                       RawRunfilePath("invalid.pbrt.v3.txtpb"), proto),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Failed to parse txtpb input:");
}

TEST(LoadFile, Txtpb) {
  PbrtProto proto;
  std::filesystem::path path =
      LoadFile(std::filesystem::current_path(),
               RawRunfilePath("cornell_box.pbrt.v3.txtpb"), proto);
  EXPECT_EQ(path,
            std::filesystem::path(RawRunfilePath("cornell_box.pbrt.v3.txtpb")));
  ASSERT_EQ(proto.directives_size(), 36);
  EXPECT_TRUE(proto.directives(0).has_look_at());
}

TEST(LoadFile, ConversionFailed) {
  PbrtProto proto;
  EXPECT_EXIT(LoadFile(std::filesystem::current_path(),
                       RawRunfilePath("rgba8.png"), proto),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Failed to parse pbrt file with error:");
}

TEST(LoadFile, Pbrt) {
  PbrtProto proto;
  std::filesystem::path path =
      LoadFile(std::filesystem::current_path(),
               RawRunfilePath("cornell_box.pbrt"), proto);
  EXPECT_EQ(path, std::filesystem::path(RawRunfilePath("cornell_box.pbrt")));
  ASSERT_EQ(proto.directives_size(), 36);
  EXPECT_TRUE(proto.directives(0).has_look_at());
}

}  // namespace
}  // namespace pbrt_frontend
}  // namespace iris
