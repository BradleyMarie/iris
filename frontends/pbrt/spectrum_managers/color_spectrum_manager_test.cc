#include "frontends/pbrt/spectrum_managers/color_spectrum_manager.h"

#include "frontends/pbrt/spectrum_managers/internal/color_spectrum.h"
#include "googletest/include/gtest/gtest.h"

namespace iris {
namespace pbrt_frontend {
namespace spectrum_managers {
namespace {

static ColorColorMatcher g_color_matcher;
static ColorAlbedoMatcher g_albedo_matcher;
static ColorPowerMatcher g_power_matcher;
static ColorSpectrumManager g_spectrum_manager(false);

TEST(ColorColorMatcher, Match) {
  Color color({0.25, 0.5, 0.75}, Color::RGB);
  internal::ColorSpectrum spectrum(color);
  std::array<visual_t, 3> values = g_color_matcher.Match(spectrum);
  EXPECT_EQ(0.25, values[0]);
  EXPECT_EQ(0.50, values[1]);
  EXPECT_EQ(0.75, values[2]);
}

TEST(ColorColorMatcher, ColorSpace) {
  EXPECT_EQ(iris::Color::LINEAR_SRGB, g_color_matcher.ColorSpace());
}

TEST(ColorAlbedoMatcher, Match) {
  Color color({0.6, 0.5, 0.4}, Color::XYZ);
  ReferenceCounted<Reflector> reflector =
      g_spectrum_manager.AllocateReflector(color);
  EXPECT_NEAR(0.5, g_albedo_matcher.Match(*reflector), 0.001);
}

TEST(ColorPowerMatcher, Match) {
  Color color({0.75, 0.5, 0.25}, Color::XYZ);
  ReferenceCounted<Spectrum> spectrum =
      g_spectrum_manager.AllocateSpectrum(color);
  EXPECT_NEAR(0.5, g_power_matcher.Match(*spectrum), 0.001);
}

TEST(ColorSpectrumManager, ComputeLuma) {
  Color color({0.25, 0.5, 0.75}, Color::XYZ);
  EXPECT_EQ(0.5, g_spectrum_manager.ComputeLuma(color));
}

TEST(ColorSpectrumManager, AllocateSpectrumFromColor) {
  Color color({0.25, 0.5, 0.75}, Color::RGB);
  ReferenceCounted<Spectrum> spectrum =
      g_spectrum_manager.AllocateSpectrum(color);
  EXPECT_EQ(0.25, spectrum->Intensity(0.5));
  EXPECT_EQ(0.50, spectrum->Intensity(1.5));
  EXPECT_EQ(0.75, spectrum->Intensity(2.5));
}

TEST(ColorSpectrumManager, AllocateSpectrumFromColorXYZ) {
  Color color({0.25, 0.5, 0.75}, Color::XYZ);
  visual_t luma;
  ReferenceCounted<Spectrum> spectrum =
      g_spectrum_manager.AllocateSpectrum(color, &luma);
  EXPECT_NEAR(0.0, spectrum->Intensity(0.5), 0.001);
  EXPECT_NEAR(0.726856, spectrum->Intensity(1.5), 0.001);
  EXPECT_NEAR(0.704817, spectrum->Intensity(2.5), 0.001);
  EXPECT_EQ(0.5, luma);
}

TEST(ColorSpectrumManager, AllocateSpectrumFromColorZero) {
  Color color({0.0, 0.0, 0.0}, Color::RGB);
  ReferenceCounted<Spectrum> spectrum =
      g_spectrum_manager.AllocateSpectrum(color);
  EXPECT_FALSE(spectrum);
}

TEST(ColorSpectrumManager, AllocateSpectrumFromSpectrum) {
  ReferenceCounted<Spectrum> spectrum =
      g_spectrum_manager.AllocateSpectrum({{1.0, 1.0}});
  EXPECT_NEAR(128.75, spectrum->Intensity(0.5), 0.001);
  EXPECT_NEAR(101.325, spectrum->Intensity(1.5), 0.001);
  EXPECT_NEAR(97.154, spectrum->Intensity(2.5), 0.001);
}

TEST(ColorSpectrumManager, AllocateSpectrumFromSpectrumZero) {
  ReferenceCounted<Spectrum> spectrum =
      g_spectrum_manager.AllocateSpectrum({{1.0, 0.0}});
  EXPECT_FALSE(spectrum);
}

TEST(ColorSpectrumManager, AllocateSpectrumFromReflectiveSpectrum) {
  ColorSpectrumManager spectrum_manager(true);
  ReferenceCounted<Spectrum> spectrum =
      spectrum_manager.AllocateSpectrum({{1.0, 1.0}});
  EXPECT_NEAR(1.204, spectrum->Intensity(0.5), 0.1);
  EXPECT_NEAR(0.948, spectrum->Intensity(1.5), 0.1);
  EXPECT_NEAR(0.909, spectrum->Intensity(2.5), 0.1);
}

TEST(ColorSpectrumManager, AllocateSpectrumFromReflectiveSpectrumZero) {
  ColorSpectrumManager spectrum_manager(true);
  ReferenceCounted<Spectrum> spectrum =
      spectrum_manager.AllocateSpectrum({{1.0, 0.0}});
  EXPECT_FALSE(spectrum);
}

TEST(ColorSpectrumManager, ScaleSpectrumBothNull) {
  visual_t luma;
  EXPECT_FALSE(g_spectrum_manager.AllocateSpectrum(
      ReferenceCounted<Spectrum>(), ReferenceCounted<Spectrum>(), &luma));
  EXPECT_EQ(0.0, luma);
}

TEST(ColorSpectrumManager, ScaleSpectrumFirstNull) {
  visual_t luma;
  EXPECT_FALSE(g_spectrum_manager.AllocateSpectrum(
      ReferenceCounted<Spectrum>(),
      g_spectrum_manager.AllocateSpectrum({{1.0, 1.0}}), &luma));
  EXPECT_EQ(0.0, luma);
}

TEST(ColorSpectrumManager, ScaleSpectrumSecondNull) {
  visual_t luma;
  EXPECT_FALSE(g_spectrum_manager.AllocateSpectrum(
      g_spectrum_manager.AllocateSpectrum({{1.0, 1.0}}),
      ReferenceCounted<Spectrum>(), &luma));
  EXPECT_EQ(0.0, luma);
}

TEST(ColorSpectrumManager, ScaleSpectrumFirstOne) {
  ReferenceCounted<Spectrum> one =
      g_spectrum_manager.AllocateSpectrum(Color({1.0, 1.0, 1.0}, Color::RGB));
  visual_t expected_luma;
  ReferenceCounted<Spectrum> half = g_spectrum_manager.AllocateSpectrum(
      Color({0.5, 0.5, 0.5}, Color::RGB), &expected_luma);
  visual_t luma;
  EXPECT_EQ(half.Get(),
            g_spectrum_manager.AllocateSpectrum(one, half, &luma).Get());
  EXPECT_EQ(expected_luma, luma);
}

TEST(ColorSpectrumManager, ScaleSpectrumSecondOne) {
  ReferenceCounted<Spectrum> one =
      g_spectrum_manager.AllocateSpectrum(Color({1.0, 1.0, 1.0}, Color::RGB));
  visual_t expected_luma;
  ReferenceCounted<Spectrum> half = g_spectrum_manager.AllocateSpectrum(
      Color({0.5, 0.5, 0.5}, Color::RGB), &expected_luma);
  visual_t luma;
  EXPECT_EQ(half.Get(),
            g_spectrum_manager.AllocateSpectrum(half, one, &luma).Get());
  EXPECT_EQ(expected_luma, luma);
}

TEST(ColorSpectrumManager, ScaleSpectrum) {
  ReferenceCounted<Spectrum> first =
      g_spectrum_manager.AllocateSpectrum(Color({1.0, 2.0, 3.0}, Color::RGB));
  ReferenceCounted<Spectrum> second =
      g_spectrum_manager.AllocateSpectrum(Color({4.0, 5.0, 6.0}, Color::RGB));
  visual_t luma;
  ReferenceCounted<Spectrum> scaled =
      g_spectrum_manager.AllocateSpectrum(first, second, &luma);
  ASSERT_TRUE(scaled);
  EXPECT_EQ(4.0, scaled->Intensity(0.5));
  EXPECT_EQ(10.0, scaled->Intensity(1.5));
  EXPECT_EQ(18.0, scaled->Intensity(2.5));
  EXPECT_NEAR(9.30136, luma, 0.1);
}

TEST(ColorSpectrumManager, AllocateReflectorFromColor) {
  Color color({0.25, 0.5, 0.75}, Color::RGB);
  ReferenceCounted<Reflector> reflector =
      g_spectrum_manager.AllocateReflector(color);
  EXPECT_EQ(0.25, reflector->Reflectance(0.5));
  EXPECT_EQ(0.50, reflector->Reflectance(1.5));
  EXPECT_EQ(0.75, reflector->Reflectance(2.5));
}

TEST(ColorSpectrumManager, AllocateReflectorFromColorXYZ) {
  Color color({0.25, 0.5, 0.75}, Color::XYZ);
  ReferenceCounted<Reflector> reflector =
      g_spectrum_manager.AllocateReflector(color);
  EXPECT_NEAR(0.0, reflector->Reflectance(0.5), 0.001);
  EXPECT_NEAR(0.726856, reflector->Reflectance(1.5), 0.001);
  EXPECT_NEAR(0.704817, reflector->Reflectance(2.5), 0.001);
}

TEST(ColorSpectrumManager, AllocateReflectorFromColorZero) {
  Color color({0.0, 0.0, 0.0}, Color::RGB);
  ReferenceCounted<Reflector> reflector =
      g_spectrum_manager.AllocateReflector(color);
  EXPECT_FALSE(reflector);
}

TEST(ColorSpectrumManager, AllocateReflectorFromSpectrum) {
  ReferenceCounted<Reflector> reflector =
      g_spectrum_manager.AllocateReflector({{1.0, 1.0}});
  EXPECT_NEAR(1.000, reflector->Reflectance(0.5), 0.001);
  EXPECT_NEAR(0.948, reflector->Reflectance(1.5), 0.001);
  EXPECT_NEAR(0.909, reflector->Reflectance(2.5), 0.001);
}

TEST(ColorSpectrumManager, AllocateReflectorFromSpectrumZero) {
  ReferenceCounted<Reflector> reflector =
      g_spectrum_manager.AllocateReflector({{1.0, 0.0}});
  EXPECT_FALSE(reflector);
}

}  // namespace
}  // namespace spectrum_managers
}  // namespace pbrt_frontend
}  // namespace iris