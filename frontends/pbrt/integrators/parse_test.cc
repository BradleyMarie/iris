#include "frontends/pbrt/integrators/parse.h"

#include "googletest/include/gtest/gtest.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace {

using ::pbrt_proto::v3::Integrator;

TEST(Default, Empty) {
  Integrator integrator;
  EXPECT_FALSE(ParseIntegrator(integrator));
}

TEST(ParseIntegrator, AmbientOcclusion) {
  Integrator integrator;
  integrator.mutable_ambientocclusion();
  EXPECT_FALSE(ParseIntegrator(integrator));
}

TEST(ParseIntegrator, Bdpt) {
  Integrator integrator;
  integrator.mutable_bdpt();
  EXPECT_FALSE(ParseIntegrator(integrator));
}

TEST(ParseIntegrator, DirectLighting) {
  Integrator integrator;
  integrator.mutable_directlighting();
  EXPECT_FALSE(ParseIntegrator(integrator));
}

TEST(ParseIntegrator, Mlt) {
  Integrator integrator;
  integrator.mutable_mlt();
  EXPECT_FALSE(ParseIntegrator(integrator));
}

TEST(ParseIntegrator, Path) {
  Integrator integrator;
  integrator.mutable_path();
  EXPECT_TRUE(ParseIntegrator(integrator));
}

TEST(ParseIntegrator, Sppm) {
  Integrator integrator;
  integrator.mutable_sppm();
  EXPECT_FALSE(ParseIntegrator(integrator));
}

TEST(ParseIntegrator, VolPath) {
  Integrator integrator;
  integrator.mutable_volpath();
  EXPECT_FALSE(ParseIntegrator(integrator));
}

TEST(ParseIntegrator, Whitted) {
  Integrator integrator;
  integrator.mutable_whitted();
  EXPECT_FALSE(ParseIntegrator(integrator));
}

}  // namespace
}  // namespace pbrt_frontend
}  // namespace iris
