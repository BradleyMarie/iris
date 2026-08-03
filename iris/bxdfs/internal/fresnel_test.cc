#include "iris/bxdfs/internal/fresnel.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/bxdfs/internal/math.h"
#include "iris/internal/arena.h"
#include "iris/reflectors/mock_reflector.h"
#include "iris/spectra/mock_spectrum.h"
#include "iris/testing/spectral_allocator.h"

namespace iris {
namespace bxdfs {
namespace internal {
namespace {

using ::iris::internal::Arena;
using ::iris::reflectors::MockReflector;
using ::iris::spectra::MockSpectrum;
using ::iris::testing::GetSpectralAllocator;
using ::testing::Return;

TEST(FresnelDielectric, IsValid) {
  EXPECT_TRUE(FresnelDielectric(1.0, 1.5).IsValid());
  EXPECT_FALSE(FresnelDielectric(0.5, 1.5).IsValid());
  EXPECT_FALSE(FresnelDielectric(1.0, 0.9).IsValid());
}

TEST(FresnelDielectric, AttenuateReflectance) {
  MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(1.0)).WillRepeatedly(Return(1.0));

  FresnelDielectric fresnel(1.0, 1.5);
  const Reflector* result =
      fresnel.AttenuateReflectance(reflector, 1.0, GetSpectralAllocator());
  ASSERT_TRUE(result);
  EXPECT_NEAR(0.04, result->Reflectance(1.0), 0.001);
}

TEST(FresnelDielectric, AttenuateTransmittance) {
  MockReflector transmittance;
  EXPECT_CALL(transmittance, Reflectance(1.0)).WillRepeatedly(Return(1.0));

  FresnelDielectric fresnel(1.0, 1.5);
  const Reflector* result = fresnel.AttenuateTransmittance(
      transmittance, 1.0, GetSpectralAllocator());
  ASSERT_TRUE(result);
  EXPECT_NEAR(0.96, result->Reflectance(1.0), 0.001);
}

TEST(FresnelConductor, IsValidValid) {
  MockSpectrum eta_spec;
  MockSpectrum k_spec;
  EXPECT_TRUE(FresnelConductor(1.0, &eta_spec, &k_spec).IsValid());
  EXPECT_FALSE(FresnelConductor(0.5, &eta_spec, &k_spec).IsValid());
}

TEST(FresnelConductor, AttenuateReflectance) {
  MockSpectrum eta_conductor;
  EXPECT_CALL(eta_conductor, Intensity(1.0)).WillRepeatedly(Return(2.0));
  MockSpectrum k_conductor;
  EXPECT_CALL(k_conductor, Intensity(1.0)).WillRepeatedly(Return(2.0));

  MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(1.0)).WillRepeatedly(Return(1.0));

  FresnelConductor fresnel(1.0, &eta_conductor, &k_conductor);
  const Reflector* result =
      fresnel.AttenuateReflectance(reflector, 1.0, GetSpectralAllocator());
  ASSERT_TRUE(result);
  EXPECT_NEAR(0.3846, result->Reflectance(1.0), 0.001);
}

TEST(FresnelConductor, AttenuateTransmittance) {
  MockSpectrum eta_conductor;
  EXPECT_CALL(eta_conductor, Intensity(1.0)).WillRepeatedly(Return(2.0));
  MockSpectrum k_conductor;
  EXPECT_CALL(k_conductor, Intensity(1.0)).WillRepeatedly(Return(2.0));

  MockReflector transmittance;
  EXPECT_CALL(transmittance, Reflectance(1.0)).WillRepeatedly(Return(1.0));

  FresnelConductor fresnel(1.0, &eta_conductor, &k_conductor);
  const Reflector* result = fresnel.AttenuateTransmittance(
      transmittance, 0.5, GetSpectralAllocator());
  EXPECT_EQ(nullptr, result);
}

TEST(DisneyFresnel, IsValid) {
  EXPECT_TRUE(DisneyFresnel(nullptr, 1.0, 1.0, 1.0).IsValid());
  EXPECT_FALSE(DisneyFresnel(nullptr, 1.0, 0.5, 1.5).IsValid());
  EXPECT_FALSE(DisneyFresnel(nullptr, 1.0, 1.0, 0.9).IsValid());
  EXPECT_FALSE(DisneyFresnel(nullptr, -0.1, 1.0, 1.5).IsValid());
}

TEST(DisneyFresnel, AttenuateReflectanceZeroMetallic) {
  MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(1.0)).WillRepeatedly(Return(1.0));

  DisneyFresnel fresnel(&reflector, 0.0, 1.0, 1.5);
  const Reflector* result =
      fresnel.AttenuateReflectance(reflector, 0.5, GetSpectralAllocator());
  ASSERT_TRUE(result);
  EXPECT_NEAR(0.0891, result->Reflectance(1.0), 0.001);
}

TEST(DisneyFresnel, AttenuateReflectanceFullMetallic) {
  MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(1.0)).WillRepeatedly(Return(1.0));

  DisneyFresnel fresnel(&reflector, 1.0, 1.0, 1.5);
  const Reflector* result =
      fresnel.AttenuateReflectance(reflector, 0.5, GetSpectralAllocator());
  ASSERT_TRUE(result);
  EXPECT_NEAR(1.0, result->Reflectance(1.0), 0.001);
}

TEST(DisneyFresnel, AttenuateReflectanceGrazingIncidence) {
  MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(1.0)).WillRepeatedly(Return(1.0));

  DisneyFresnel fresnel(&reflector, 0.5, 1.0, 1.5);
  const Reflector* result =
      fresnel.AttenuateReflectance(reflector, 0.0, GetSpectralAllocator());
  ASSERT_TRUE(result);
  EXPECT_NEAR(1.0, result->Reflectance(1.0), 0.001);
}

TEST(DisneyFresnel, AttenuateReflectanceNormalIncidence) {
  MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(1.0)).WillRepeatedly(Return(1.0));

  DisneyFresnel fresnel(&reflector, 0.5, 1.0, 1.5);
  const Reflector* result =
      fresnel.AttenuateReflectance(reflector, 1.0, GetSpectralAllocator());
  ASSERT_TRUE(result);
  EXPECT_NEAR(0.5200, result->Reflectance(1.0), 0.001);
}

TEST(DisneyFresnel, AttenuateReflectanceBlendAll) {
  MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(1.0)).WillRepeatedly(Return(1.0));

  DisneyFresnel fresnel_zero(&reflector, 0.5, 1.0, 1.5);
  const Reflector* result_zero =
      fresnel_zero.AttenuateReflectance(reflector, 0.5, GetSpectralAllocator());
  ASSERT_TRUE(result_zero);
  EXPECT_NEAR(0.5445, result_zero->Reflectance(1.0), 0.001);
}

TEST(DisneyFresnel, AttenuateTransmittance) {
  MockReflector transmittance;
  EXPECT_CALL(transmittance, Reflectance(1.0)).WillRepeatedly(Return(1.0));

  DisneyFresnel fresnel(nullptr, 0.5, 1.0, 1.5);
  const Reflector* result = fresnel.AttenuateTransmittance(
      transmittance, 0.5, GetSpectralAllocator());
  EXPECT_EQ(nullptr, result);
}

}  // namespace
}  // namespace internal
}  // namespace bxdfs
}  // namespace iris
