#include "iris/geometry/bvh_aggregate.h"

#include <span>
#include <vector>

#include "googletest/include/gtest/gtest.h"
#include "iris/bounding_box.h"
#include "iris/emissive_material.h"
#include "iris/emissive_materials/mock_emissive_material.h"
#include "iris/float.h"
#include "iris/geometry.h"
#include "iris/geometry/mock_geometry.h"
#include "iris/geometry/sphere.h"
#include "iris/hit.h"
#include "iris/hit_allocator.h"
#include "iris/integer.h"
#include "iris/material.h"
#include "iris/materials/mock_material.h"
#include "iris/matrix.h"
#include "iris/normal_map.h"
#include "iris/point.h"
#include "iris/ray.h"
#include "iris/reference_counted.h"
#include "iris/testing/hit_allocator.h"
#include "iris/vector.h"

namespace iris {
namespace geometry {
namespace {

using ::iris::emissive_materials::MockEmissiveMaterial;
using ::iris::materials::MockMaterial;
using ::iris::testing::MakeHitAllocator;
using ::testing::Return;

ReferenceCounted<Geometry> MakeSphere(geometric x_origin) {
  static const ReferenceCounted<Material> front_material =
      MakeReferenceCounted<MockMaterial>();
  static const ReferenceCounted<Material> back_material =
      MakeReferenceCounted<MockMaterial>();

  return AllocateSphere(Point(x_origin, 0.0, 0.0), 1.0, front_material,
                        back_material, ReferenceCounted<EmissiveMaterial>(),
                        ReferenceCounted<EmissiveMaterial>(),
                        ReferenceCounted<NormalMap>(),
                        ReferenceCounted<NormalMap>());
}

TEST(BVHAggregate, NoShapes) {
  std::vector<ReferenceCounted<Geometry>> shapes;
  EXPECT_EQ(nullptr, AllocateBVHAggregate(shapes).Get());
}

TEST(BVHAggregate, OneShape) {
  ReferenceCounted<MockGeometry> geometry =
      MakeReferenceCounted<MockGeometry>();
  EXPECT_CALL(*geometry, GetFaces()).WillOnce(Return(std::span<face_t>()));
  EXPECT_EQ(geometry.Get(), AllocateBVHAggregate({geometry}).Get());
}

TEST(BVHAggregate, OneEmissiveShape) {
  MockEmissiveMaterial emissive;
  face_t faces[1] = {0};
  ReferenceCounted<MockGeometry> geometry =
      MakeReferenceCounted<MockGeometry>();
  EXPECT_CALL(*geometry, GetFaces()).WillOnce(Return(std::span<face_t>(faces)));
  EXPECT_CALL(*geometry, GetEmissiveMaterial(0)).WillOnce(Return(&emissive));
  EXPECT_NE(geometry.Get(), AllocateBVHAggregate({geometry}).Get());
}

TEST(BVHAggregate, ComputeBounds) {
  std::vector<ReferenceCounted<Geometry>> spheres = {MakeSphere(-0.5),
                                                     MakeSphere(0.5)};
  ReferenceCounted<Geometry> aggregate = AllocateBVHAggregate(spheres);

  BoundingBox::Builder bounds_builder;
  bounds_builder.Add(spheres[0]->ComputeBounds(nullptr));
  bounds_builder.Add(spheres[1]->ComputeBounds(nullptr));
  BoundingBox bounds = bounds_builder.Build();

  BoundingBox computed_bounds = aggregate->ComputeBounds(nullptr);
  EXPECT_EQ(bounds.lower, computed_bounds.lower);
  EXPECT_EQ(bounds.upper, computed_bounds.upper);

  Matrix rotation = Matrix::Rotation(0.5, 0.5, 0.5, 0.5).value();
  BoundingBox expected_rotated_bounds = rotation.Multiply(bounds);
  BoundingBox actual_rotated_bounds = aggregate->ComputeBounds(&rotation);
  EXPECT_EQ(expected_rotated_bounds.lower, actual_rotated_bounds.lower);
  EXPECT_EQ(expected_rotated_bounds.upper, actual_rotated_bounds.upper);
}

TEST(BVHAggregate, FirstSphere) {
  std::vector<ReferenceCounted<Geometry>> spheres = {MakeSphere(-0.5),
                                                     MakeSphere(0.5)};
  ReferenceCounted<Geometry> aggregate = AllocateBVHAggregate(spheres);

  Point origin(-2.0, 0.0, 0.0);
  Vector direction(1.0, 0.0, 0.0);
  Ray ray(origin, direction);

  HitAllocator hit_allocator = MakeHitAllocator(ray);
  Hit* hit = aggregate->Trace(hit_allocator);
  EXPECT_EQ(hit->distance, 0.5);
}

TEST(BVHAggregate, SecondSphere) {
  std::vector<ReferenceCounted<Geometry>> spheres = {MakeSphere(-0.5),
                                                     MakeSphere(0.5)};
  ReferenceCounted<Geometry> aggregate = AllocateBVHAggregate(spheres);

  Point origin(2.0, 0.0, 0.0);
  Vector direction(-1.0, 0.0, 0.0);
  Ray ray(origin, direction);

  HitAllocator hit_allocator = MakeHitAllocator(ray);
  Hit* hit = aggregate->Trace(hit_allocator);
  EXPECT_EQ(hit->distance, 0.5);
}

}  // namespace
}  // namespace geometry
}  // namespace iris
