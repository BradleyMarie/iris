#include "iris/spectral_allocator.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/internal/arena.h"
#include "iris/reflector.h"
#include "iris/reflectors/mock_reflector.h"
#include "iris/spectra/mock_spectrum.h"
#include "iris/spectrum.h"

namespace iris {
namespace {

using ::iris::internal::Arena;
using ::iris::reflectors::MockReflector;
using ::iris::spectra::MockSpectrum;
using ::testing::Return;

TEST(SpectralAllocator, SpectrumAddNullptr) {
  Arena arena;
  SpectralAllocator allocator(arena);

  MockSpectrum spectrum;
  EXPECT_EQ(nullptr, allocator.Add(static_cast<const Spectrum*>(nullptr),
                                   static_cast<const Spectrum*>(nullptr)));
  EXPECT_EQ(&spectrum, allocator.Add(&spectrum, nullptr));
  EXPECT_EQ(&spectrum, allocator.Add(nullptr, &spectrum));
}

TEST(SpectralAllocator, SpectrumAdd) {
  Arena arena;
  SpectralAllocator allocator(arena);

  MockSpectrum spectrum0;
  EXPECT_CALL(spectrum0, Intensity(1.0)).WillOnce(Return(1.0));
  MockSpectrum spectrum1;
  EXPECT_CALL(spectrum1, Intensity(1.0)).WillOnce(Return(2.0));
  EXPECT_EQ(3.0, allocator.Add(&spectrum0, &spectrum1)->Intensity(1.0));
}

TEST(SpectralAllocator, SpectrumScaleNullptr) {
  Arena arena;
  SpectralAllocator allocator(arena);

  MockSpectrum spectrum;
  EXPECT_EQ(nullptr,
            allocator.Scale(static_cast<const Spectrum*>(nullptr), 0.0));
  EXPECT_EQ(nullptr, allocator.Scale(&spectrum, 0.0));
}

TEST(SpectralAllocator, SpectrumScaleIdentity) {
  Arena arena;
  SpectralAllocator allocator(arena);

  MockSpectrum spectrum;
  EXPECT_EQ(&spectrum, allocator.Scale(&spectrum, 1.0));
}

TEST(SpectralAllocator, SpectrumScale) {
  Arena arena;
  SpectralAllocator allocator(arena);

  MockSpectrum spectrum;
  EXPECT_CALL(spectrum, Intensity(1.0)).WillOnce(Return(0.5));
  EXPECT_EQ(0.25, allocator.Scale(&spectrum, 0.5)->Intensity(1.0));
}

TEST(SpectralAllocator, SpectrumReflectNullptr) {
  Arena arena;
  SpectralAllocator allocator(arena);

  MockSpectrum spectrum;
  MockReflector reflector;
  EXPECT_EQ(nullptr, allocator.Reflect(nullptr, nullptr));
  EXPECT_EQ(nullptr, allocator.Reflect(&spectrum, nullptr));
  EXPECT_EQ(nullptr, allocator.Reflect(nullptr, &reflector));
}

TEST(SpectralAllocator, SpectrumReflect) {
  Arena arena;
  SpectralAllocator allocator(arena);

  MockSpectrum spectrum;
  EXPECT_CALL(spectrum, Intensity(1.0)).WillOnce(Return(2.0));
  MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(1.0)).WillOnce(Return(0.25));
  EXPECT_EQ(0.5, allocator.Reflect(&spectrum, &reflector)->Intensity(1.0));
}

TEST(SpectralAllocator, ReflectorAddNullptr) {
  Arena arena;
  SpectralAllocator allocator(arena);

  MockReflector reflector;
  EXPECT_EQ(nullptr, allocator.Add(static_cast<const Reflector*>(nullptr),
                                   static_cast<const Reflector*>(nullptr)));
  EXPECT_EQ(&reflector, allocator.Add(&reflector, nullptr));
  EXPECT_EQ(&reflector, allocator.Add(nullptr, &reflector));
}

TEST(SpectralAllocator, ReflectorAdd) {
  Arena arena;
  SpectralAllocator allocator(arena);

  MockReflector reflector0;
  EXPECT_CALL(reflector0, Reflectance(1.0)).WillOnce(Return(0.125));
  MockReflector reflector1;
  EXPECT_CALL(reflector1, Reflectance(1.0)).WillOnce(Return(0.25));
  EXPECT_EQ(0.375, allocator.Add(&reflector0, &reflector1)->Reflectance(1.0));
}

TEST(SpectralAllocator, ReflectorScaleNullptr) {
  Arena arena;
  SpectralAllocator allocator(arena);

  MockReflector reflector;
  EXPECT_EQ(nullptr,
            allocator.Scale(static_cast<const Reflector*>(nullptr), 0.0));
  EXPECT_EQ(nullptr, allocator.Scale(&reflector, 0.0));
}

TEST(SpectralAllocator, ReflectorScaleIdentity) {
  Arena arena;
  SpectralAllocator allocator(arena);

  MockReflector reflector;
  EXPECT_EQ(&reflector, allocator.Scale(&reflector, 1.0));
}

TEST(SpectralAllocator, ReflectorScale) {
  Arena arena;
  SpectralAllocator allocator(arena);

  MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(1.0)).WillOnce(Return(0.5));
  EXPECT_EQ(0.25, allocator.Scale(&reflector, 0.5)->Reflectance(1.0));
}

