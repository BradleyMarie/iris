#include "frontends/pbrt/texture_manager.h"

#include <cstdlib>
#include <filesystem>

#include "frontends/pbrt/spectrum_managers/color_spectrum_manager.h"
#include "frontends/pbrt/spectrum_managers/test_spectrum_manager.h"
#include "googletest/include/gtest/gtest.h"
#include "pbrt_proto/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace {

using ::iris::pbrt_frontend::spectrum_managers::ColorSpectrumManager;
using ::iris::pbrt_frontend::spectrum_managers::TestSpectrumManager;
using ::pbrt_proto::FloatTextureParameter;
using ::pbrt_proto::Spectrum;
using ::pbrt_proto::SpectrumTextureParameter;
using ::testing::ExitedWithCode;

TEST(TextureManager, MismatchedAttribute) {
  std::filesystem::path current_path = std::filesystem::current_path();
  ColorSpectrumManager spectrum_manager(current_path, true);
  TextureManager texture_manager(spectrum_manager);

  EXPECT_EXIT(texture_manager.AttributeEnd(), ExitedWithCode(EXIT_FAILURE),
              "ERROR: Mismatched AttributeBegin and AttributeEnd directives");
}

TEST(TextureManager, AttributePushPop) {
  std::filesystem::path current_path = std::filesystem::current_path();
  ColorSpectrumManager spectrum_manager(current_path, true);
  TextureManager texture_manager(spectrum_manager);

  texture_manager.AttributeBegin();
  texture_manager.Put("test",
                      ReferenceCounted<iris::textures::ReflectorTexture>());
  texture_manager.AttributeEnd();

  EXPECT_EXIT(texture_manager.GetReflectorTexture("test"),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: No spectrum texture defined with name: \"test\"");
}

TEST(TextureManager, AllocateFloatTexture) {
  std::filesystem::path current_path = std::filesystem::current_path();
  ColorSpectrumManager spectrum_manager(current_path, true);
  TextureManager texture_manager(spectrum_manager);

  ReferenceCounted<iris::textures::FloatTexture> tex0 =
      texture_manager.AllocateFloatTexture(1.0);
  EXPECT_TRUE(tex0);
  EXPECT_EQ(tex0, texture_manager.AllocateFloatTexture(1.0));
}

TEST(TextureManager, AllocateFloatTextureNull) {
  std::filesystem::path current_path = std::filesystem::current_path();
  ColorSpectrumManager spectrum_manager(current_path, true);
  TextureManager texture_manager(spectrum_manager);

  EXPECT_FALSE(texture_manager.AllocateFloatTexture(0.0));
}

TEST(TextureManager, AllocateFloatTextureEmpty) {
  std::filesystem::path current_path = std::filesystem::current_path();
  ColorSpectrumManager spectrum_manager(current_path, true);
  TextureManager texture_manager(spectrum_manager);

  FloatTextureParameter parameter;

  EXPECT_FALSE(texture_manager.AllocateFloatTexture(parameter));
}

TEST(TextureManager, AllocateFloatTextureConstant) {
  std::filesystem::path current_path = std::filesystem::current_path();
  ColorSpectrumManager spectrum_manager(current_path, true);
  TextureManager texture_manager(spectrum_manager);

  FloatTextureParameter parameter;
  parameter.set_float_value(1.0);

  EXPECT_TRUE(texture_manager.AllocateFloatTexture(parameter));
}

TEST(TextureManager, AllocateFloatTextureByName) {
  std::filesystem::path current_path = std::filesystem::current_path();
  ColorSpectrumManager spectrum_manager(current_path, true);
  TextureManager texture_manager(spectrum_manager);

  FloatTextureParameter parameter;
  parameter.set_float_texture_name("a");

  EXPECT_EXIT(texture_manager.AllocateFloatTexture(parameter),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: No float texture defined with name: \"a\"");
}

TEST(TextureManager, AllocateReflectorTexture) {
  std::filesystem::path current_path = std::filesystem::current_path();
  ColorSpectrumManager spectrum_manager(current_path, true);
  TextureManager texture_manager(spectrum_manager);

  EXPECT_TRUE(texture_manager.AllocateFloatTexture(1.0));
}

TEST(TextureManager, AllocateReflectorTextureNull) {
  std::filesystem::path current_path = std::filesystem::current_path();
  ColorSpectrumManager spectrum_manager(current_path, true);
  TextureManager texture_manager(spectrum_manager);

  EXPECT_FALSE(texture_manager.AllocateReflectorTexture(0.0));
}

TEST(TextureManager, AllocateReflectorTextureSpectrumNull) {
  std::filesystem::path current_path = std::filesystem::current_path();
  ColorSpectrumManager spectrum_manager(current_path, true);
  TextureManager texture_manager(spectrum_manager);

  Spectrum spectrum;

  EXPECT_FALSE(texture_manager.AllocateReflectorTexture(spectrum));
}

TEST(TextureManager, AllocateReflectorTextureSpectrum) {
  std::filesystem::path current_path = std::filesystem::current_path();
  ColorSpectrumManager spectrum_manager(current_path, true);
  TextureManager texture_manager(spectrum_manager);

  Spectrum spectrum;
  spectrum.set_constant_spectrum(1.0);

  EXPECT_TRUE(texture_manager.AllocateReflectorTexture(spectrum));
}

