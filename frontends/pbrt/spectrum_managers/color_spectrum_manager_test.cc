#include "frontends/pbrt/spectrum_managers/color_spectrum_manager.h"

#include <array>

#include "frontends/pbrt/spectrum_managers/internal/color_spectrum.h"
#include "googletest/include/gtest/gtest.h"
#include "iris/color.h"
#include "iris/reference_counted.h"
#include "iris/spectrum.h"
#include "tools/cpp/runfiles/runfiles.h"

namespace iris {
namespace pbrt_frontend {
namespace spectrum_managers {
namespace {

using bazel::tools::cpp::runfiles::Runfiles;
using ::pbrt_proto::v3::Spectrum;
using ::testing::ExitedWithCode;

std::string RunfilePath(const std::string& path) {
  std::unique_ptr<Runfiles> runfiles(Runfiles::CreateForTest());
  const char* base_path = "_main/frontends/pbrt/spectrum_managers/test_data/";
  return runfiles->Rlocation(base_path + path);
}

TEST(ColorColorMatcher, Match) {
  ColorColorMatcher color_matcher;

  ReferenceCounted<iris::Spectrum> spectrum =
      internal::MakeColorSpectrum(0.25, 0.5, 0.75, Color::LINEAR_SRGB);
  std::array<visual_t, 3> values = color_matcher.Match(*spectrum);
  EXPECT_EQ(0.25, values[0]);
  EXPECT_EQ(0.50, values[1]);
  EXPECT_EQ(0.75, values[2]);
}

TEST(ColorColorMatcher, ColorSpace) {
  ColorColorMatcher color_matcher;

  EXPECT_EQ(Color::LINEAR_SRGB, color_matcher.ColorSpace());
}

TEST(ColorAlbedoMatcher, Match) {
  std::filesystem::path path = std::filesystem::current_path();
  ColorAlbedoMatcher albedo_matcher;
  ColorSpectrumManager spectrum_manager(path, false);

  Spectrum xyz_spectrum;
  xyz_spectrum.mutable_xyz_spectrum()->set_x(0.6);
  xyz_spectrum.mutable_xyz_spectrum()->set_y(0.5);
  xyz_spectrum.mutable_xyz_spectrum()->set_z(0.4);

  ReferenceCounted<Reflector> reflector =
      spectrum_manager.AllocateReflector(xyz_spectrum);
  EXPECT_NEAR(0.5, albedo_matcher.Match(*reflector), 0.001);
}

TEST(ColorPowerMatcher, Match) {
  std::filesystem::path path = std::filesystem::current_path();
  ColorPowerMatcher power_matcher;
  ColorSpectrumManager spectrum_manager(path, false);

  Spectrum xyz_spectrum;
  xyz_spectrum.mutable_xyz_spectrum()->set_x(0.75);
  xyz_spectrum.mutable_xyz_spectrum()->set_y(0.5);
  xyz_spectrum.mutable_xyz_spectrum()->set_z(0.25);

  ReferenceCounted<iris::Spectrum> spectrum =
      spectrum_manager.AllocateSpectrum(xyz_spectrum);
  EXPECT_NEAR(0.5, power_matcher.Match(*spectrum), 0.001);
}

TEST(ColorSpectrumManager, ComputeLuma) {
  std::filesystem::path path = std::filesystem::current_path();
  ColorSpectrumManager spectrum_manager(path, false);

  Color xyz(0.4, 0.5, 0.6, Color::CIE_XYZ);
  Color rgb = xyz.ConvertTo(Color::LINEAR_SRGB);
  EXPECT_NEAR(0.5, spectrum_manager.ComputeLuma(rgb.r, rgb.g, rgb.b), 0.001);
}

TEST(ColorSpectrumManager, AllocateSpectrumFromUniform) {
  std::filesystem::path path = std::filesystem::current_path();
  ColorSpectrumManager spectrum_manager(path, false);

  Spectrum uniform_spectrum;
  uniform_spectrum.set_uniform_spectrum(0.5);

  ReferenceCounted<iris::Spectrum> spectrum =
      spectrum_manager.AllocateSpectrum(uniform_spectrum);
  EXPECT_EQ(0.5, spectrum->Intensity(0.5));
  EXPECT_EQ(0.5, spectrum->Intensity(1.5));
  EXPECT_EQ(0.5, spectrum->Intensity(2.5));
}

TEST(ColorSpectrumManager, AllocateSpectrumFromBlackbody) {
  std::filesystem::path path = std::filesystem::current_path();
  ColorSpectrumManager spectrum_manager(path, false);

  Spectrum blackbody_spectrum;
  blackbody_spectrum.mutable_blackbody_spectrum()->set_temperature(65000.0);
  blackbody_spectrum.mutable_blackbody_spectrum()->set_scale(2.0);

  ReferenceCounted<iris::Spectrum> spectrum =
      spectrum_manager.AllocateSpectrum(blackbody_spectrum);
  EXPECT_NEAR(0.144622, spectrum->Intensity(0.5), 0.01);
  EXPECT_NEAR(0.206813, spectrum->Intensity(1.5), 0.01);
  EXPECT_NEAR(0.447234, spectrum->Intensity(2.5), 0.01);
}

TEST(ColorSpectrumManager, AllocateSpectrumFromColor) {
  std::filesystem::path path = std::filesystem::current_path();
  ColorSpectrumManager spectrum_manager(path, false);

  Spectrum rgb_spectrum;
  rgb_spectrum.mutable_rgb_spectrum()->set_r(0.25);
  rgb_spectrum.mutable_rgb_spectrum()->set_g(0.5);
  rgb_spectrum.mutable_rgb_spectrum()->set_b(0.75);

  ReferenceCounted<iris::Spectrum> spectrum =
      spectrum_manager.AllocateSpectrum(rgb_spectrum);
  EXPECT_EQ(0.25, spectrum->Intensity(0.5));
  EXPECT_EQ(0.50, spectrum->Intensity(1.5));
  EXPECT_EQ(0.75, spectrum->Intensity(2.5));
}

TEST(ColorSpectrumManager, AllocateSpectrumFromColorXYZ) {
  std::filesystem::path path = std::filesystem::current_path();
  ColorSpectrumManager spectrum_manager(path, false);

  Spectrum xyz_spectrum;
  xyz_spectrum.mutable_xyz_spectrum()->set_x(0.4);
  xyz_spectrum.mutable_xyz_spectrum()->set_y(0.5);
  xyz_spectrum.mutable_xyz_spectrum()->set_z(0.6);

  visual_t luma;
  ReferenceCounted<iris::Spectrum> spectrum =
      spectrum_manager.AllocateSpectrum(xyz_spectrum, &luma);
  EXPECT_NEAR(0.228493, spectrum->Intensity(0.5), 0.001);
  EXPECT_NEAR(0.575232, spectrum->Intensity(1.5), 0.001);
  EXPECT_NEAR(0.554579, spectrum->Intensity(2.5), 0.001);
  EXPECT_EQ(0.5, luma);
}

TEST(ColorSpectrumManager, AllocateSpectrumFromColorZero) {
  std::filesystem::path path = std::filesystem::current_path();
  ColorSpectrumManager spectrum_manager(path, false);

  Spectrum rgb_spectrum;
  rgb_spectrum.mutable_rgb_spectrum()->set_r(0.0);
  rgb_spectrum.mutable_rgb_spectrum()->set_g(0.0);
  rgb_spectrum.mutable_rgb_spectrum()->set_b(0.0);

  visual_t luma;
  ReferenceCounted<iris::Spectrum> spectrum =
      spectrum_manager.AllocateSpectrum(rgb_spectrum, &luma);
  EXPECT_FALSE(spectrum);
  EXPECT_EQ(0.0, luma);
}

TEST(ColorSpectrumManager, AllocateSpectrumFromSpectrum) {
  std::filesystem::path path = std::filesystem::current_path();
  ColorSpectrumManager spectrum_manager(path, false);

  Spectrum sampled_spectrum;
  auto& sample = *sampled_spectrum.mutable_sampled_spectrum()->add_samples();
  sample.set_wavelength(1.0);
  sample.set_intensity(1.0);

  ReferenceCounted<iris::Spectrum> spectrum =
      spectrum_manager.AllocateSpectrum(sampled_spectrum);
  EXPECT_NEAR(128.75, spectrum->Intensity(0.5), 0.001);
  EXPECT_NEAR(101.325, spectrum->Intensity(1.5), 0.001);
  EXPECT_NEAR(97.154, spectrum->Intensity(2.5), 0.001);
}

TEST(ColorSpectrumManager, AllocateSpectrumFromSpectrumZero) {
  std::filesystem::path path = std::filesystem::current_path();
  ColorSpectrumManager spectrum_manager(path, false);

  Spectrum sampled_spectrum;
  auto& sample = *sampled_spectrum.mutable_sampled_spectrum()->add_samples();
  sample.set_wavelength(1.0);
  sample.set_intensity(0.0);

  visual_t luma;
  ReferenceCounted<iris::Spectrum> spectrum =
      spectrum_manager.AllocateSpectrum(sampled_spectrum, &luma);
  EXPECT_FALSE(spectrum);
  EXPECT_EQ(0.0, luma);
}

TEST(ColorSpectrumManager, AllocateSpectrumFromReflectiveSpectrum) {
  std::filesystem::path path = std::filesystem::current_path();
  ColorSpectrumManager spectrum_manager(path, true);

  Spectrum sampled_spectrum;
  auto& sample = *sampled_spectrum.mutable_sampled_spectrum()->add_samples();
  sample.set_wavelength(1.0);
  sample.set_intensity(1.0);

  ReferenceCounted<iris::Spectrum> spectrum =
      spectrum_manager.AllocateSpectrum(sampled_spectrum);
  EXPECT_NEAR(1.204, spectrum->Intensity(0.5), 0.1);
  EXPECT_NEAR(0.948, spectrum->Intensity(1.5), 0.1);
  EXPECT_NEAR(0.909, spectrum->Intensity(2.5), 0.1);
}

TEST(ColorSpectrumManager, AllocateSpectrumFromReflectiveSpectrumZero) {
  std::filesystem::path path = std::filesystem::current_path();
  ColorSpectrumManager spectrum_manager(path, true);

  Spectrum sampled_spectrum;
  auto& sample = *sampled_spectrum.mutable_sampled_spectrum()->add_samples();
  sample.set_wavelength(1.0);
  sample.set_intensity(0.0);

  visual_t luma;
  ReferenceCounted<iris::Spectrum> spectrum =
      spectrum_manager.AllocateSpectrum(sampled_spectrum, &luma);
  EXPECT_FALSE(spectrum);
  EXPECT_EQ(0.0, luma);
}

TEST(ColorSpectrumManager, AllocateSpectrumFromFile) {
  std::filesystem::path path = std::filesystem::current_path();
  ColorSpectrumManager spectrum_manager(path, false);

  Spectrum sampled_spectrum;
  sampled_spectrum.set_sampled_spectrum_filename(RunfilePath("emissive.spd"));

  ReferenceCounted<iris::Spectrum> spectrum =
      spectrum_manager.AllocateSpectrum(sampled_spectrum);
  EXPECT_TRUE(spectrum);
}

TEST(ColorSpectrumManager, AllocateReflectorFromUniform) {
  std::filesystem::path path = std::filesystem::current_path();
  ColorSpectrumManager spectrum_manager(path, false);

  Spectrum uniform_spectrum;
  uniform_spectrum.set_uniform_spectrum(0.5);

  ReferenceCounted<Reflector> reflector =
      spectrum_manager.AllocateReflector(uniform_spectrum);
  EXPECT_EQ(0.5, reflector->Reflectance(0.5));
  EXPECT_EQ(0.5, reflector->Reflectance(1.5));
  EXPECT_EQ(0.5, reflector->Reflectance(2.5));
}

TEST(ColorSpectrumManager, AllocateReflectorFromBlackbody) {
  std::filesystem::path path = std::filesystem::current_path();
  ColorSpectrumManager spectrum_manager(path, false);

  Spectrum blackbody_spectrum;
  blackbody_spectrum.mutable_blackbody_spectrum()->set_temperature(65000.0);
  blackbody_spectrum.mutable_blackbody_spectrum()->set_scale(1.0);

  ReferenceCounted<iris::Spectrum> spectrum =
      spectrum_manager.AllocateSpectrum(blackbody_spectrum);
  EXPECT_NEAR(0.07231, spectrum->Intensity(0.5), 0.01);
  EXPECT_NEAR(0.10340, spectrum->Intensity(1.5), 0.01);
  EXPECT_NEAR(0.22361, spectrum->Intensity(2.5), 0.01);
}

TEST(ColorSpectrumManager, AllocateReflectorFromColor) {
  std::filesystem::path path = std::filesystem::current_path();
  ColorSpectrumManager spectrum_manager(path, false);

  Spectrum rgb_spectrum;
  rgb_spectrum.mutable_rgb_spectrum()->set_r(0.25);
  rgb_spectrum.mutable_rgb_spectrum()->set_g(0.5);
  rgb_spectrum.mutable_rgb_spectrum()->set_b(0.75);

  ReferenceCounted<Reflector> reflector =
      spectrum_manager.AllocateReflector(rgb_spectrum);
  EXPECT_EQ(0.25, reflector->Reflectance(0.5));
  EXPECT_EQ(0.50, reflector->Reflectance(1.5));
  EXPECT_EQ(0.75, reflector->Reflectance(2.5));
}

TEST(ColorSpectrumManager, AllocateReflectorFromColorXYZ) {
  std::filesystem::path path = std::filesystem::current_path();
  ColorSpectrumManager spectrum_manager(path, false);

  Spectrum xyz_spectrum;
  xyz_spectrum.mutable_xyz_spectrum()->set_x(0.4);
  xyz_spectrum.mutable_xyz_spectrum()->set_y(0.5);
  xyz_spectrum.mutable_xyz_spectrum()->set_z(0.6);

  ReferenceCounted<Reflector> reflector =
      spectrum_manager.AllocateReflector(xyz_spectrum);
  EXPECT_NEAR(0.228493, reflector->Reflectance(0.5), 0.001);
  EXPECT_NEAR(0.575232, reflector->Reflectance(1.5), 0.001);
  EXPECT_NEAR(0.554579, reflector->Reflectance(2.5), 0.001);
}

TEST(ColorSpectrumManager, AllocateReflectorFromColorZero) {
  std::filesystem::path path = std::filesystem::current_path();
  ColorSpectrumManager spectrum_manager(path, false);

  Spectrum rgb_spectrum;
  rgb_spectrum.mutable_rgb_spectrum()->set_r(0.0);
  rgb_spectrum.mutable_rgb_spectrum()->set_g(0.0);
  rgb_spectrum.mutable_rgb_spectrum()->set_b(0.0);

  ReferenceCounted<Reflector> reflector =
      spectrum_manager.AllocateReflector(rgb_spectrum);
  EXPECT_FALSE(reflector);
}

TEST(ColorSpectrumManager, AllocateReflectorFromSpectrum) {
  std::filesystem::path path = std::filesystem::current_path();
  ColorSpectrumManager spectrum_manager(path, false);

  Spectrum sampled_spectrum;
  auto& sample = *sampled_spectrum.mutable_sampled_spectrum()->add_samples();
  sample.set_wavelength(1.0);
  sample.set_intensity(1.0);

  ReferenceCounted<Reflector> reflector =
      spectrum_manager.AllocateReflector(sampled_spectrum);
  EXPECT_NEAR(1.000, reflector->Reflectance(0.5), 0.1);
  EXPECT_NEAR(0.948, reflector->Reflectance(1.5), 0.1);
  EXPECT_NEAR(0.909, reflector->Reflectance(2.5), 0.1);
}

TEST(ColorSpectrumManager, AllocateReflectorFromSpectrumZero) {
  std::filesystem::path path = std::filesystem::current_path();
  ColorSpectrumManager spectrum_manager(path, false);

  Spectrum sampled_spectrum;
  auto& sample = *sampled_spectrum.mutable_sampled_spectrum()->add_samples();
  sample.set_wavelength(1.0);
  sample.set_intensity(0.0);

  ReferenceCounted<Reflector> reflector =
      spectrum_manager.AllocateReflector(sampled_spectrum);
  EXPECT_FALSE(reflector);
}

TEST(ColorSpectrumManager, AllocateReflectorFromFile) {
  std::filesystem::path path = std::filesystem::current_path();
  ColorSpectrumManager spectrum_manager(path, false);

  Spectrum sampled_spectrum;
  sampled_spectrum.set_sampled_spectrum_filename(RunfilePath("reflective.spd"));

  ReferenceCounted<Reflector> reflector =
      spectrum_manager.AllocateReflector(sampled_spectrum);
  EXPECT_TRUE(reflector);
}

TEST(ColorSpectrumManager, ScaleSpectrumBothNull) {
  std::filesystem::path path = std::filesystem::current_path();
  ColorSpectrumManager spectrum_manager(path, false);

  visual_t luma;
  EXPECT_FALSE(spectrum_manager.AllocateSpectrum(
      ReferenceCounted<iris::Spectrum>(), ReferenceCounted<iris::Spectrum>(),
      &luma));
  EXPECT_EQ(0.0, luma);
}

TEST(ColorSpectrumManager, ScaleSpectrumFirstNull) {
  std::filesystem::path path = std::filesystem::current_path();
  ColorSpectrumManager spectrum_manager(path, false);

  Spectrum rgb_spectrum;
  rgb_spectrum.mutable_rgb_spectrum()->set_r(1.0);
  rgb_spectrum.mutable_rgb_spectrum()->set_g(1.0);
  rgb_spectrum.mutable_rgb_spectrum()->set_b(1.0);

  visual_t luma;
  EXPECT_FALSE(spectrum_manager.AllocateSpectrum(
      ReferenceCounted<iris::Spectrum>(),
      spectrum_manager.AllocateSpectrum(rgb_spectrum), &luma));
  EXPECT_EQ(0.0, luma);
}

TEST(ColorSpectrumManager, ScaleSpectrumSecondNull) {
  std::filesystem::path path = std::filesystem::current_path();
  ColorSpectrumManager spectrum_manager(path, false);

  Spectrum rgb_spectrum;
  rgb_spectrum.mutable_rgb_spectrum()->set_r(1.0);
  rgb_spectrum.mutable_rgb_spectrum()->set_g(1.0);
  rgb_spectrum.mutable_rgb_spectrum()->set_b(1.0);

  visual_t luma;
  EXPECT_FALSE(spectrum_manager.AllocateSpectrum(
      spectrum_manager.AllocateSpectrum(rgb_spectrum),
      ReferenceCounted<iris::Spectrum>(), &luma));
  EXPECT_EQ(0.0, luma);
}

TEST(ColorSpectrumManager, ScaleSpectrumFirstOne) {
  std::filesystem::path path = std::filesystem::current_path();
  ColorSpectrumManager spectrum_manager(path, false);

  Spectrum rgb_spectrum0;
  rgb_spectrum0.mutable_rgb_spectrum()->set_r(1.0);
  rgb_spectrum0.mutable_rgb_spectrum()->set_g(1.0);
  rgb_spectrum0.mutable_rgb_spectrum()->set_b(1.0);

  Spectrum rgb_spectrum1;
  rgb_spectrum1.mutable_rgb_spectrum()->set_r(0.5);
  rgb_spectrum1.mutable_rgb_spectrum()->set_g(0.5);
  rgb_spectrum1.mutable_rgb_spectrum()->set_b(0.5);

  ReferenceCounted<iris::Spectrum> one =
      spectrum_manager.AllocateSpectrum(rgb_spectrum0);

  visual_t expected_luma;
  ReferenceCounted<iris::Spectrum> half =
      spectrum_manager.AllocateSpectrum(rgb_spectrum1, &expected_luma);

  visual_t luma;
  EXPECT_EQ(half.Get(),
            spectrum_manager.AllocateSpectrum(one, half, &luma).Get());
  EXPECT_EQ(expected_luma, luma);
}

TEST(ColorSpectrumManager, ScaleSpectrumSecondOne) {
  std::filesystem::path path = std::filesystem::current_path();
  ColorSpectrumManager spectrum_manager(path, false);

  Spectrum rgb_spectrum0;
  rgb_spectrum0.mutable_rgb_spectrum()->set_r(1.0);
  rgb_spectrum0.mutable_rgb_spectrum()->set_g(1.0);
  rgb_spectrum0.mutable_rgb_spectrum()->set_b(1.0);

  Spectrum rgb_spectrum1;
  rgb_spectrum1.mutable_rgb_spectrum()->set_r(0.5);
  rgb_spectrum1.mutable_rgb_spectrum()->set_g(0.5);
  rgb_spectrum1.mutable_rgb_spectrum()->set_b(0.5);

  ReferenceCounted<iris::Spectrum> one =
      spectrum_manager.AllocateSpectrum(rgb_spectrum0);

  visual_t expected_luma;
  ReferenceCounted<iris::Spectrum> half =
      spectrum_manager.AllocateSpectrum(rgb_spectrum1, &expected_luma);

  visual_t luma;
  EXPECT_EQ(half.Get(),
            spectrum_manager.AllocateSpectrum(half, one, &luma).Get());
  EXPECT_EQ(expected_luma, luma);
}

TEST(ColorSpectrumManager, ScaleSpectrum) {
  std::filesystem::path path = std::filesystem::current_path();
  ColorSpectrumManager spectrum_manager(path, false);

  Spectrum rgb_spectrum0;
  rgb_spectrum0.mutable_rgb_spectrum()->set_r(1.0);
  rgb_spectrum0.mutable_rgb_spectrum()->set_g(2.0);
  rgb_spectrum0.mutable_rgb_spectrum()->set_b(3.0);

  Spectrum rgb_spectrum1;
  rgb_spectrum1.mutable_rgb_spectrum()->set_r(4.0);
  rgb_spectrum1.mutable_rgb_spectrum()->set_g(5.0);
  rgb_spectrum1.mutable_rgb_spectrum()->set_b(6.0);

  ReferenceCounted<iris::Spectrum> first =
      spectrum_manager.AllocateSpectrum(rgb_spectrum0);
  ReferenceCounted<iris::Spectrum> second =
      spectrum_manager.AllocateSpectrum(rgb_spectrum1);

  visual_t luma;
  ReferenceCounted<iris::Spectrum> scaled =
      spectrum_manager.AllocateSpectrum(first, second, &luma);
  ASSERT_TRUE(scaled);
  EXPECT_EQ(4.0, scaled->Intensity(0.5));
  EXPECT_EQ(10.0, scaled->Intensity(1.5));
  EXPECT_EQ(18.0, scaled->Intensity(2.5));
  EXPECT_NEAR(9.30136, luma, 0.1);
}

}  // namespace
}  // namespace spectrum_managers
}  // namespace pbrt_frontend
}  // namespace iris
