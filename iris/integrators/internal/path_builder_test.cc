#include "iris/integrators/internal/path_builder.h"

#include <vector>

#include "googletest/include/gtest/gtest.h"
#include "iris/float.h"
#include "iris/reflector.h"
#include "iris/reflectors/mock_reflector.h"
#include "iris/spectra/mock_spectrum.h"
#include "iris/spectrum.h"
#include "iris/testing/spectral_allocator.h"

namespace iris {
namespace integrators {
namespace internal {
namespace {

using ::iris::reflectors::MockReflector;
using ::iris::spectra::MockSpectrum;
using ::iris::testing::GetSpectralAllocator;
using ::testing::_;
using ::testing::Return;

class PathBuilderTest : public ::testing::Test {
 public:
  PathBuilderTest()
      : reflectors_(),
        spectra_(),
        attenuations_(),
        path_builder_(reflectors_, spectra_, attenuations_) {}

  void Add(const Spectrum& spectrum) {
    path_builder_.Add(&spectrum, GetSpectralAllocator());
  }

  void Bounce(const Reflector& reflector, visual_t attenuation) {
    path_builder_.Bounce(&reflector, attenuation);
  }

  const Spectrum* Build() {
    return path_builder_.Build(GetSpectralAllocator());
  }

 private:
  std::vector<const Reflector*> reflectors_;
  std::vector<const Spectrum*> spectra_;
  std::vector<visual_t> attenuations_;

 public:
  PathBuilder path_builder_;
};

TEST_F(PathBuilderTest, Empty) { EXPECT_EQ(nullptr, Build()); }

TEST_F(PathBuilderTest, NoSpectra) {
  MockReflector mock_reflector;
  Bounce(mock_reflector, 1.0);
  EXPECT_EQ(nullptr, Build());
}

TEST_F(PathBuilderTest, NoBounces) {
  MockSpectrum first_spectrum;
  EXPECT_CALL(first_spectrum, Intensity(_)).WillOnce(Return(1.0));

  Add(first_spectrum);

  const Spectrum* sum = Build();
  ASSERT_NE(nullptr, sum);
  EXPECT_EQ(1.0, sum->Intensity(1.0));
}

TEST_F(PathBuilderTest, FullPath) {
  MockSpectrum first_spectrum;
  EXPECT_CALL(first_spectrum, Intensity(_)).WillOnce(Return(1.0));

  MockReflector mock_reflector;
  EXPECT_CALL(mock_reflector, Reflectance(_)).WillOnce(Return(0.5));

  MockSpectrum second_spectrum;
  EXPECT_CALL(second_spectrum, Intensity(_)).WillOnce(Return(2.0));

  Add(first_spectrum);
  Bounce(mock_reflector, 5.0);
  Add(second_spectrum);

  const Spectrum* sum = Build();
  ASSERT_NE(nullptr, sum);
  EXPECT_EQ(6.0, sum->Intensity(1.0));
}

}  // namespace
}  // namespace internal
}  // namespace integrators
}  // namespace iris