TEST(SpectralAllocator, ReflectorsScaleNullptr) {
  Arena arena;
  SpectralAllocator allocator(arena);

  MockReflector reflector;
  EXPECT_EQ(nullptr, allocator.Scale(&reflector, nullptr));
  EXPECT_EQ(nullptr, allocator.Scale(nullptr, &reflector));
}

TEST(SpectralAllocator, ReflectorsScale) {
  Arena arena;
  SpectralAllocator allocator(arena);

  MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(1.0)).Times(2).WillRepeatedly(Return(0.5));
  EXPECT_EQ(0.25, allocator.Scale(&reflector, &reflector)->Reflectance(1.0));
}

TEST(SpectralAllocator, InvertNullptr) {
  Arena arena;
  SpectralAllocator allocator(arena);

  EXPECT_EQ(1.0, allocator.Invert(nullptr)->Reflectance(1.0));
}

TEST(SpectralAllocator, DoubleInvertNullptr) {
  Arena arena;
  SpectralAllocator allocator(arena);

  EXPECT_EQ(nullptr, allocator.Invert(allocator.Invert(nullptr)));
}

TEST(SpectralAllocator, Invert) {
  Arena arena;
  SpectralAllocator allocator(arena);

  MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(1.0))
      .Times(1)
      .WillRepeatedly(Return(0.75));
  EXPECT_EQ(0.25, allocator.Invert(&reflector)->Reflectance(1.0));
}

TEST(SpectralAllocator, ReflectorUnboundedAddNullptr) {
  Arena arena;
  SpectralAllocator allocator(arena);

  MockReflector reflector;
  EXPECT_EQ(nullptr,
            allocator.UnboundedAdd(static_cast<const Reflector*>(nullptr),
                                   static_cast<const Reflector*>(nullptr)));
  EXPECT_EQ(&reflector, allocator.UnboundedAdd(&reflector, nullptr));
  EXPECT_EQ(&reflector, allocator.UnboundedAdd(nullptr, &reflector));
}

TEST(SpectralAllocator, ReflectorUnboundedAdd) {
  Arena arena;
  SpectralAllocator allocator(arena);

  MockReflector reflector0;
  EXPECT_CALL(reflector0, Reflectance(1.0)).WillOnce(Return(1.0));
  MockReflector reflector1;
  EXPECT_CALL(reflector1, Reflectance(1.0)).WillOnce(Return(0.25));
  EXPECT_EQ(1.25,
            allocator.UnboundedAdd(&reflector0, &reflector1)->Reflectance(1.0));
}

TEST(SpectralAllocator, ReflectorUnboundedScaleNullptr) {
  Arena arena;
  SpectralAllocator allocator(arena);

  MockReflector reflector;
  EXPECT_EQ(nullptr, allocator.UnboundedScale(
                         static_cast<const Reflector*>(nullptr), 0.0));
  EXPECT_EQ(nullptr, allocator.UnboundedScale(&reflector, 0.0));
}

