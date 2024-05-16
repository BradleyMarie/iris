#include "frontends/pbrt/spectrum_managers/color_spectrum_manager.h"

#include "frontends/pbrt/spectrum_managers/internal/color_spectrum.h"
#include "googletest/include/gtest/gtest.h"

static iris::pbrt_frontend::spectrum_managers::ColorColorMatcher
    g_color_matcher;
static iris::pbrt_frontend::spectrum_managers::ColorSpectrumManager
    g_spectrum_manager(false);

TEST(ColorColorMatcher, Match) {
  iris::pbrt_frontend::Color color({0.25, 0.5, 0.75},
                                   iris::pbrt_frontend::Color::RGB);
  iris::pbrt_frontend::spectrum_managers::internal::ColorSpectrum spectrum(
      color);
  auto values = g_color_matcher.Match(spectrum);
  EXPECT_EQ(0.25, values[0]);
  EXPECT_EQ(0.50, values[1]);
  EXPECT_EQ(0.75, values[2]);
}

TEST(ColorColorMatcher, ColorSpace) {
  EXPECT_EQ(iris::Color::LINEAR_SRGB, g_color_matcher.ColorSpace());
}

TEST(ColorSpectrumManager, AllocateSpectrumFromColor) {
  iris::pbrt_frontend::Color color({0.25, 0.5, 0.75},
                                   iris::pbrt_frontend::Color::RGB);
  auto spectrum = g_spectrum_manager.AllocateSpectrum(color);
  EXPECT_EQ(0.25, spectrum->Intensity(0.5));
  EXPECT_EQ(0.50, spectrum->Intensity(1.5));
  EXPECT_EQ(0.75, spectrum->Intensity(2.5));
}

TEST(ColorSpectrumManager, AllocateSpectrumFromColorXYZ) {
  iris::pbrt_frontend::Color color({0.25, 0.5, 0.75},
                                   iris::pbrt_frontend::Color::XYZ);
  iris::visual_t luma;
  auto spectrum = g_spectrum_manager.AllocateSpectrum(color, &luma);
  EXPECT_NEAR(0.0, spectrum->Intensity(0.5), 0.001);
  EXPECT_NEAR(0.726856, spectrum->Intensity(1.5), 0.001);
  EXPECT_NEAR(0.704817, spectrum->Intensity(2.5), 0.001);
  EXPECT_EQ(0.5, luma);
}

TEST(ColorSpectrumManager, AllocateSpectrumFromColorZero) {
  iris::pbrt_frontend::Color color({0.0, 0.0, 0.0},
                                   iris::pbrt_frontend::Color::RGB);
  auto spectrum = g_spectrum_manager.AllocateSpectrum(color);
  EXPECT_FALSE(spectrum);
}

TEST(ColorSpectrumManager, AllocateSpectrumFromSpectrum) {
  auto spectrum = g_spectrum_manager.AllocateSpectrum(
      {{static_cast<iris::visual>(1.0), static_cast<iris::visual>(1.0)}});
  EXPECT_NEAR(128.75, spectrum->Intensity(0.5), 0.001);
  EXPECT_NEAR(101.325, spectrum->Intensity(1.5), 0.001);
  EXPECT_NEAR(97.154, spectrum->Intensity(2.5), 0.001);
}

TEST(ColorSpectrumManager, AllocateSpectrumFromSpectrumZero) {
  auto spectrum = g_spectrum_manager.AllocateSpectrum(
      {{static_cast<iris::visual>(1.0), static_cast<iris::visual>(0.0)}});
  EXPECT_FALSE(spectrum);
}

TEST(ColorSpectrumManager, AllocateSpectrumFromReflectiveSpectrum) {
  iris::pbrt_frontend::spectrum_managers::ColorSpectrumManager spectrum_manager(
      true);
  auto spectrum = spectrum_manager.AllocateSpectrum(
      {{static_cast<iris::visual>(1.0), static_cast<iris::visual>(1.0)}});
  EXPECT_NEAR(1.204, spectrum->Intensity(0.5), 0.1);
  EXPECT_NEAR(0.948, spectrum->Intensity(1.5), 0.1);
  EXPECT_NEAR(0.909, spectrum->Intensity(2.5), 0.1);
}

TEST(ColorSpectrumManager, AllocateSpectrumFromReflectiveSpectrumZero) {
  iris::pbrt_frontend::spectrum_managers::ColorSpectrumManager spectrum_manager(
      true);
  auto spectrum = spectrum_manager.AllocateSpectrum(
      {{static_cast<iris::visual>(1.0), static_cast<iris::visual>(0.0)}});
  EXPECT_FALSE(spectrum);
}

TEST(ColorSpectrumManager, AllocateReflectorFromColor) {
  iris::pbrt_frontend::Color color({0.25, 0.5, 0.75},
                                   iris::pbrt_frontend::Color::RGB);
  auto reflector = g_spectrum_manager.AllocateReflector(color);
  EXPECT_EQ(0.25, reflector->Reflectance(0.5));
  EXPECT_EQ(0.50, reflector->Reflectance(1.5));
  EXPECT_EQ(0.75, reflector->Reflectance(2.5));
}

TEST(ColorSpectrumManager, AllocateReflectorFromColorXYZ) {
  iris::pbrt_frontend::Color color({0.25, 0.5, 0.75},
                                   iris::pbrt_frontend::Color::XYZ);
  auto reflector = g_spectrum_manager.AllocateReflector(color);
  EXPECT_NEAR(0.0, reflector->Reflectance(0.5), 0.001);
  EXPECT_NEAR(0.726856, reflector->Reflectance(1.5), 0.001);
  EXPECT_NEAR(0.704817, reflector->Reflectance(2.5), 0.001);
}

TEST(ColorSpectrumManager, AllocateReflectorFromColorZero) {
  iris::pbrt_frontend::Color color({0.0, 0.0, 0.0},
                                   iris::pbrt_frontend::Color::RGB);
  auto reflector = g_spectrum_manager.AllocateReflector(color);
  EXPECT_FALSE(reflector);
}

TEST(ColorSpectrumManager, AllocateReflectorFromSpectrum) {
  auto reflector = g_spectrum_manager.AllocateReflector(
      {{static_cast<iris::visual>(1.0), static_cast<iris::visual>(1.0)}});
  EXPECT_NEAR(1.000, reflector->Reflectance(0.5), 0.001);
  EXPECT_NEAR(0.948, reflector->Reflectance(1.5), 0.001);
  EXPECT_NEAR(0.909, reflector->Reflectance(2.5), 0.001);
}

TEST(ColorSpectrumManager, AllocateReflectorFromSpectrumZero) {
  auto reflector = g_spectrum_manager.AllocateReflector(
      {{static_cast<iris::visual>(1.0), static_cast<iris::visual>(0.0)}});
  EXPECT_FALSE(reflector);
}