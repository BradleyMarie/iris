#include "iris/bxdfs/fresnel.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/reflectors/mock_reflector.h"
#include "iris/testing/spectral_allocator.h"

namespace iris {
namespace bxdfs {
namespace {

using ::iris::reflectors::MockReflector;
using ::testing::Return;

TEST(FresnelNoOp, NoOp) {
  MockReflector reflector;
  FresnelNoOp fresnel;
  EXPECT_EQ(&reflector, fresnel.AttenuateReflectance(
                            reflector, 1.0, testing::GetSpectralAllocator()));
  EXPECT_EQ(&reflector, fresnel.AttenuateTransmittance(
                            reflector, 1.0, testing::GetSpectralAllocator()));
}

TEST(FresnelDielectric, PositiveCosTheta) {
  MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(1.0)).WillOnce(Return(1.0));
  FresnelDielectric fresnel(1.0, 2.0);
  const Reflector* attenuated = fresnel.AttenuateReflectance(
      reflector, 1.0, testing::GetSpectralAllocator());
  EXPECT_NEAR(0.111111, attenuated->Reflectance(1.0), 0.001);
}

TEST(FresnelDielectric, NegativeCosTheta) {
  MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(1.0)).WillOnce(Return(1.0));
  FresnelDielectric fresnel(1.0, 2.0);
  const Reflector* attenuated = fresnel.AttenuateReflectance(
      reflector, -1.0, testing::GetSpectralAllocator());
  EXPECT_NEAR(0.111111, attenuated->Reflectance(1.0), 0.001);
}

TEST(FresnelDielectric, TotalInternalReflection) {
  MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(1.0)).WillOnce(Return(1.0));
  FresnelDielectric fresnel(1.0, 2.0);
  const Reflector* attenuated = fresnel.AttenuateReflectance(
      reflector, -0.5, testing::GetSpectralAllocator());
  EXPECT_EQ(1.0, attenuated->Reflectance(1.0));
}

TEST(FresnelDielectric, Transmittance) {
  MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(1.0)).WillOnce(Return(1.0));
  FresnelDielectric fresnel(1.0, 2.0);
  const Reflector* attenuated = fresnel.AttenuateTransmittance(
      reflector, 1.0, testing::GetSpectralAllocator());
  EXPECT_NEAR(0.88888, attenuated->Reflectance(1.0), 0.001);
}

}  // namespace
}  // namespace bxdfs
}  // namespace iris