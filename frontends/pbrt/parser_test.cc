#include "frontends/pbrt/parser.h"

#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <memory>

#include "googletest/include/gtest/gtest.h"
#include "iris/random_bitstreams/mersenne_twister_random_bitstream.h"
#include "pbrt_proto/v3/pbrt.pb.h"
#include "tools/cpp/runfiles/runfiles.h"

namespace iris {
namespace pbrt_frontend {
namespace {

using ::bazel::tools::cpp::runfiles::Runfiles;
using ::iris::random_bitstreams::MakeMersenneTwisterRandomBitstream;
using ::pbrt_proto::v3::PbrtProto;
using ::testing::ExitedWithCode;

std::string RunfilePath(const std::string& path) {
  std::unique_ptr<Runfiles> runfiles(Runfiles::CreateForTest());
  const char* base_path = "_main/frontends/pbrt/test_data/";
  return runfiles->Rlocation(base_path + path);
}

TEST(ParseScene, Empty) {
  PbrtProto proto;

  Directives directives;
  directives.Include(proto);

  Options options;

  EXPECT_FALSE(ParseScene(directives, options, std::filesystem::current_path())
                   .has_value());
}

TEST(ParseScene, NoEnd) {
  PbrtProto proto;
  proto.add_directives()->mutable_world_begin();

  Directives directives;
  directives.Include(proto);

  Options options;

  EXPECT_EXIT(ParseScene(directives, options, std::filesystem::current_path()),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Final directive should be WorldEnd");
}

TEST(AreaLightSource, BeforeWorldEnd) {
  PbrtProto proto;
  proto.add_directives()->mutable_area_light_source();

  Directives directives;
  directives.Include(proto);

  Options options;

  EXPECT_EXIT(ParseScene(directives, options, std::filesystem::current_path()),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Directive cannot be specified before WorldBegin: "
              "AreaLightSource");
}

TEST(AttributeBegin, BeforeWorldEnd) {
  PbrtProto proto;
  proto.add_directives()->mutable_attribute_begin();

  Directives directives;
  directives.Include(proto);

  Options options;

  EXPECT_EXIT(ParseScene(directives, options, std::filesystem::current_path()),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Directive cannot be specified before WorldBegin: "
              "AttributeBegin");
}

TEST(AttributeBegin, Mismatched) {
  PbrtProto proto;
  proto.add_directives()->mutable_world_begin();
  proto.add_directives()->mutable_attribute_begin();
  proto.add_directives()->mutable_transform_end();

  Directives directives;
  directives.Include(proto);

  Options options;

  EXPECT_EXIT(ParseScene(directives, options, std::filesystem::current_path()),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Mismatched AttributeBegin and AttributeEnd directives");
}

TEST(AttributeEnd, BeforeWorldEnd) {
  PbrtProto proto;
  proto.add_directives()->mutable_attribute_end();

  Directives directives;
  directives.Include(proto);

  Options options;

  EXPECT_EXIT(ParseScene(directives, options, std::filesystem::current_path()),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Directive cannot be specified before WorldBegin: "
              "AttributeEnd");
}

TEST(AttributeEnd, Mismatched) {
  PbrtProto proto;
  proto.add_directives()->mutable_world_begin();
  proto.add_directives()->mutable_attribute_end();

  Directives directives;
  directives.Include(proto);

  Options options;

  EXPECT_EXIT(ParseScene(directives, options, std::filesystem::current_path()),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Mismatched AttributeBegin and AttributeEnd directives");
}

TEST(Camera, AfterWorldBegin) {
  PbrtProto proto;
  proto.add_directives()->mutable_world_begin();
  proto.add_directives()->mutable_camera();

  Directives directives;
  directives.Include(proto);

  Options options;

  EXPECT_EXIT(ParseScene(directives, options, std::filesystem::current_path()),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Directive cannot be specified between WorldBegin and "
              "WorldEnd: Camera");
}

TEST(Film, AfterWorldBegin) {
  PbrtProto proto;
  proto.add_directives()->mutable_world_begin();
  proto.add_directives()->mutable_film();

  Directives directives;
  directives.Include(proto);

  Options options;

  EXPECT_EXIT(ParseScene(directives, options, std::filesystem::current_path()),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Directive cannot be specified between WorldBegin and "
              "WorldEnd: Film");
}

TEST(Include, Circular) {
  std::filesystem::path path = RunfilePath("include_circular_first.pbrt");

  PbrtProto proto;
  proto.add_directives()->mutable_include()->set_path(path.native());

  Directives directives;
  directives.Include(proto);

  Options options;

  EXPECT_EXIT(ParseScene(directives, options, path.parent_path()),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Detected cyclic Include of file:");
}

TEST(Integrator, AfterWorldBegin) {
  PbrtProto proto;
  proto.add_directives()->mutable_world_begin();
  proto.add_directives()->mutable_integrator();

  Directives directives;
  directives.Include(proto);

  Options options;

  EXPECT_EXIT(ParseScene(directives, options, std::filesystem::current_path()),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Directive cannot be specified between WorldBegin and "
              "WorldEnd: Integrator");
}

TEST(LightSource, BeforeWorldBegin) {
  PbrtProto proto;
  proto.add_directives()->mutable_light_source();

  Directives directives;
  directives.Include(proto);

  Options options;

  EXPECT_EXIT(
      ParseScene(directives, options, std::filesystem::current_path()),
      ExitedWithCode(EXIT_FAILURE),
      "ERROR: Directive cannot be specified before WorldBegin: LightSource");
}

TEST(MakeNamedMaterial, BeforeWorldBegin) {
  PbrtProto proto;
  proto.add_directives()->mutable_make_named_material();

  Directives directives;
  directives.Include(proto);

  Options options;

  EXPECT_EXIT(ParseScene(directives, options, std::filesystem::current_path()),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Directive cannot be specified before WorldBegin: "
              "MakeNamedMaterial");
}

TEST(Material, BeforeWorldBegin) {
  PbrtProto proto;
  proto.add_directives()->mutable_material();

  Directives directives;
  directives.Include(proto);

  Options options;

  EXPECT_EXIT(
      ParseScene(directives, options, std::filesystem::current_path()),
      ExitedWithCode(EXIT_FAILURE),
      "ERROR: Directive cannot be specified before WorldBegin: Material");
}

TEST(NamedMaterial, BeforeWorldBegin) {
  PbrtProto proto;
  proto.add_directives()->mutable_named_material();

  Directives directives;
  directives.Include(proto);

  Options options;

  EXPECT_EXIT(
      ParseScene(directives, options, std::filesystem::current_path()),
      ExitedWithCode(EXIT_FAILURE),
      "ERROR: Directive cannot be specified before WorldBegin: NamedMaterial");
}

TEST(ObjectBegin, BeforeWorldBegin) {
  PbrtProto proto;
  proto.add_directives()->mutable_object_begin();

  Directives directives;
  directives.Include(proto);

  Options options;

  EXPECT_EXIT(
      ParseScene(directives, options, std::filesystem::current_path()),
      ExitedWithCode(EXIT_FAILURE),
      "ERROR: Directive cannot be specified before WorldBegin: ObjectBegin");
}

TEST(ObjectBegin, Mismatched) {
  PbrtProto proto;
  proto.add_directives()->mutable_world_begin();
  proto.add_directives()->mutable_object_begin();
  proto.add_directives()->mutable_object_begin();

  Directives directives;
  directives.Include(proto);

  Options options;

  EXPECT_EXIT(ParseScene(directives, options, std::filesystem::current_path()),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Mismatched ObjectBegin and ObjectEnd directives");
}

TEST(ObjectEnd, BeforeWorldBegin) {
  PbrtProto proto;
  proto.add_directives()->mutable_object_end();

  Directives directives;
  directives.Include(proto);

  Options options;

  EXPECT_EXIT(
      ParseScene(directives, options, std::filesystem::current_path()),
      ExitedWithCode(EXIT_FAILURE),
      "ERROR: Directive cannot be specified before WorldBegin: ObjectEnd");
}

TEST(ObjectEnd, Mismatched) {
  PbrtProto proto;
  proto.add_directives()->mutable_world_begin();
  proto.add_directives()->mutable_object_end();

  Directives directives;
  directives.Include(proto);

  Options options;

  EXPECT_EXIT(ParseScene(directives, options, std::filesystem::current_path()),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Mismatched ObjectBegin and ObjectEnd directives");
}

TEST(ObjectInstance, BeforeWorldBegin) {
  PbrtProto proto;
  proto.add_directives()->mutable_object_instance();

  Directives directives;
  directives.Include(proto);

  Options options;

  EXPECT_EXIT(
      ParseScene(directives, options, std::filesystem::current_path()),
      ExitedWithCode(EXIT_FAILURE),
      "ERROR: Directive cannot be specified before WorldBegin: ObjectInstance");
}

TEST(ObjectInstance, InsideObject) {
  PbrtProto proto;
  proto.add_directives()->mutable_world_begin();
  proto.add_directives()->mutable_object_begin();
  proto.add_directives()->mutable_object_instance();

  Directives directives;
  directives.Include(proto);

  Options options;

  EXPECT_EXIT(ParseScene(directives, options, std::filesystem::current_path()),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: ObjectInstance cannot be specified between ObjectBegin "
              "and ObjectEnd");
}

TEST(ObjectInstance, MissingObject) {
  PbrtProto proto;
  proto.add_directives()->mutable_world_begin();
  proto.add_directives()->mutable_object_instance()->set_name("1");

  Directives directives;
  directives.Include(proto);

  Options options;

  EXPECT_EXIT(ParseScene(directives, options, std::filesystem::current_path()),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: ObjectInstance referred to an unknown object: 1");
}

TEST(PixelFilter, AfterWorldBegin) {
  PbrtProto proto;
  proto.add_directives()->mutable_world_begin();
  proto.add_directives()->mutable_pixel_filter();

  Directives directives;
  directives.Include(proto);

  Options options;

  EXPECT_EXIT(ParseScene(directives, options, std::filesystem::current_path()),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Directive cannot be specified between WorldBegin and "
              "WorldEnd: PixelFilter");
}

TEST(ReverseOrientation, BeforeWorldBegin) {
  PbrtProto proto;
  proto.add_directives()->mutable_reverse_orientation();

  Directives directives;
  directives.Include(proto);

  Options options;

  EXPECT_EXIT(ParseScene(directives, options, std::filesystem::current_path()),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Directive cannot be specified before WorldBegin: "
              "ReverseOrientation");
}

TEST(Sampler, AfterWorldBegin) {
  PbrtProto proto;
  proto.add_directives()->mutable_world_begin();
  proto.add_directives()->mutable_sampler();

  Directives directives;
  directives.Include(proto);

  Options options;

  EXPECT_EXIT(ParseScene(directives, options, std::filesystem::current_path()),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Directive cannot be specified between WorldBegin and "
              "WorldEnd: Sampler");
}

TEST(Shape, BeforeWorldBegin) {
  PbrtProto proto;
  proto.add_directives()->mutable_shape();

  Directives directives;
  directives.Include(proto);

  Options options;

  EXPECT_EXIT(ParseScene(directives, options, std::filesystem::current_path()),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Directive cannot be specified before WorldBegin: "
              "Shape");
}

TEST(Texture, FloatTextureBeforeWorldBegin) {
  PbrtProto proto;
  proto.add_directives()->mutable_float_texture();

  Directives directives;
  directives.Include(proto);

  Options options;

  EXPECT_EXIT(ParseScene(directives, options, std::filesystem::current_path()),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Directive cannot be specified before WorldBegin: "
              "Texture");
}

TEST(Texture, SpectrumTextureBeforeWorldBegin) {
  PbrtProto proto;
  proto.add_directives()->mutable_spectrum_texture();

  Directives directives;
  directives.Include(proto);

  Options options;

  EXPECT_EXIT(ParseScene(directives, options, std::filesystem::current_path()),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Directive cannot be specified before WorldBegin: "
              "Texture");
}

TEST(WorldBegin, Mismatched) {
  PbrtProto proto;
  proto.add_directives()->mutable_world_begin();
  proto.add_directives()->mutable_world_begin();

  Directives directives;
  directives.Include(proto);

  Options options;

  EXPECT_EXIT(ParseScene(directives, options, std::filesystem::current_path()),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Invalid WorldBegin directive");
}

TEST(WorldEnd, Mismatched) {
  PbrtProto proto;
  proto.add_directives()->mutable_world_end();

  Directives directives;
  directives.Include(proto);

  Options options;

  EXPECT_EXIT(ParseScene(directives, options, std::filesystem::current_path()),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Invalid WorldEnd directive");
}

TEST(Render, EmptyScene) {
  PbrtProto proto;
  proto.add_directives()->mutable_world_begin();
  proto.add_directives()->mutable_world_end();

  Directives directives;
  directives.Include(proto);

  Options options;

  std::optional<ParsingResult> result =
      ParseScene(directives, options, std::filesystem::current_path());
  ASSERT_TRUE(result.has_value());

  EXPECT_FALSE(result->skip_pixel_callback({0, 0}, {720, 1280}));
  EXPECT_FALSE(result->skip_pixel_callback({719, 1279}, {720, 1280}));
  EXPECT_TRUE(result->skip_pixel_callback({720, 1280}, {720, 1280}));
  EXPECT_FALSE(std::isfinite(result->max_sample_luminance));
  EXPECT_GT(result->max_sample_luminance, 0.0);
  EXPECT_EQ("pbrt.exr", result->output_filename);

  std::unique_ptr<RandomBitstream> rng = MakeMersenneTwisterRandomBitstream();

  Framebuffer framebuffer = result->renderable.Render(*rng);
  std::pair<size_t, size_t> dimensions = framebuffer.Size();
  EXPECT_EQ(720u, dimensions.first);
  EXPECT_EQ(1280u, dimensions.second);

  for (size_t y = 0; y < dimensions.first; y++) {
    for (size_t x = 0; x < dimensions.second; x++) {
      Color color = framebuffer.Get(y, x);
      EXPECT_EQ(0.0, color.r);
      EXPECT_EQ(0.0, color.g);
      EXPECT_EQ(0.0, color.b);
      EXPECT_EQ(Color::LINEAR_SRGB, color.space);
    }
  }
}

}  // namespace
}  // namespace pbrt_frontend
}  // namespace iris