TEST(TextureManager, AllocateReflectorTextureSpectrumParameterEmpty) {
  std::filesystem::path current_path = std::filesystem::current_path();
  ColorSpectrumManager spectrum_manager(current_path, true);
  TextureManager texture_manager(spectrum_manager);

  SpectrumTextureParameter parameter;

  EXPECT_FALSE(texture_manager.AllocateReflectorTexture(parameter));
}

TEST(TextureManager, AllocateReflectorTextureSpectrumParameterConstant) {
  std::filesystem::path current_path = std::filesystem::current_path();
  ColorSpectrumManager spectrum_manager(current_path, true);
  TextureManager texture_manager(spectrum_manager);

  SpectrumTextureParameter parameter;
  parameter.set_constant_spectrum(1.0);

  EXPECT_TRUE(texture_manager.AllocateReflectorTexture(parameter));
}

TEST(TextureManager, AllocateReflectorTextureSpectrumParameterBlackbody) {
  std::filesystem::path current_path = std::filesystem::current_path();
  ColorSpectrumManager spectrum_manager(current_path, true);
  TextureManager texture_manager(spectrum_manager);

  SpectrumTextureParameter parameter;
  parameter.mutable_blackbody_spectrum()->set_temperature(65000.0);
  parameter.mutable_blackbody_spectrum()->set_scale(1.0);

  EXPECT_TRUE(texture_manager.AllocateReflectorTexture(parameter));
}

TEST(TextureManager, AllocateReflectorTextureSpectrumParameterRgb) {
  std::filesystem::path current_path = std::filesystem::current_path();
  ColorSpectrumManager spectrum_manager(current_path, true);
  TextureManager texture_manager(spectrum_manager);

  SpectrumTextureParameter parameter;
  parameter.mutable_rgb_spectrum()->set_r(1.0);
  parameter.mutable_rgb_spectrum()->set_g(1.0);
  parameter.mutable_rgb_spectrum()->set_b(1.0);

  EXPECT_TRUE(texture_manager.AllocateReflectorTexture(parameter));
}

TEST(TextureManager, AllocateReflectorTextureSpectrumParameterXyz) {
  std::filesystem::path current_path = std::filesystem::current_path();
  ColorSpectrumManager spectrum_manager(current_path, true);
  TextureManager texture_manager(spectrum_manager);

  SpectrumTextureParameter parameter;
  parameter.mutable_xyz_spectrum()->set_x(1.0);
  parameter.mutable_xyz_spectrum()->set_y(1.0);
  parameter.mutable_xyz_spectrum()->set_z(1.0);

  EXPECT_TRUE(texture_manager.AllocateReflectorTexture(parameter));
}

TEST(TextureManager, AllocateReflectorTextureSpectrumParameterSampled) {
  std::filesystem::path current_path = std::filesystem::current_path();
  ColorSpectrumManager spectrum_manager(current_path, true);
  TextureManager texture_manager(spectrum_manager);

  SpectrumTextureParameter parameter;
  auto& sample = *parameter.mutable_sampled_spectrum()->add_samples();
  sample.set_wavelength(1.0);
  sample.set_intensity(1.0);

  EXPECT_TRUE(texture_manager.AllocateReflectorTexture(parameter));
}

TEST(TextureManager, AllocateReflectorTextureSpectrumParameterFile) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);

  SpectrumTextureParameter parameter;
  parameter.set_sampled_spectrum_filename("a");

  EXPECT_TRUE(texture_manager.AllocateReflectorTexture(parameter));
}

TEST(TextureManager, AllocateReflectorTextureSpectrumParameterNamed) {
  std::filesystem::path current_path = std::filesystem::current_path();
  ColorSpectrumManager spectrum_manager(current_path, true);
  TextureManager texture_manager(spectrum_manager);

  SpectrumTextureParameter parameter;
  parameter.set_spectrum_texture_name("a");

  EXPECT_EXIT(texture_manager.AllocateReflectorTexture(parameter),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: No spectrum texture defined with name: \"a\"");
}

TEST(TextureManager, FloatTextureGetFails) {
  std::filesystem::path current_path = std::filesystem::current_path();
  ColorSpectrumManager spectrum_manager(current_path, true);
  TextureManager texture_manager(spectrum_manager);

  EXPECT_EXIT(texture_manager.GetFloatTexture("test"),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: No float texture defined with name: \"test\"");
}

TEST(TextureManager, ReflectorTextureGetFails) {
  std::filesystem::path current_path = std::filesystem::current_path();
  ColorSpectrumManager spectrum_manager(current_path, true);
  TextureManager texture_manager(spectrum_manager);

  EXPECT_EXIT(texture_manager.GetReflectorTexture("test"),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: No spectrum texture defined with name: \"test\"");
}

TEST(TextureManager, FloatTextureGetSucceeds) {
  std::filesystem::path current_path = std::filesystem::current_path();
  ColorSpectrumManager spectrum_manager(current_path, true);
  TextureManager texture_manager(spectrum_manager);

  ReferenceCounted<iris::textures::FloatTexture> texture;
  texture_manager.Put("test", texture);

  EXPECT_EQ(nullptr, texture_manager.GetFloatTexture("test").Get());
}

TEST(TextureManager, ReflectorTextureGetSucceeds) {
  std::filesystem::path current_path = std::filesystem::current_path();
  ColorSpectrumManager spectrum_manager(current_path, true);
  TextureManager texture_manager(spectrum_manager);

  ReferenceCounted<iris::textures::ReflectorTexture> texture;
  texture_manager.Put("test", texture);

  EXPECT_EQ(nullptr, texture_manager.GetReflectorTexture("test").Get());
}

}  // namespace
}  // namespace pbrt_frontend
}  // namespace iris
