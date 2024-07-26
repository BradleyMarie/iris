#include "iris/environmental_lights/image_environmental_light.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/random/mock_random.h"
#include "iris/spectra/mock_spectrum.h"
#include "iris/testing/spectral_allocator.h"

iris::ReferenceCounted<iris::Spectrum> MakeUniformSpectrum(
    iris::visual_t value) {
  auto result = iris::MakeReferenceCounted<iris::spectra::MockSpectrum>();
  EXPECT_CALL(*result, Intensity(testing::_))
      .WillRepeatedly(testing::Return(value));
  return result;
}

static const iris::ReferenceCounted<iris::Spectrum> kSpectrum0;
static const iris::ReferenceCounted<iris::Spectrum> kSpectrum1 =
    MakeUniformSpectrum(1.0);
static const iris::ReferenceCounted<iris::Spectrum> kSpectrum2 =
    MakeUniformSpectrum(2.0);
static const iris::ReferenceCounted<iris::Spectrum> kScalar =
    MakeUniformSpectrum(2.0);

TEST(ImageEnvironmentalLight, SampleTwo) {
  std::vector<iris::ReferenceCounted<iris::Spectrum>> spectra = {
      kSpectrum0, kSpectrum0, kSpectrum1, kSpectrum2};
  std::vector<iris::visual> luma = {0.0, 0.0, 1.0, 2.0};
  std::pair<size_t, size_t> size(2, 2);
  iris::environmental_lights::ImageEnvironmentalLight light(
      spectra, luma, size, kScalar, iris::Matrix::Identity());

  iris::random::MockRandom rng;
  EXPECT_CALL(rng, NextGeometric())
      .WillRepeatedly(testing::Return(static_cast<iris::visual_t>(0.25)));
  iris::Sampler sampler(rng);

  auto result = light.Sample(sampler, iris::testing::GetSpectralAllocator());
  ASSERT_TRUE(result);
  EXPECT_EQ(2.0, result->emission.Intensity(1.0));
  EXPECT_NEAR(0.07311284, result->pdf, 0.001);
  EXPECT_NEAR(-0.6532813, result->to_light.x, 0.001);
  EXPECT_NEAR(+0.6532815, result->to_light.y, 0.001);
  EXPECT_NEAR(-0.3826835, result->to_light.z, 0.001);
}

TEST(ImageEnvironmentalLight, SampleFour) {
  std::vector<iris::ReferenceCounted<iris::Spectrum>> spectra = {
      kSpectrum0, kSpectrum0, kSpectrum1, kSpectrum2};
  std::vector<iris::visual> luma = {0.0, 0.0, 1.0, 2.0};
  std::pair<size_t, size_t> size(2, 2);
  iris::environmental_lights::ImageEnvironmentalLight light(
      spectra, luma, size, kScalar, iris::Matrix::Identity());

  iris::random::MockRandom rng;
  EXPECT_CALL(rng, NextGeometric()).WillRepeatedly(testing::Return(0.75));
  iris::Sampler sampler(rng);

  auto result = light.Sample(sampler, iris::testing::GetSpectralAllocator());
  ASSERT_TRUE(result);
  EXPECT_EQ(4.0, result->emission.Intensity(1.0));
  EXPECT_NEAR(0.35302019, result->pdf, 0.001);
  EXPECT_NEAR(+0.1464466, result->to_light.x, 0.001);
  EXPECT_NEAR(-0.3535532, result->to_light.y, 0.001);
  EXPECT_NEAR(-0.9238796, result->to_light.z, 0.001);
}

TEST(ImageEnvironmentalLight, EmissionZero) {
  std::vector<iris::ReferenceCounted<iris::Spectrum>> spectra = {
      kSpectrum0, kSpectrum0, kSpectrum1, kSpectrum2};
  std::vector<iris::visual> luma = {0.0, 0.0, 1.0, 2.0};
  std::pair<size_t, size_t> size(2, 2);
  iris::environmental_lights::ImageEnvironmentalLight light(
      spectra, luma, size, kScalar, iris::Matrix::Identity());

  auto result = light.Emission(iris::Normalize(iris::Vector(1.0, 1.0, 1.0)),
                               iris::testing::GetSpectralAllocator(), nullptr);
  EXPECT_FALSE(result);
}

TEST(ImageEnvironmentalLight, EmissionOne) {
  std::vector<iris::ReferenceCounted<iris::Spectrum>> spectra = {
      kSpectrum0, kSpectrum0, kSpectrum1, kSpectrum2};
  std::vector<iris::visual> luma = {0.0, 0.0, 1.0, 2.0};
  std::pair<size_t, size_t> size(2, 2);
  iris::environmental_lights::ImageEnvironmentalLight light(
      spectra, luma, size, kScalar, iris::Matrix::Identity());

  iris::visual_t pdf;
  auto result = light.Emission(iris::Normalize(iris::Vector(1.0, 1.0, -1.0)),
                               iris::testing::GetSpectralAllocator(), &pdf);
  ASSERT_TRUE(result);
  EXPECT_EQ(2.0, result->Intensity(1.0));
  EXPECT_NEAR(0.08272840, pdf, 0.001);
}

TEST(ImageEnvironmentalLight, EmissionFour) {
  std::vector<iris::ReferenceCounted<iris::Spectrum>> spectra = {
      kSpectrum0, kSpectrum0, kSpectrum1, kSpectrum2};
  std::vector<iris::visual> luma = {0.0, 0.0, 1.0, 2.0};
  std::pair<size_t, size_t> size(2, 2);
  iris::environmental_lights::ImageEnvironmentalLight light(
      spectra, luma, size, kScalar, iris::Matrix::Identity());

  iris::visual_t pdf;
  auto result = light.Emission(iris::Normalize(iris::Vector(-1.0, -1.0, -1.0)),
                               iris::testing::GetSpectralAllocator(), &pdf);
  ASSERT_TRUE(result);
  EXPECT_EQ(4.0, result->Intensity(1.0));
  EXPECT_NEAR(0.16545681, pdf, 0.001);
}