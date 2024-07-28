#include "iris/emissive_materials/constant_emissive_material.h"

#include "googlemock/include/gmock/gmock.h"
#include "googletest/include/gtest/gtest.h"
#include "iris/power_matchers/mock_power_matcher.h"
#include "iris/spectra/mock_spectrum.h"
#include "iris/testing/spectral_allocator.h"

TEST(ConstantEmissiveMaterialTest, Evaluate) {
  auto spectrum = iris::MakeReferenceCounted<iris::spectra::MockSpectrum>();
  iris::emissive_materials::ConstantEmissiveMaterial material(spectrum);
  EXPECT_EQ(
      spectrum.Get(),
      material.Evaluate(iris::TextureCoordinates{{0.0, 0.0}, std::nullopt},
                        iris::testing::GetSpectralAllocator()));
}

TEST(ConstantEmissiveMaterialTest, UnitPower) {
  auto spectrum = iris::MakeReferenceCounted<iris::spectra::MockSpectrum>();
  iris::emissive_materials::ConstantEmissiveMaterial material(spectrum);

  iris::power_matchers::MockPowerMatcher power_matcher;
  EXPECT_CALL(power_matcher, Match(testing::Ref(*spectrum)))
      .WillOnce(testing::Return(1.0));
  EXPECT_EQ(1.0, material.UnitPower(power_matcher));
}