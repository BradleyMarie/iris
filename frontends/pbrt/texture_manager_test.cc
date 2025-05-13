#include "frontends/pbrt/texture_manager.h"

#include "frontends/pbrt/spectrum_managers/color_spectrum_manager.h"
#include "frontends/pbrt/spectrum_managers/test_spectrum_manager.h"
#include "googletest/include/gtest/gtest.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace {

using ::iris::pbrt_frontend::spectrum_managers::ColorSpectrumManager;
using ::iris::pbrt_frontend::spectrum_managers::TestSpectrumManager;
using ::pbrt_proto::v3::FloatTextureParameter;
using ::pbrt_proto::v3::Spectrum;
using ::pbrt_proto::v3::SpectrumTextureParameter;

TEST(TextureManager, AllocateFloatTexture) {
  ColorSpectrumManager spectrum_manager(true);
  TextureManager texture_manager(spectrum_manager);

  TextureManager::FloatTexturePtr tex0 =
      texture_manager.AllocateFloatTexture(1.0);
  EXPECT_TRUE(tex0);
  EXPECT_EQ(tex0, texture_manager.AllocateFloatTexture(1.0));
}

TEST(TextureManager, AllocateFloatTextureNull) {
  ColorSpectrumManager spectrum_manager(true);
  TextureManager texture_manager(spectrum_manager);

  EXPECT_FALSE(texture_manager.AllocateFloatTexture(0.0));
}

TEST(TextureManager, AllocateFloatTextureEmpty) {
  ColorSpectrumManager spectrum_manager(true);
  TextureManager texture_manager(spectrum_manager);

  FloatTextureParameter parameter;

  EXPECT_FALSE(texture_manager.AllocateFloatTexture(parameter));
}

TEST(TextureManager, AllocateFloatTextureUniform) {
  ColorSpectrumManager spectrum_manager(true);
  TextureManager texture_manager(spectrum_manager);

  FloatTextureParameter parameter;
  parameter.set_float_value(1.0);

  EXPECT_TRUE(texture_manager.AllocateFloatTexture(parameter));
}

TEST(TextureManager, AllocateFloatTextureByName) {
  ColorSpectrumManager spectrum_manager(true);
  TextureManager texture_manager(spectrum_manager);

  FloatTextureParameter parameter;
  parameter.set_float_texture_name("a");

  EXPECT_EXIT(texture_manager.AllocateFloatTexture(parameter),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: No float texture defined with name: \"a\"");
}

TEST(TextureManager, AllocateReflectorTexture) {
  ColorSpectrumManager spectrum_manager(true);
  TextureManager texture_manager(spectrum_manager);

  EXPECT_TRUE(texture_manager.AllocateFloatTexture(1.0));
}

TEST(TextureManager, AllocateReflectorTextureNull) {
  ColorSpectrumManager spectrum_manager(true);
  TextureManager texture_manager(spectrum_manager);

  EXPECT_FALSE(texture_manager.AllocateReflectorTexture(0.0));
}

TEST(TextureManager, AllocateReflectorTextureSpectrumNull) {
  ColorSpectrumManager spectrum_manager(true);
  TextureManager texture_manager(spectrum_manager);

  Spectrum spectrum;

  EXPECT_FALSE(texture_manager.AllocateReflectorTexture(spectrum));
}

TEST(TextureManager, AllocateReflectorTextureSpectrum) {
  ColorSpectrumManager spectrum_manager(true);
  TextureManager texture_manager(spectrum_manager);

  Spectrum spectrum;
  spectrum.set_uniform_spectrum(1.0);

  EXPECT_TRUE(texture_manager.AllocateReflectorTexture(spectrum));
}

TEST(TextureManager, AllocateReflectorTextureSpectrumParameterEmpty) {
  ColorSpectrumManager spectrum_manager(true);
  TextureManager texture_manager(spectrum_manager);

  SpectrumTextureParameter parameter;

  EXPECT_FALSE(texture_manager.AllocateReflectorTexture(parameter));
}

