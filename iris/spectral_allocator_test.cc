#include "iris/spectral_allocator.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/internal/arena.h"
#include "iris/reflectors/mock_reflector.h"
#include "iris/spectra/mock_spectrum.h"

TEST(SpectralAllocator, SpectrumAddNullptr) {
  iris::internal::Arena arena;
  iris::SpectralAllocator allocator(arena);

  iris::spectra::MockSpectrum spectrum;
  EXPECT_EQ(nullptr,
            allocator.Add(static_cast<const iris::Spectrum*>(nullptr),
                          static_cast<const iris::Spectrum*>(nullptr)));
  EXPECT_EQ(&spectrum, allocator.Add(&spectrum, nullptr));
  EXPECT_EQ(&spectrum, allocator.Add(nullptr, &spectrum));
}

TEST(SpectralAllocator, SpectrumAdd) {
  iris::internal::Arena arena;
  iris::SpectralAllocator allocator(arena);

  iris::spectra::MockSpectrum spectrum0;
  EXPECT_CALL(spectrum0, Intensity(1.0)).WillOnce(testing::Return(1.0));
  iris::spectra::MockSpectrum spectrum1;
  EXPECT_CALL(spectrum1, Intensity(1.0)).WillOnce(testing::Return(2.0));
  EXPECT_EQ(3.0, allocator.Add(&spectrum0, &spectrum1)->Intensity(1.0));
}

TEST(SpectralAllocator, SpectrumScaleNullptr) {
  iris::internal::Arena arena;
  iris::SpectralAllocator allocator(arena);

  iris::spectra::MockSpectrum spectrum;
  EXPECT_EQ(nullptr,
            allocator.Scale(static_cast<const iris::Spectrum*>(nullptr), 0.0));
  EXPECT_EQ(nullptr, allocator.Scale(&spectrum, 0.0));
}

TEST(SpectralAllocator, SpectrumScaleIdentity) {
  iris::internal::Arena arena;
  iris::SpectralAllocator allocator(arena);

  iris::spectra::MockSpectrum spectrum;
  EXPECT_EQ(&spectrum, allocator.Scale(&spectrum, 1.0));
}

TEST(SpectralAllocator, SpectrumScale) {
  iris::internal::Arena arena;
  iris::SpectralAllocator allocator(arena);

  iris::spectra::MockSpectrum spectrum;
  EXPECT_CALL(spectrum, Intensity(1.0)).WillOnce(testing::Return(0.5));
  EXPECT_EQ(0.25, allocator.Scale(&spectrum, 0.5)->Intensity(1.0));
}

TEST(SpectralAllocator, SpectrumReflectNullptr) {
  iris::internal::Arena arena;
  iris::SpectralAllocator allocator(arena);

  iris::spectra::MockSpectrum spectrum;
  iris::reflectors::MockReflector reflector;
  EXPECT_EQ(nullptr, allocator.Reflect(nullptr, nullptr));
  EXPECT_EQ(nullptr, allocator.Reflect(&spectrum, nullptr));
  EXPECT_EQ(nullptr, allocator.Reflect(nullptr, &reflector));
}

TEST(SpectralAllocator, SpectrumReflect) {
  iris::internal::Arena arena;
  iris::SpectralAllocator allocator(arena);

  iris::spectra::MockSpectrum spectrum;
  EXPECT_CALL(spectrum, Intensity(1.0)).WillOnce(testing::Return(2.0));
  iris::reflectors::MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(1.0)).WillOnce(testing::Return(0.25));
  EXPECT_EQ(0.5, allocator.Reflect(&spectrum, &reflector)->Intensity(1.0));
}

TEST(SpectralAllocator, ReflectorAddNullptr) {
  iris::internal::Arena arena;
  iris::SpectralAllocator allocator(arena);

  iris::reflectors::MockReflector reflector;
  EXPECT_EQ(nullptr,
            allocator.Add(static_cast<const iris::Reflector*>(nullptr),
                          static_cast<const iris::Reflector*>(nullptr)));
  EXPECT_EQ(&reflector, allocator.Add(&reflector, nullptr));
  EXPECT_EQ(&reflector, allocator.Add(nullptr, &reflector));
}

TEST(SpectralAllocator, ReflectorAdd) {
  iris::internal::Arena arena;
  iris::SpectralAllocator allocator(arena);

  iris::reflectors::MockReflector reflector0;
  EXPECT_CALL(reflector0, Reflectance(1.0)).WillOnce(testing::Return(0.125));
  iris::reflectors::MockReflector reflector1;
  EXPECT_CALL(reflector1, Reflectance(1.0)).WillOnce(testing::Return(0.25));
  EXPECT_EQ(0.375, allocator.Add(&reflector0, &reflector1)->Reflectance(1.0));
}

TEST(SpectralAllocator, ReflectorScaleNullptr) {
  iris::internal::Arena arena;
  iris::SpectralAllocator allocator(arena);

  iris::reflectors::MockReflector reflector;
  EXPECT_EQ(nullptr,
            allocator.Scale(static_cast<const iris::Reflector*>(nullptr), 0.0));
  EXPECT_EQ(nullptr, allocator.Scale(&reflector, 0.0));
}

TEST(SpectralAllocator, ReflectorScaleIdentity) {
  iris::internal::Arena arena;
  iris::SpectralAllocator allocator(arena);

  iris::reflectors::MockReflector reflector;
  EXPECT_EQ(&reflector, allocator.Scale(&reflector, 1.0));
}

