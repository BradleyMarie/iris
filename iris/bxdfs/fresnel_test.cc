#include "iris/bxdfs/fresnel.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/testing/spectral_allocator.h"

TEST(FresnelDielectric, PositiveCosTheta) {
  iris::bxdfs::FresnelDielectric fresnel(1.0, 2.0);
  auto* reflector =
      fresnel.Evaluate(1.0, iris::testing::GetSpectralAllocator());
  EXPECT_NEAR(0.111111, reflector->Reflectance(1.0), 0.001);
}

TEST(FresnelDielectric, NegativeCosTheta) {
  iris::bxdfs::FresnelDielectric fresnel(1.0, 2.0);
  auto* reflector =
      fresnel.Evaluate(-1.0, iris::testing::GetSpectralAllocator());
  EXPECT_NEAR(0.111111, reflector->Reflectance(1.0), 0.001);
}

TEST(FresnelDielectric, TotalInternalReflection) {
  iris::bxdfs::FresnelDielectric fresnel(1.0, 2.0);
  auto* reflector =
      fresnel.Evaluate(-0.5, iris::testing::GetSpectralAllocator());
  EXPECT_EQ(1.0, reflector->Reflectance(1.0));
}