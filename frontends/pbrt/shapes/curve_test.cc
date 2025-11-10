#include "frontends/pbrt/shapes/curve.h"

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

TEST(MakeCurve, Empty) {
  Shape::Curve curve;

  EXPECT_EXIT(
      MakeCurve(curve, Matrix::Identity(), ReferenceCounted<Material>(),
                ReferenceCounted<Material>(),
                ReferenceCounted<EmissiveMaterial>(),
                ReferenceCounted<EmissiveMaterial>(),
                ReferenceCounted<NormalMap>(), ReferenceCounted<NormalMap>()),
      ExitedWithCode(EXIT_FAILURE),
      "ERROR: Incorrect number of values for parameter: p");
}

TEST(MakeCurve, BezierUnsupported) {
  Shape::Curve curve;
  curve.add_p();
  curve.add_p();
  curve.add_p();
  curve.add_p();
  curve.set_basis(Shape::Curve::BSPLINE);

  EXPECT_EXIT(
      MakeCurve(curve, Matrix::Identity(), ReferenceCounted<Material>(),
                ReferenceCounted<Material>(),
                ReferenceCounted<EmissiveMaterial>(),
                ReferenceCounted<EmissiveMaterial>(),
                ReferenceCounted<NormalMap>(), ReferenceCounted<NormalMap>()),
      ExitedWithCode(EXIT_FAILURE),
      "ERROR: Unsupported value for parameter: basis");
}

TEST(MakeCurve, DegreeUnsupported) {
  Shape::Curve curve;
  curve.add_p();
  curve.add_p();
  curve.add_p();
  curve.add_p();
  curve.set_degree(2);

  EXPECT_EXIT(
      MakeCurve(curve, Matrix::Identity(), ReferenceCounted<Material>(),
                ReferenceCounted<Material>(),
                ReferenceCounted<EmissiveMaterial>(),
                ReferenceCounted<EmissiveMaterial>(),
                ReferenceCounted<NormalMap>(), ReferenceCounted<NormalMap>()),
      ExitedWithCode(EXIT_FAILURE),
      "ERROR: Unsupported value for parameter: degree");
}

TEST(MakeCurve, RibbonUnsupported) {
  Shape::Curve curve;
  curve.add_p();
  curve.add_p();
  curve.add_p();
  curve.add_p();
  curve.set_type(Shape::Curve::RIBBON);

  EXPECT_EXIT(
      MakeCurve(curve, Matrix::Identity(), ReferenceCounted<Material>(),
                ReferenceCounted<Material>(),
                ReferenceCounted<EmissiveMaterial>(),
                ReferenceCounted<EmissiveMaterial>(),
                ReferenceCounted<NormalMap>(), ReferenceCounted<NormalMap>()),
      ExitedWithCode(EXIT_FAILURE),
      "ERROR: Unsupported value for parameter: type");
}

TEST(MakeCurve, SplitDepthTooLow) {
  Shape::Curve curve;
  curve.add_p();
  curve.add_p();
  curve.add_p();
  curve.add_p();
  curve.set_splitdepth(-1);

  EXPECT_EXIT(
      MakeCurve(curve, Matrix::Identity(), ReferenceCounted<Material>(),
                ReferenceCounted<Material>(),
                ReferenceCounted<EmissiveMaterial>(),
                ReferenceCounted<EmissiveMaterial>(),
                ReferenceCounted<NormalMap>(), ReferenceCounted<NormalMap>()),
      ExitedWithCode(EXIT_FAILURE),
      "ERROR: Out of range value for parameter: splitdepth");
}

TEST(MakeCurve, SplitDepthTooHigh) {
  Shape::Curve curve;
  curve.add_p();
  curve.add_p();
  curve.add_p();
  curve.add_p();
  curve.set_splitdepth(32);

  EXPECT_EXIT(
      MakeCurve(curve, Matrix::Identity(), ReferenceCounted<Material>(),
                ReferenceCounted<Material>(),
                ReferenceCounted<EmissiveMaterial>(),
                ReferenceCounted<EmissiveMaterial>(),
                ReferenceCounted<NormalMap>(), ReferenceCounted<NormalMap>()),
      ExitedWithCode(EXIT_FAILURE),
      "ERROR: Out of range value for parameter: splitdepth");
}

TEST(MakeCurve, Valid) {
  Shape::Curve curve;
  pbrt_proto::v3::Point& p0 = *curve.add_p();
  p0.set_x(0.0);
  p0.set_y(0.0);
  p0.set_z(0.0);

  pbrt_proto::v3::Point& p1 = *curve.add_p();
  p1.set_x(1.0);
  p1.set_y(1.0);
  p1.set_z(0.0);

  pbrt_proto::v3::Point& p2 = *curve.add_p();
  p2.set_x(2.0);
  p2.set_y(0.0);
  p2.set_z(0.0);

  pbrt_proto::v3::Point& p3 = *curve.add_p();
  p3.set_x(3.0);
  p3.set_y(1.0);
  p3.set_z(0.0);

  pbrt_proto::v3::Point& p4 = *curve.add_p();
  p4.set_x(4.0);
  p4.set_y(0.0);
  p4.set_z(0.0);

  pbrt_proto::v3::Point& p5 = *curve.add_p();
  p5.set_x(5.0);
  p5.set_y(1.0);
  p5.set_z(0.0);

  pbrt_proto::v3::Point& p6 = *curve.add_p();
  p6.set_x(6.0);
  p6.set_y(0.0);
  p6.set_z(0.0);

  pbrt_proto::v3::Point& p7 = *curve.add_p();
  p7.set_x(7.0);
  p7.set_y(1.0);
  p7.set_z(0.0);

  pbrt_proto::v3::Point& p8 = *curve.add_p();
  p8.set_x(8.0);
  p8.set_y(0.0);
  p8.set_z(0.0);

  auto [shapes, matrix] = MakeCurve(
      curve, Matrix::Identity(), ReferenceCounted<Material>(),
      ReferenceCounted<Material>(), ReferenceCounted<EmissiveMaterial>(),
      ReferenceCounted<EmissiveMaterial>(), ReferenceCounted<NormalMap>(),
      ReferenceCounted<NormalMap>());
  EXPECT_EQ(16u, shapes.size());
}

}  // namespace
}  // namespace shapes
}  // namespace pbrt_frontend
}  // namespace iris