TEST(TextureManager, AllocateReflectorTextureSpectrumParameterUniform) {
  ColorSpectrumManager spectrum_manager(true);
  TextureManager texture_manager(spectrum_manager);

  SpectrumTextureParameter parameter;
  parameter.set_uniform_spectrum(1.0);

  EXPECT_TRUE(texture_manager.AllocateReflectorTexture(parameter));
}

TEST(TextureManager, AllocateReflectorTextureSpectrumParameterBlackbody) {
  ColorSpectrumManager spectrum_manager(true);
  TextureManager texture_manager(spectrum_manager);

  SpectrumTextureParameter parameter;
  parameter.mutable_blackbody_spectrum();

  EXPECT_EXIT(texture_manager.AllocateReflectorTexture(parameter),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Blackbody spectrum parsing is not implemented");
}

TEST(TextureManager, AllocateReflectorTextureSpectrumParameterRgb) {
  ColorSpectrumManager spectrum_manager(true);
  TextureManager texture_manager(spectrum_manager);

  SpectrumTextureParameter parameter;
  parameter.mutable_rgb_spectrum()->set_r(1.0);
  parameter.mutable_rgb_spectrum()->set_g(1.0);
  parameter.mutable_rgb_spectrum()->set_b(1.0);

  EXPECT_TRUE(texture_manager.AllocateReflectorTexture(parameter));
}

TEST(TextureManager, AllocateReflectorTextureSpectrumParameterXyz) {
  ColorSpectrumManager spectrum_manager(true);
  TextureManager texture_manager(spectrum_manager);

  SpectrumTextureParameter parameter;
  parameter.mutable_xyz_spectrum()->set_x(1.0);
  parameter.mutable_xyz_spectrum()->set_y(1.0);
  parameter.mutable_xyz_spectrum()->set_z(1.0);

  EXPECT_TRUE(texture_manager.AllocateReflectorTexture(parameter));
}

TEST(TextureManager, AllocateReflectorTextureSpectrumParameterSampled) {
  ColorSpectrumManager spectrum_manager(true);
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
  ColorSpectrumManager spectrum_manager(true);
  TextureManager texture_manager(spectrum_manager);

  SpectrumTextureParameter parameter;
  parameter.set_spectrum_texture_name("a");

  EXPECT_EXIT(texture_manager.AllocateReflectorTexture(parameter),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: No spectrum texture defined with name: \"a\"");
}

TEST(TextureManager, FloatTextureGetFails) {
  ColorSpectrumManager spectrum_manager(true);
  TextureManager texture_manager(spectrum_manager);

  EXPECT_EXIT(texture_manager.GetFloatTexture("test"),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: No float texture defined with name: \"test\"");
}

TEST(TextureManager, ReflectorTextureGetFails) {
  ColorSpectrumManager spectrum_manager(true);
  TextureManager texture_manager(spectrum_manager);

  EXPECT_EXIT(texture_manager.GetReflectorTexture("test"),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: No spectrum texture defined with name: \"test\"");
}

TEST(TextureManager, FloatTextureGetSucceeds) {
  ColorSpectrumManager spectrum_manager(true);
  TextureManager texture_manager(spectrum_manager);

  TextureManager::FloatTexturePtr texture;
  texture_manager.Put("test", texture);

  EXPECT_EQ(nullptr, texture_manager.GetFloatTexture("test").Get());
}

TEST(TextureManager, ReflectorTextureGetSucceeds) {
  ColorSpectrumManager spectrum_manager(true);
  TextureManager texture_manager(spectrum_manager);

  TextureManager::ReflectorTexturePtr texture;
  texture_manager.Put("test", texture);

  EXPECT_EQ(nullptr, texture_manager.GetReflectorTexture("test").Get());
}

}  // namespace
}  // namespace pbrt_frontend
}  // namespace iris
