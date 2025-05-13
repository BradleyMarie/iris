#include "frontends/pbrt/area_lights/diffuse.h"

#include <cstdlib>

#include "frontends/pbrt/spectrum_managers/test_spectrum_manager.h"
#include "googletest/include/gtest/gtest.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace area_lights {
namespace {

using ::iris::pbrt_frontend::spectrum_managers::TestSpectrumManager;
using ::pbrt_proto::v3::AreaLightSource;
using ::testing::ExitedWithCode;

TEST(Diffuse, Empty) {
  TestSpectrumManager spectrum_manager;
  AreaLightSource::Diffuse diffuse;

  auto [front, back] = MakeDiffuse(diffuse, spectrum_manager);
  EXPECT_TRUE(front);
  EXPECT_FALSE(back);
}

TEST(Diffuse, TooLowSamples) {
  TestSpectrumManager spectrum_manager;
  AreaLightSource::Diffuse diffuse;
  diffuse.set_samples(0);

  EXPECT_EXIT(MakeDiffuse(diffuse, spectrum_manager),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: samples");
}

TEST(Diffuse, TooHighSamples) {
  TestSpectrumManager spectrum_manager;
  AreaLightSource::Diffuse diffuse;
  diffuse.set_samples(256);

  EXPECT_EXIT(MakeDiffuse(diffuse, spectrum_manager),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: samples");
}

TEST(Diffuse, TwoSided) {
  TestSpectrumManager spectrum_manager;
  AreaLightSource::Diffuse diffuse;
  diffuse.set_twosided(true);

  auto [front, back] = MakeDiffuse(diffuse, spectrum_manager);
  EXPECT_TRUE(front);
  EXPECT_TRUE(back);
}

}  // namespace
}  // namespace area_lights
}  // namespace pbrt_frontend
}  // namespace iris
