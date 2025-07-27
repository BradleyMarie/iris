#include "iris/textures/ptex_texture.h"

#include <limits>
#include <memory>

#include "googletest/include/gtest/gtest.h"
#include "iris/reference_counted.h"
#include "iris/reflectors/mock_reflector.h"
#include "iris/textures/constant_texture.h"
#include "iris/textures/reflector_texture.h"
#include "third_party/disney/ptex/Ptexture.h"
#include "tools/cpp/runfiles/runfiles.h"

namespace iris {
namespace textures {
namespace {

using ::iris::reflectors::MockReflector;
using ::iris::textures::MakeConstantTexture;

using bazel::tools::cpp::runfiles::Runfiles;

std::string RunfilePath(const std::string& path) {
  std::unique_ptr<Runfiles> runfiles(Runfiles::CreateForTest());
  const char* base_path = "_main/iris/textures/test_data/";
  return runfiles->Rlocation(base_path + path);
}

std::shared_ptr<Ptex::PtexCache> GetTextureCache() {
  return std::shared_ptr<Ptex::PtexCache>(
      Ptex::PtexCache::create(/*maxFiles=*/100, /*maxMem=*/0x100000000ull,
                              /*premultiply=*/true),
      [](Ptex::PtexCache* cache) { cache->release(); });
}

TEST(PtexFloatTexture, Null) {
  EXPECT_FALSE(MakePtexTexture(nullptr, RunfilePath("teapot.ptx"), 2.2));
  EXPECT_FALSE(MakePtexTexture(GetTextureCache(), "bad", 2.2));
  EXPECT_FALSE(
      MakePtexTexture(GetTextureCache(), RunfilePath("teapot.ptx"), 0.0));
  EXPECT_FALSE(MakePtexTexture(GetTextureCache(), RunfilePath("teapot.ptx"),
                               std::numeric_limits<visual_t>::infinity()));
  EXPECT_TRUE(
      MakePtexTexture(GetTextureCache(), RunfilePath("teapot.ptx"), 2.2));
}

TEST(PtexReflectorTexture, Null) {
  EXPECT_FALSE(MakePtexTexture(
      nullptr, RunfilePath("teapot.ptx"), 2.2,
      MakeConstantTexture(MakeReferenceCounted<MockReflector>()),
      MakeConstantTexture(MakeReferenceCounted<MockReflector>()),
      MakeConstantTexture(MakeReferenceCounted<MockReflector>())));
  EXPECT_FALSE(MakePtexTexture(
      GetTextureCache(), "bad", 2.2,
      MakeConstantTexture(MakeReferenceCounted<MockReflector>()),
      MakeConstantTexture(MakeReferenceCounted<MockReflector>()),
      MakeConstantTexture(MakeReferenceCounted<MockReflector>())));
  EXPECT_FALSE(MakePtexTexture(
      GetTextureCache(), RunfilePath("teapot.ptx"), 0.0,
      MakeConstantTexture(MakeReferenceCounted<MockReflector>()),
      MakeConstantTexture(MakeReferenceCounted<MockReflector>()),
      MakeConstantTexture(MakeReferenceCounted<MockReflector>())));
  EXPECT_FALSE(MakePtexTexture(
      GetTextureCache(), RunfilePath("teapot.ptx"),
      std::numeric_limits<visual_t>::infinity(),
      MakeConstantTexture(MakeReferenceCounted<MockReflector>()),
      MakeConstantTexture(MakeReferenceCounted<MockReflector>()),
      MakeConstantTexture(MakeReferenceCounted<MockReflector>())));
  EXPECT_FALSE(MakePtexTexture(GetTextureCache(), RunfilePath("teapot.ptx"),
                               2.2, ReferenceCounted<ReflectorTexture>(),
                               ReferenceCounted<ReflectorTexture>(),
                               ReferenceCounted<ReflectorTexture>()));
  EXPECT_TRUE(MakePtexTexture(
      GetTextureCache(), RunfilePath("teapot.ptx"), 2.2,
      MakeConstantTexture(MakeReferenceCounted<MockReflector>()),
      MakeConstantTexture(MakeReferenceCounted<MockReflector>()),
      MakeConstantTexture(MakeReferenceCounted<MockReflector>())));
}

}  // namespace
}  // namespace textures
}  // namespace iris