TEST(SpectralAllocator, ReflectorScale) {
  iris::internal::Arena arena;
  iris::SpectralAllocator allocator(arena);

  iris::reflectors::MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(1.0)).WillOnce(testing::Return(0.5));
  EXPECT_EQ(0.25, allocator.Scale(&reflector, 0.5)->Reflectance(1.0));
}

TEST(SpectralAllocator, ReflectorsScaleNullptr) {
  iris::internal::Arena arena;
  iris::SpectralAllocator allocator(arena);

  iris::reflectors::MockReflector reflector;
  EXPECT_EQ(nullptr, allocator.Scale(&reflector, nullptr));
  EXPECT_EQ(nullptr, allocator.Scale(nullptr, &reflector));
}

TEST(SpectralAllocator, ReflectorsScale) {
  iris::internal::Arena arena;
  iris::SpectralAllocator allocator(arena);

  iris::reflectors::MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(1.0))
      .Times(2)
      .WillRepeatedly(testing::Return(0.5));
  EXPECT_EQ(0.25, allocator.Scale(&reflector, &reflector)->Reflectance(1.0));
}

TEST(SpectralAllocator, InvertNullptr) {
  iris::internal::Arena arena;
  iris::SpectralAllocator allocator(arena);

  EXPECT_EQ(1.0, allocator.Invert(nullptr)->Reflectance(1.0));
}

TEST(SpectralAllocator, DoubleInvertNullptr) {
  iris::internal::Arena arena;
  iris::SpectralAllocator allocator(arena);

  EXPECT_EQ(nullptr, allocator.Invert(allocator.Invert(nullptr)));
}

TEST(SpectralAllocator, Invert) {
  iris::internal::Arena arena;
  iris::SpectralAllocator allocator(arena);

  iris::reflectors::MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(1.0))
      .Times(1)
      .WillRepeatedly(testing::Return(0.75));
  EXPECT_EQ(0.25, allocator.Invert(&reflector)->Reflectance(1.0));
}

TEST(SpectralAllocator, ReflectorUnboundedAddNullptr) {
  iris::internal::Arena arena;
  iris::SpectralAllocator allocator(arena);

  iris::reflectors::MockReflector reflector;
  EXPECT_EQ(nullptr, allocator.UnboundedAdd(
                         static_cast<const iris::Reflector*>(nullptr),
                         static_cast<const iris::Reflector*>(nullptr)));
  EXPECT_EQ(&reflector, allocator.UnboundedAdd(&reflector, nullptr));
  EXPECT_EQ(&reflector, allocator.UnboundedAdd(nullptr, &reflector));
}

TEST(SpectralAllocator, ReflectorUnboundedAdd) {
  iris::internal::Arena arena;
  iris::SpectralAllocator allocator(arena);

  iris::reflectors::MockReflector reflector0;
  EXPECT_CALL(reflector0, Reflectance(1.0)).WillOnce(testing::Return(1.0));
  iris::reflectors::MockReflector reflector1;
  EXPECT_CALL(reflector1, Reflectance(1.0)).WillOnce(testing::Return(0.25));
  EXPECT_EQ(1.25,
            allocator.UnboundedAdd(&reflector0, &reflector1)->Reflectance(1.0));
}

TEST(SpectralAllocator, ReflectorUnboundedScaleNullptr) {
  iris::internal::Arena arena;
  iris::SpectralAllocator allocator(arena);

  iris::reflectors::MockReflector reflector;
  EXPECT_EQ(nullptr, allocator.UnboundedScale(
                         static_cast<const iris::Reflector*>(nullptr), 0.0));
  EXPECT_EQ(nullptr, allocator.UnboundedScale(&reflector, 0.0));
}

TEST(SpectralAllocator, ReflectorUnboundedScaleIdentity) {
  iris::internal::Arena arena;
  iris::SpectralAllocator allocator(arena);

  iris::reflectors::MockReflector reflector;
  EXPECT_EQ(&reflector, allocator.UnboundedScale(&reflector, 1.0));
}

TEST(SpectralAllocator, ReflectorUnboundedScale) {
  iris::internal::Arena arena;
  iris::SpectralAllocator allocator(arena);

  iris::reflectors::MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(1.0)).WillOnce(testing::Return(0.5));
  EXPECT_EQ(1.0, allocator.UnboundedScale(&reflector, 2.0)->Reflectance(1.0));
}

TEST(SpectralAllocator, FresnelConductorPerpendicular) {
  iris::internal::Arena arena;
  iris::SpectralAllocator allocator(arena);

  EXPECT_EQ(nullptr,
            allocator.FresnelConductor(0.0, nullptr, nullptr, nullptr));
}

TEST(SpectralAllocator, FresnelConductorGlancing) {
  iris::internal::Arena arena;
  iris::SpectralAllocator allocator(arena);

  EXPECT_NEAR(1.0,
              allocator.FresnelConductor(0.0001, nullptr, nullptr, nullptr)
                  ->Reflectance(1.0),
              0.001);
}

TEST(SpectralAllocator, FresnelConductor) {
  iris::internal::Arena arena;
  iris::SpectralAllocator allocator(arena);

  iris::spectra::MockSpectrum eta_incident;
  EXPECT_CALL(eta_incident, Intensity(1.0)).WillOnce(testing::Return(1.0));
  iris::spectra::MockSpectrum eta_transmitted;
  EXPECT_CALL(eta_transmitted, Intensity(1.0)).WillOnce(testing::Return(2.0));
  iris::spectra::MockSpectrum k;
  EXPECT_CALL(k, Intensity(1.0)).WillOnce(testing::Return(2.0));

  EXPECT_NEAR(
      0.4018,
      allocator.FresnelConductor(0.5, &eta_incident, &eta_transmitted, &k)
          ->Reflectance(1.0),
      0.001);
}