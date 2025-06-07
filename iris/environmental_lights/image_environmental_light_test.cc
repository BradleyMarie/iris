#include "iris/environmental_lights/image_environmental_light.h"

#include <numbers>

#include "googletest/include/gtest/gtest.h"
#include "iris/power_matchers/mock_power_matcher.h"
#include "iris/random/mock_random.h"
#include "iris/spectra/mock_spectrum.h"
#include "iris/testing/spectral_allocator.h"

namespace iris {
namespace environmental_lights {
namespace {

using ::iris::power_matchers::MockPowerMatcher;
using ::iris::spectra::MockSpectrum;
using ::iris::testing::GetSpectralAllocator;
using ::testing::_;
using ::testing::Return;

ReferenceCounted<Spectrum> MakeUniformSpectrum(visual_t value) {
  ReferenceCounted<MockSpectrum> result = MakeReferenceCounted<MockSpectrum>();
  EXPECT_CALL(*result, Intensity(_)).WillRepeatedly(Return(value));
  return result;
}

static const ReferenceCounted<Spectrum> kSpectrum0;
static const ReferenceCounted<Spectrum> kSpectrum1 = MakeUniformSpectrum(1.0);
static const ReferenceCounted<Spectrum> kSpectrum2 = MakeUniformSpectrum(2.0);

TEST(ImageEnvironmentalLight, Null) {
  EXPECT_TRUE(MakeImageEnvironmentalLight({{kSpectrum1, 1.0}}, {1, 1},
                                          Matrix::Identity()));
  EXPECT_FALSE(MakeImageEnvironmentalLight({{kSpectrum1, 0.0}}, {1, 1},
                                           Matrix::Identity()));
  EXPECT_FALSE(MakeImageEnvironmentalLight({{kSpectrum0, 1.0}}, {1, 1},
                                           Matrix::Identity()));
  EXPECT_FALSE(MakeImageEnvironmentalLight({{kSpectrum0, 0.0}}, {1, 1},
                                           Matrix::Identity()));
}

TEST(ImageEnvironmentalLight, SampleOne) {
  std::vector<std::pair<ReferenceCounted<Spectrum>, visual>> spectra_and_luma =
      {{kSpectrum0, 0.0},
       {kSpectrum0, 0.0},
       {kSpectrum1, 1.0},
       {kSpectrum2, 2.0}};
  std::pair<size_t, size_t> size(2, 2);
  ReferenceCounted<EnvironmentalLight> light =
      MakeImageEnvironmentalLight(spectra_and_luma, size, Matrix::Identity());

  random::MockRandom rng;
  EXPECT_CALL(rng, NextGeometric()).WillRepeatedly(Return(0.25));
  Sampler sampler(rng);

  std::optional<EnvironmentalLight::SampleResult> result =
      light->Sample(sampler, GetSpectralAllocator());
  ASSERT_TRUE(result);
  EXPECT_EQ(1.0, result->emission.Intensity(1.0));
  EXPECT_NEAR(0.07311284, result->pdf, 0.001);
  EXPECT_NEAR(-0.6532813, result->to_light.x, 0.001);
  EXPECT_NEAR(+0.6532815, result->to_light.y, 0.001);
  EXPECT_NEAR(-0.3826835, result->to_light.z, 0.001);
}

TEST(ImageEnvironmentalLight, SampleTwo) {
  std::vector<std::pair<ReferenceCounted<Spectrum>, visual>> spectra_and_luma =
      {{kSpectrum0, 0.0},
       {kSpectrum0, 0.0},
       {kSpectrum1, 1.0},
       {kSpectrum2, 2.0}};
  std::pair<size_t, size_t> size(2, 2);
  ReferenceCounted<EnvironmentalLight> light =
      MakeImageEnvironmentalLight(spectra_and_luma, size, Matrix::Identity());

  random::MockRandom rng;
  EXPECT_CALL(rng, NextGeometric()).WillRepeatedly(Return(0.75));
  Sampler sampler(rng);

  std::optional<EnvironmentalLight::SampleResult> result =
      light->Sample(sampler, GetSpectralAllocator());
  ASSERT_TRUE(result);
  EXPECT_EQ(2.0, result->emission.Intensity(1.0));
  EXPECT_NEAR(0.35302019, result->pdf, 0.001);
  EXPECT_NEAR(+0.1464466, result->to_light.x, 0.001);
  EXPECT_NEAR(-0.3535532, result->to_light.y, 0.001);
  EXPECT_NEAR(-0.9238796, result->to_light.z, 0.001);
}

TEST(ImageEnvironmentalLight, EmissionZero) {
  std::vector<std::pair<ReferenceCounted<Spectrum>, visual>> spectra_and_luma =
      {{kSpectrum0, 0.0},
       {kSpectrum0, 0.0},
       {kSpectrum1, 1.0},
       {kSpectrum2, 2.0}};
  std::pair<size_t, size_t> size(2, 2);
  ReferenceCounted<EnvironmentalLight> light =
      MakeImageEnvironmentalLight(spectra_and_luma, size, Matrix::Identity());

  const Spectrum* result = light->Emission(Normalize(Vector(1.0, 1.0, 1.0)),
                                           GetSpectralAllocator(), nullptr);
  EXPECT_FALSE(result);
}

TEST(ImageEnvironmentalLight, EmissionOne) {
  std::vector<std::pair<ReferenceCounted<Spectrum>, visual>> spectra_and_luma =
      {{kSpectrum0, 0.0},
       {kSpectrum0, 0.0},
       {kSpectrum1, 1.0},
       {kSpectrum2, 2.0}};
  std::pair<size_t, size_t> size(2, 2);
  ReferenceCounted<EnvironmentalLight> light =
      MakeImageEnvironmentalLight(spectra_and_luma, size, Matrix::Identity());

  visual_t pdf;
  const Spectrum* result = light->Emission(Normalize(Vector(1.0, 1.0, -1.0)),
                                           GetSpectralAllocator(), &pdf);
  ASSERT_TRUE(result);
  EXPECT_EQ(1.0, result->Intensity(1.0));
  EXPECT_NEAR(0.08272840, pdf, 0.001);
}

TEST(ImageEnvironmentalLight, EmissionTwo) {
  std::vector<std::pair<ReferenceCounted<Spectrum>, visual>> spectra_and_luma =
      {{kSpectrum0, 0.0},
       {kSpectrum0, 0.0},
       {kSpectrum1, 1.0},
       {kSpectrum2, 2.0}};
  std::pair<size_t, size_t> size(2, 2);
  ReferenceCounted<EnvironmentalLight> light =
      MakeImageEnvironmentalLight(spectra_and_luma, size, Matrix::Identity());

  visual_t pdf;
  const Spectrum* result = light->Emission(Normalize(Vector(-1.0, -1.0, -1.0)),
                                           GetSpectralAllocator(), &pdf);
  ASSERT_TRUE(result);
  EXPECT_EQ(2.0, result->Intensity(1.0));
  EXPECT_NEAR(0.16545681, pdf, 0.001);
}

TEST(ImageEnvironmentalLight, Power1x1) {
  std::vector<std::pair<ReferenceCounted<Spectrum>, visual>> spectra_and_luma =
      {{kSpectrum1, 1.0}};
  std::pair<size_t, size_t> size(1, 1);
  ReferenceCounted<EnvironmentalLight> light =
      MakeImageEnvironmentalLight(spectra_and_luma, size, Matrix::Identity());

  MockPowerMatcher power_matcher;
  EXPECT_NEAR(4.0 * std::numbers::pi, light->Power(power_matcher, 1.0), 0.001);
}

TEST(ImageEnvironmentalLight, Power2x2) {
  std::vector<std::pair<ReferenceCounted<Spectrum>, visual>> spectra_and_luma =
      {{kSpectrum0, 1.0},
       {kSpectrum0, 1.0},
       {kSpectrum1, 1.0},
       {kSpectrum2, 1.0}};
  std::pair<size_t, size_t> size(2, 2);
  ReferenceCounted<EnvironmentalLight> light =
      MakeImageEnvironmentalLight(spectra_and_luma, size, Matrix::Identity());

  MockPowerMatcher power_matcher;
  EXPECT_NEAR(4.0 * std::numbers::pi, light->Power(power_matcher, 1.0), 0.001);
}

TEST(ImageEnvironmentalLight, Power2x2Quarter) {
  std::vector<std::pair<ReferenceCounted<Spectrum>, visual>> spectra_and_luma =
      {{kSpectrum0, 0.0},
       {kSpectrum0, 0.0},
       {kSpectrum1, 0.0},
       {kSpectrum2, 1.0}};
  std::pair<size_t, size_t> size(2, 2);
  ReferenceCounted<EnvironmentalLight> light =
      MakeImageEnvironmentalLight(spectra_and_luma, size, Matrix::Identity());

  MockPowerMatcher power_matcher;
  EXPECT_NEAR(std::numbers::pi, light->Power(power_matcher, 1.0), 0.001);
}

TEST(ImageEnvironmentalLight, Power2x2Half) {
  std::vector<std::pair<ReferenceCounted<Spectrum>, visual>> spectra_and_luma =
      {{kSpectrum0, 0.0},
       {kSpectrum0, 0.0},
       {kSpectrum1, 1.0},
       {kSpectrum2, 2.0}};
  std::pair<size_t, size_t> size(2, 2);
  ReferenceCounted<EnvironmentalLight> light =
      MakeImageEnvironmentalLight(spectra_and_luma, size, Matrix::Identity());

  MockPowerMatcher power_matcher;
  EXPECT_NEAR(3.0 * std::numbers::pi, light->Power(power_matcher, 1.0), 0.001);
}

}  // namespace
}  // namespace environmental_lights
}  // namespace iris
