#include "frontends/pbrt/area_lights/parse.h"

#include "frontends/pbrt/spectrum_managers/test_spectrum_manager.h"
#include "googletest/include/gtest/gtest.h"
#include "pbrt_proto/v3/v3.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace {

using ::iris::pbrt_frontend::spectrum_managers::TestSpectrumManager;
using ::pbrt_proto::v3::AreaLightSource;

TEST(ParseAreaLightSource, Empty) {
  TestSpectrumManager spectrum_manager;
  AreaLightSource area_light_source;

  auto [front, back] =
      ParseAreaLightSource(area_light_source, spectrum_manager);
  EXPECT_FALSE(front);
  EXPECT_FALSE(back);
}

TEST(ParseAreaLightSource, Diffuse) {
  TestSpectrumManager spectrum_manager;
  AreaLightSource area_light_source;
  area_light_source.mutable_diffuse();

  auto [front, back] =
      ParseAreaLightSource(area_light_source, spectrum_manager);
  EXPECT_TRUE(front);
  EXPECT_FALSE(back);
}

}  // namespace
}  // namespace pbrt_frontend
}  // namespace iris
