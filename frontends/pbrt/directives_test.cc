#include "frontends/pbrt/directives.h"

#include <cstdlib>
#include <filesystem>
#include <memory>
#include <string>

#include "google/protobuf/arena.h"
#include "googletest/include/gtest/gtest.h"
#include "tools/cpp/runfiles/runfiles.h"

namespace iris {
namespace pbrt_frontend {
namespace {

using ::bazel::tools::cpp::runfiles::Runfiles;
using ::google::protobuf::Arena;
using ::pbrt_proto::v3::Directive;
using ::pbrt_proto::v3::PbrtProto;
using ::testing::ExitedWithCode;

std::string RawRunfilePath(const std::string& path) {
  std::unique_ptr<Runfiles> runfiles(Runfiles::CreateForTest());
  const char* base_path = "_main/frontends/pbrt/test_data/";
  return runfiles->Rlocation(base_path + path);
}

TEST(Directive, IncludeCyclic) {
  Directives directives;
  directives.Include(PbrtProto(),
                     std::filesystem::path(RawRunfilePath("cornell_box.pbrt")));
  EXPECT_EXIT(
      directives.Include(PbrtProto(), std::filesystem::path(
                                          RawRunfilePath("cornell_box.pbrt"))),
      ExitedWithCode(EXIT_FAILURE), "ERROR: Detected cyclic Include of file:");
}

TEST(Directive, IncludeCyclicByArena) {
  std::unique_ptr<Arena> arena = std::make_unique<Arena>();
  PbrtProto* proto = Arena::Create<PbrtProto>(arena.get());

  Directives directives;
  directives.Include(*proto,
                     std::filesystem::path(RawRunfilePath("cornell_box.pbrt")));
  EXPECT_EXIT(directives.Include(
                  std::move(arena), proto,
                  std::filesystem::path(RawRunfilePath("cornell_box.pbrt"))),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Detected cyclic Include of file:");
}

TEST(Directive, Next) {
  PbrtProto first;
  first.add_directives()->mutable_world_begin();

  std::unique_ptr<Arena> arena = std::make_unique<Arena>();
  PbrtProto* second = Arena::Create<PbrtProto>(arena.get());
  second->add_directives()->mutable_world_end();

  Directives directives;
  directives.Include(std::move(arena), second, "second");
  directives.Include(first, "first");

  EXPECT_TRUE(directives.HasNext());
  const Directive* first_directive = directives.Next();
  ASSERT_TRUE(first_directive);
  EXPECT_EQ(first.directives(0).DebugString(), first_directive->DebugString());

  EXPECT_TRUE(directives.HasNext());
  const Directive* second_directive = directives.Next();
  ASSERT_TRUE(second_directive);
  EXPECT_EQ(second->directives(0).DebugString(),
            second_directive->DebugString());

  EXPECT_FALSE(directives.HasNext());
  EXPECT_FALSE(directives.Next());
}

}  // namespace
}  // namespace pbrt_frontend
}  // namespace iris
