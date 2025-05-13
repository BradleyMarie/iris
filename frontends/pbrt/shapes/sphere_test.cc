#include "frontends/pbrt/shapes/sphere.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/emissive_material.h"
#include "iris/geometry.h"
#include "iris/material.h"
#include "iris/matrix.h"
#include "iris/normal_map.h"
#include "iris/reference_counted.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace shapes {
namespace {

using ::pbrt_proto::v3::Shape;
using ::testing::ExitedWithCode;

TEST(MakeSphere, Empty) {
  Shape::Sphere sphere;

  auto result = MakeSphere(
      sphere, Matrix::Identity(), ReferenceCounted<Material>(),
      ReferenceCounted<Material>(), ReferenceCounted<EmissiveMaterial>(),
      ReferenceCounted<EmissiveMaterial>(), ReferenceCounted<NormalMap>(),
      ReferenceCounted<NormalMap>());
  EXPECT_EQ(1u, result.first.size());
}

TEST(MakeSphere, BadRadius) {
  Shape::Sphere sphere;
  sphere.set_radius(-1.0);

  EXPECT_EXIT(
      MakeSphere(sphere, Matrix::Identity(), ReferenceCounted<Material>(),
                 ReferenceCounted<Material>(),
                 ReferenceCounted<EmissiveMaterial>(),
                 ReferenceCounted<EmissiveMaterial>(),
                 ReferenceCounted<NormalMap>(), ReferenceCounted<NormalMap>()),
      ExitedWithCode(EXIT_FAILURE),
      "ERROR: Out of range value for parameter: radius");
}

TEST(MakeSphere, ZeroRadius) {
  Shape::Sphere sphere;
  sphere.set_radius(0.0);

  auto result = MakeSphere(
      sphere, Matrix::Identity(), ReferenceCounted<Material>(),
      ReferenceCounted<Material>(), ReferenceCounted<EmissiveMaterial>(),
      ReferenceCounted<EmissiveMaterial>(), ReferenceCounted<NormalMap>(),
      ReferenceCounted<NormalMap>());
  EXPECT_TRUE(result.first.empty());
}

}  // namespace
}  // namespace shapes
}  // namespace pbrt_frontend
}  // namespace iris
