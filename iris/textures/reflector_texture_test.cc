#include "iris/textures/reflector_texture.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/reference_counted.h"
#include "iris/reflector.h"
#include "iris/reflectors/mock_reflector.h"
#include "iris/testing/spectral_allocator.h"
#include "iris/texture_coordinates.h"
#include "iris/textures/spectrum_texture.h"

namespace iris {
namespace textures {
namespace {

using ::iris::reflectors::MockReflector;
using ::iris::testing::GetSpectralAllocator;
using ::testing::_;
using ::testing::Return;

class TextTexture : public ReflectorTexture {
 public:
  TextTexture(const Reflector* reflector) : reflector_(reflector) {}

  const Reflector* Evaluate(const TextureCoordinates& coordinates,
                            SpectralAllocator& allocator) const override {
    return reflector_;
  }

  const Reflector* reflector_;
};

TEST(ReflectorTexture, EvaluateIntensity) {
  MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(_)).WillOnce(Return(2.0));

  TextTexture reflector_texture(&reflector);
  const SpectrumTexture& spectrum_texture =
      static_cast<SpectrumTexture&>(reflector_texture);

  TextureCoordinates coordinates{};
  EXPECT_EQ(2.0, spectrum_texture.Eval(coordinates, GetSpectralAllocator())
                     ->Intensity(1.0));
}

}  // namespace
}  // namespace textures
}  // namespace iris
