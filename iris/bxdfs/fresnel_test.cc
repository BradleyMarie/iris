#include "iris/bxdfs/fresnel.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/reflectors/mock_reflector.h"
#include "iris/spectra/mock_spectrum.h"
#include "iris/testing/spectral_allocator.h"

namespace iris {
namespace bxdfs {
namespace {

using ::iris::reflectors::MockReflector;
using ::iris::spectra::MockSpectrum;
using ::iris::testing::GetSpectralAllocator;
using ::testing::Return;

TEST(FresnelNoOp, NoOp) {
  MockReflector reflector;
  FresnelNoOp fresnel;
  EXPECT_TRUE(fresnel.IsValid());
  EXPECT_EQ(&reflector, fresnel.AttenuateReflectance(reflector, 1.0,
                                                     GetSpectralAllocator()));
  EXPECT_EQ(&reflector, fresnel.AttenuateTransmittance(reflector, 1.0,
                                                       GetSpectralAllocator()));
}

TEST(FresnelDielectric, IsValid) {
  EXPECT_TRUE(FresnelDielectric(1.0, 2.0).IsValid());
  EXPECT_FALSE(FresnelDielectric(-1.0, 2.0).IsValid());
  EXPECT_FALSE(FresnelDielectric(1.0, -2.0).IsValid());
}

TEST(FresnelDielectric, ReflectancePositiveCosTheta) {
  MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(1.0)).WillOnce(Return(1.0));
  FresnelDielectric fresnel(1.0, 2.0);
  const Reflector* attenuated =
      fresnel.AttenuateReflectance(reflector, 1.0, GetSpectralAllocator());
  EXPECT_NEAR(0.111111, attenuated->Reflectance(1.0), 0.001);
}

TEST(FresnelDielectric, ReflectanceNegativeCosTheta) {
  MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(1.0)).WillOnce(Return(1.0));
  FresnelDielectric fresnel(1.0, 2.0);
  const Reflector* attenuated =
      fresnel.AttenuateReflectance(reflector, -1.0, GetSpectralAllocator());
  EXPECT_NEAR(0.111111, attenuated->Reflectance(1.0), 0.001);
}

TEST(FresnelDielectric, ReflectanceTotalInternalReflection) {
  MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(1.0)).WillOnce(Return(1.0));
  FresnelDielectric fresnel(1.0, 2.0);
  const Reflector* attenuated =
      fresnel.AttenuateReflectance(reflector, -0.5, GetSpectralAllocator());
  EXPECT_EQ(1.0, attenuated->Reflectance(1.0));
}

TEST(FresnelDielectric, TransmittancePositiveCosTheta) {
  MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(1.0)).WillOnce(Return(1.0));
  FresnelDielectric fresnel(1.0, 2.0);
  const Reflector* attenuated =
      fresnel.AttenuateTransmittance(reflector, 1.0, GetSpectralAllocator());
  EXPECT_NEAR(0.88888, attenuated->Reflectance(1.0), 0.001);
}

TEST(FresnelDielectric, TransmittanceNegativeCosTheta) {
  MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(1.0)).WillOnce(Return(1.0));
  FresnelDielectric fresnel(1.0, 2.0);
  const Reflector* attenuated =
      fresnel.AttenuateTransmittance(reflector, 1.0, GetSpectralAllocator());
  EXPECT_NEAR(0.88888, attenuated->Reflectance(1.0), 0.001);
}

TEST(FresnelDieFresnelConductorlectric, IsValid) {
  MockSpectrum spectrum;
  EXPECT_TRUE(FresnelConductor(1.0, &spectrum, &spectrum).IsValid());
  EXPECT_TRUE(FresnelConductor(1.0, &spectrum, nullptr).IsValid());
  EXPECT_FALSE(FresnelConductor(0.0, &spectrum, &spectrum).IsValid());
  EXPECT_FALSE(FresnelConductor(1.0, nullptr, &spectrum).IsValid());
}

TEST(FresnelConductor, ReflectancePositiveCosTheta) {
  MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(1.0)).WillOnce(Return(1.0));
  MockSpectrum spectrum;
  EXPECT_CALL(spectrum, Intensity(1.0)).WillRepeatedly(Return(1.0));
  FresnelConductor fresnel(1.0, &spectrum, &spectrum);
  const Reflector* attenuated =
      fresnel.AttenuateReflectance(reflector, 1.0, GetSpectralAllocator());
  EXPECT_NEAR(0.200000, attenuated->Reflectance(1.0), 0.001);
}

TEST(FresnelConductor, ReflectanceNegativeCosTheta) {
  MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(1.0)).WillOnce(Return(1.0));
  MockSpectrum spectrum;
  EXPECT_CALL(spectrum, Intensity(1.0)).WillRepeatedly(Return(1.0));
  FresnelConductor fresnel(1.0, &spectrum, &spectrum);
  const Reflector* attenuated =
      fresnel.AttenuateReflectance(reflector, -1.0, GetSpectralAllocator());
  EXPECT_NEAR(0.200000, attenuated->Reflectance(1.0), 0.001);
}

TEST(FresnelConductor, Transmittance) {
  MockReflector reflector;
  MockSpectrum spectrum;
  FresnelConductor fresnel(1.0, &spectrum, &spectrum);
  EXPECT_FALSE(
      fresnel.AttenuateTransmittance(reflector, 1.0, GetSpectralAllocator()));
}

}  // namespace
}  // namespace bxdfs
}  // namespace iris
