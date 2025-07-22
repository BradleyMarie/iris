#include "iris/emissive_materials/constant_emissive_material.h"

#include "googlemock/include/gmock/gmock.h"
#include "googletest/include/gtest/gtest.h"
#include "iris/point.h"
#include "iris/power_matchers/mock_power_matcher.h"
#include "iris/spectra/mock_spectrum.h"
#include "iris/testing/spectral_allocator.h"

namespace iris {
namespace emissive_materials {
namespace {

using ::iris::power_matchers::MockPowerMatcher;
using ::iris::spectra::MockSpectrum;
using ::iris::testing::GetSpectralAllocator;
using ::testing::Ref;
using ::testing::Return;

TEST(ConstantEmissiveMaterialTest, Null) {
  EXPECT_FALSE(MakeConstantEmissiveMaterial(ReferenceCounted<Spectrum>()));
}

TEST(ConstantEmissiveMaterialTest, Evaluate) {
  ReferenceCounted<MockSpectrum> spectrum =
      MakeReferenceCounted<MockSpectrum>();
  ReferenceCounted<EmissiveMaterial> material =
      MakeConstantEmissiveMaterial(spectrum);
  EXPECT_EQ(spectrum.Get(),
            material->Evaluate(
                TextureCoordinates{
                    Point(0.0, 0.0, 0.0), {}, {0.0, 0.0}, std::nullopt},
                GetSpectralAllocator()));
}

TEST(ConstantEmissiveMaterialTest, UnitPower) {
  ReferenceCounted<MockSpectrum> spectrum =
      MakeReferenceCounted<MockSpectrum>();
  ReferenceCounted<EmissiveMaterial> material =
      MakeConstantEmissiveMaterial(spectrum);
  MockPowerMatcher power_matcher;
  EXPECT_CALL(power_matcher, Match(Ref(*spectrum))).WillOnce(Return(1.0));
  EXPECT_EQ(1.0, material->UnitPower(power_matcher));
}

}  // namespace
}  // namespace emissive_materials
}  // namespace iris