TEST(SpectralAllocator, ReflectorUnboundedScaleIdentity) {
  Arena arena;
  SpectralAllocator allocator(arena);

  MockReflector reflector;
  EXPECT_EQ(&reflector, allocator.UnboundedScale(&reflector, 1.0));
}

TEST(SpectralAllocator, ReflectorUnboundedScale) {
  Arena arena;
  SpectralAllocator allocator(arena);

  MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(1.0)).WillOnce(Return(0.5));
  EXPECT_EQ(1.0, allocator.UnboundedScale(&reflector, 2.0)->Reflectance(1.0));
}

TEST(SpectralAllocator, FresnelConductorNullptr) {
  Arena arena;
  SpectralAllocator allocator(arena);

  MockSpectrum spectrum;
  EXPECT_TRUE(allocator.FresnelConductor(1.0, &spectrum, &spectrum, 0.0001));
  EXPECT_FALSE(allocator.FresnelConductor(0.0, &spectrum, &spectrum, 0.0001));
  EXPECT_FALSE(allocator.FresnelConductor(1.0, nullptr, &spectrum, 0.0001));
  EXPECT_TRUE(allocator.FresnelConductor(1.0, &spectrum, nullptr, 0.0001));
  EXPECT_TRUE(allocator.FresnelConductor(1.0, &spectrum, &spectrum, 0.0));
}

TEST(SpectralAllocator, FresnelConductorPerpendicular) {
  Arena arena;
  SpectralAllocator allocator(arena);

  EXPECT_EQ(nullptr, allocator.FresnelConductor(0.0, nullptr, nullptr, 0.0));
}

TEST(SpectralAllocator, FresnelConductorGlancing) {
  Arena arena;
  SpectralAllocator allocator(arena);

  MockSpectrum spectrum;
  EXPECT_CALL(spectrum, Intensity(1.0)).WillRepeatedly(Return(2.0));
  const Reflector* reflector =
      allocator.FresnelConductor(1.0, &spectrum, &spectrum, 0.0001);
  ASSERT_TRUE(reflector);
  EXPECT_NEAR(1.0, reflector->Reflectance(1.0), 0.001);
}

TEST(SpectralAllocator, FresnelConductorReturnsBadEtaConductor) {
  Arena arena;
  SpectralAllocator allocator(arena);

  MockSpectrum eta_conductor;
  EXPECT_CALL(eta_conductor, Intensity(1.0)).WillOnce(Return(-1.0));
  MockSpectrum k_conductor;

  const Reflector* reflector =
      allocator.FresnelConductor(1.0, &eta_conductor, &k_conductor, 0.5);
  ASSERT_TRUE(reflector);
  EXPECT_NEAR(0.0, reflector->Reflectance(1.0), 0.001);
}

TEST(SpectralAllocator, FresnelConductorReturnsBadKConductor) {
  Arena arena;
  SpectralAllocator allocator(arena);

  MockSpectrum eta_conductor;
  EXPECT_CALL(eta_conductor, Intensity(1.0)).WillOnce(Return(2.0));
  MockSpectrum k_conductor;
  EXPECT_CALL(k_conductor, Intensity(1.0)).WillOnce(Return(-1.0));

  const Reflector* reflector =
      allocator.FresnelConductor(1.0, &eta_conductor, &k_conductor, 0.5);
  ASSERT_TRUE(reflector);
  EXPECT_NEAR(0.1613, reflector->Reflectance(1.0), 0.001);
}

TEST(SpectralAllocator, FresnelConductor) {
  Arena arena;
  SpectralAllocator allocator(arena);

  MockSpectrum eta_conductor;
  EXPECT_CALL(eta_conductor, Intensity(1.0)).WillOnce(Return(2.0));
  MockSpectrum k_conductor;
  EXPECT_CALL(k_conductor, Intensity(1.0)).WillOnce(Return(2.0));

  const Reflector* reflector =
      allocator.FresnelConductor(1.0, &eta_conductor, &k_conductor, 0.5);
  ASSERT_TRUE(reflector);
  EXPECT_NEAR(0.4018, reflector->Reflectance(1.0), 0.001);
}

}  // namespace
}  // namespace iris
