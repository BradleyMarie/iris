#include "frontends/pbrt/lights/parse.h"

#include <filesystem>

#include "frontends/pbrt/spectrum_managers/test_spectrum_manager.h"
#include "googletest/include/gtest/gtest.h"
#include "iris/environmental_light.h"
#include "iris/light.h"
#include "iris/matrix.h"
#include "iris/reference_counted.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace {

using ::iris::pbrt_frontend::spectrum_managers::TestSpectrumManager;
using ::pbrt_proto::v3::LightSource;
using ::testing::ExitedWithCode;

TEST(Default, Empty) {
  Matrix model_to_world = Matrix::Identity();
  TestSpectrumManager spectrum_manager;

  LightSource light_source;

  std::variant<ReferenceCounted<Light>, ReferenceCounted<EnvironmentalLight>>
      result = ParseLightSource(light_source, std::filesystem::current_path(),
                                model_to_world, spectrum_manager);
  EXPECT_FALSE(std::get<ReferenceCounted<Light>>(result));
}

TEST(Default, Distant) {
  Matrix model_to_world = Matrix::Identity();
  TestSpectrumManager spectrum_manager;

  LightSource light_source;
  light_source.mutable_distant();

  std::variant<ReferenceCounted<Light>, ReferenceCounted<EnvironmentalLight>>
      result = ParseLightSource(light_source, std::filesystem::current_path(),
                                model_to_world, spectrum_manager);
  EXPECT_TRUE(std::get<ReferenceCounted<Light>>(result));
}

TEST(Default, Goniometric) {
  Matrix model_to_world = Matrix::Identity();
  TestSpectrumManager spectrum_manager;

  LightSource light_source;
  light_source.mutable_goniometric();

  std::variant<ReferenceCounted<Light>, ReferenceCounted<EnvironmentalLight>>
      result = ParseLightSource(light_source, std::filesystem::current_path(),
                                model_to_world, spectrum_manager);
  EXPECT_FALSE(std::get<ReferenceCounted<Light>>(result));
}

TEST(Default, Infinite) {
  Matrix model_to_world = Matrix::Identity();
  TestSpectrumManager spectrum_manager;

  LightSource light_source;
  light_source.mutable_infinite();

  std::variant<ReferenceCounted<Light>, ReferenceCounted<EnvironmentalLight>>
      result = ParseLightSource(light_source, std::filesystem::current_path(),
                                model_to_world, spectrum_manager);
  EXPECT_TRUE(std::get<ReferenceCounted<EnvironmentalLight>>(result));
}

TEST(Default, Point) {
  Matrix model_to_world = Matrix::Identity();
  TestSpectrumManager spectrum_manager;

  LightSource light_source;
  light_source.mutable_point();

  std::variant<ReferenceCounted<Light>, ReferenceCounted<EnvironmentalLight>>
      result = ParseLightSource(light_source, std::filesystem::current_path(),
                                model_to_world, spectrum_manager);
  EXPECT_FALSE(std::get<ReferenceCounted<Light>>(result));
}

TEST(Default, Projection) {
  Matrix model_to_world = Matrix::Identity();
  TestSpectrumManager spectrum_manager;

  LightSource light_source;
  light_source.mutable_projection();

  std::variant<ReferenceCounted<Light>, ReferenceCounted<EnvironmentalLight>>
      result = ParseLightSource(light_source, std::filesystem::current_path(),
                                model_to_world, spectrum_manager);
  EXPECT_FALSE(std::get<ReferenceCounted<Light>>(result));
}

TEST(Default, Spot) {
  Matrix model_to_world = Matrix::Identity();
  TestSpectrumManager spectrum_manager;

  LightSource light_source;
  light_source.mutable_spot();

  std::variant<ReferenceCounted<Light>, ReferenceCounted<EnvironmentalLight>>
      result = ParseLightSource(light_source, std::filesystem::current_path(),
                                model_to_world, spectrum_manager);
  EXPECT_FALSE(std::get<ReferenceCounted<Light>>(result));
}

}  // namespace
}  // namespace pbrt_frontend
}  // namespace iris
