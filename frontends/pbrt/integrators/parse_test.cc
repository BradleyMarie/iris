#include "frontends/pbrt/integrators/parse.h"

#include <cstdlib>

#include "googletest/include/gtest/gtest.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace {

using ::pbrt_proto::v3::Integrator;
using ::testing::ExitedWithCode;

TEST(Default, Empty) {
  Integrator integrator;
  EXPECT_FALSE(ParseIntegrator(integrator));
}

TEST(ParseIntegrator, AmbientOcclusion) {
  Integrator integrator;
  integrator.mutable_ambientocclusion();

  EXPECT_EXIT(ParseIntegrator(integrator), ExitedWithCode(EXIT_FAILURE),
              "ERROR: Unsupported Integrator type: ambientocclusion");
}

TEST(ParseIntegrator, Bdpt) {
  Integrator integrator;
  integrator.mutable_bdpt();

  EXPECT_EXIT(ParseIntegrator(integrator), ExitedWithCode(EXIT_FAILURE),
              "ERROR: Unsupported Integrator type: bdpt");
}

TEST(ParseIntegrator, DirectLighting) {
  Integrator integrator;
  integrator.mutable_directlighting();

  EXPECT_EXIT(ParseIntegrator(integrator), ExitedWithCode(EXIT_FAILURE),
              "ERROR: Unsupported Integrator type: directlighting");
}

TEST(ParseIntegrator, Mlt) {
  Integrator integrator;
  integrator.mutable_mlt();

  EXPECT_EXIT(ParseIntegrator(integrator), ExitedWithCode(EXIT_FAILURE),
              "ERROR: Unsupported Integrator type: mlt");
}

TEST(ParseIntegrator, Path) {
  Integrator integrator;
  integrator.mutable_path();
  EXPECT_TRUE(ParseIntegrator(integrator));
}

TEST(ParseIntegrator, Sppm) {
  Integrator integrator;
  integrator.mutable_sppm();

  EXPECT_EXIT(ParseIntegrator(integrator), ExitedWithCode(EXIT_FAILURE),
              "ERROR: Unsupported Integrator type: sppm");
}

TEST(ParseIntegrator, VolPath) {
  Integrator integrator;
  integrator.mutable_volpath();

  EXPECT_EXIT(ParseIntegrator(integrator), ExitedWithCode(EXIT_FAILURE),
              "ERROR: Unsupported Integrator type: volpath");
}

TEST(ParseIntegrator, Whitted) {
  Integrator integrator;
  integrator.mutable_whitted();

  EXPECT_EXIT(ParseIntegrator(integrator), ExitedWithCode(EXIT_FAILURE),
              "ERROR: Unsupported Integrator type: whitted");
}

}  // namespace
}  // namespace pbrt_frontend
}  // namespace iris
