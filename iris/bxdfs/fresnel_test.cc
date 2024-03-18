#include "iris/bxdfs/fresnel.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/reflectors/mock_reflector.h"
#include "iris/testing/spectral_allocator.h"

TEST(FresnelNoOp, NoOp) {
  iris::reflectors::MockReflector reflector;
  iris::bxdfs::FresnelNoOp fresnel;
  EXPECT_EQ(&reflector,
            fresnel.AttenuateReflectance(
                reflector, 1.0, iris::testing::GetSpectralAllocator()));
  EXPECT_EQ(&reflector,
            fresnel.AttenuateTransmittance(
                reflector, 1.0, iris::testing::GetSpectralAllocator()));
}

TEST(FresnelDielectric, PositiveCosTheta) {
  iris::reflectors::MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(1.0)).WillOnce(testing::Return(1.0));
  iris::bxdfs::FresnelDielectric fresnel(1.0, 2.0);
  auto* attenuated = fresnel.AttenuateReflectance(
      reflector, 1.0, iris::testing::GetSpectralAllocator());
  EXPECT_NEAR(0.111111, attenuated->Reflectance(1.0), 0.001);
}

TEST(FresnelDielectric, NegativeCosTheta) {
  iris::reflectors::MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(1.0)).WillOnce(testing::Return(1.0));
  iris::bxdfs::FresnelDielectric fresnel(1.0, 2.0);
  auto* attenuated = fresnel.AttenuateReflectance(
      reflector, -1.0, iris::testing::GetSpectralAllocator());
  EXPECT_NEAR(0.111111, attenuated->Reflectance(1.0), 0.001);
}

TEST(FresnelDielectric, TotalInternalReflection) {
  iris::reflectors::MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(1.0)).WillOnce(testing::Return(1.0));
  iris::bxdfs::FresnelDielectric fresnel(1.0, 2.0);
  auto* attenuated = fresnel.AttenuateReflectance(
      reflector, -0.5, iris::testing::GetSpectralAllocator());
  EXPECT_EQ(1.0, attenuated->Reflectance(1.0));
}

TEST(FresnelDielectric, Transmittance) {
  iris::reflectors::MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(1.0)).WillOnce(testing::Return(1.0));
  iris::bxdfs::FresnelDielectric fresnel(1.0, 2.0);
  auto* attenuated = fresnel.AttenuateTransmittance(
      reflector, 1.0, iris::testing::GetSpectralAllocator());
  EXPECT_NEAR(0.88888, attenuated->Reflectance(1.0), 0.001);
}