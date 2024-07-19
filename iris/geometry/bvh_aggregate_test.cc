#include "iris/geometry/bvh_aggregate.h"

#include <vector>

#include "googletest/include/gtest/gtest.h"
#include "iris/geometry/mock_geometry.h"
#include "iris/geometry/sphere.h"
#include "iris/materials/mock_material.h"
#include "iris/testing/hit_allocator.h"

iris::ReferenceCounted<iris::Geometry> MakeSphere(iris::geometric x_origin) {
  static const iris::ReferenceCounted<iris::Material> front_material =
      iris::MakeReferenceCounted<iris::materials::MockMaterial>();
  static const iris::ReferenceCounted<iris::Material> back_material =
      iris::MakeReferenceCounted<iris::materials::MockMaterial>();

  return iris::geometry::AllocateSphere(
      iris::Point(x_origin, 0.0, 0.0), 1.0, front_material, back_material,
      iris::ReferenceCounted<iris::EmissiveMaterial>(),
      iris::ReferenceCounted<iris::EmissiveMaterial>(),
      iris::ReferenceCounted<iris::NormalMap>(),
      iris::ReferenceCounted<iris::NormalMap>());
}

TEST(BVHAggregate, NoShapes) {
  std::vector<iris::ReferenceCounted<iris::Geometry>> shapes;
  EXPECT_EQ(nullptr, iris::geometry::AllocateBVHAggregate(shapes).Get());
}

TEST(BVHAggregate, OneShape) {
  iris::ReferenceCounted<iris::Geometry> shapes[1] = {
      iris::MakeReferenceCounted<iris::geometry::MockGeometry>()};
  EXPECT_EQ(shapes[0].Get(),
            iris::geometry::AllocateBVHAggregate(shapes).Get());
}

TEST(BVHAggregate, ComputeBounds) {
  iris::ReferenceCounted<iris::Geometry> spheres[2] = {MakeSphere(-0.5),
                                                       MakeSphere(0.5)};
  auto aggregate = iris::geometry::AllocateBVHAggregate(spheres);

  iris::BoundingBox::Builder bounds_builder;
  bounds_builder.Add(spheres[0]->ComputeBounds(nullptr));
  bounds_builder.Add(spheres[1]->ComputeBounds(nullptr));
  auto bounds = bounds_builder.Build();

  auto computed_bounds = aggregate->ComputeBounds(nullptr);
  EXPECT_EQ(bounds.lower, computed_bounds.lower);
  EXPECT_EQ(bounds.upper, computed_bounds.upper);

  auto rotation = iris::Matrix::Rotation(0.5, 0.5, 0.5, 0.5).value();
  auto expected_rotated_bounds = rotation.Multiply(bounds);
  auto actual_rotated_bounds = aggregate->ComputeBounds(&rotation);
  EXPECT_EQ(expected_rotated_bounds.lower, actual_rotated_bounds.lower);
  EXPECT_EQ(expected_rotated_bounds.upper, actual_rotated_bounds.upper);
}

TEST(BVHAggregate, FirstSphere) {
  iris::ReferenceCounted<iris::Geometry> spheres[2] = {MakeSphere(-0.5),
                                                       MakeSphere(0.5)};
  auto aggregate = iris::geometry::AllocateBVHAggregate(spheres);

  iris::Point origin(-2.0, 0.0, 0.0);
  iris::Vector direction(1.0, 0.0, 0.0);
  iris::Ray ray(origin, direction);

  auto hit_allocator = iris::testing::MakeHitAllocator(ray);
  auto* hit = aggregate->Trace(hit_allocator);
  EXPECT_EQ(hit->distance, 0.5);
}

TEST(BVHAggregate, SecondSphere) {
  iris::ReferenceCounted<iris::Geometry> spheres[2] = {MakeSphere(-0.5),
                                                       MakeSphere(0.5)};
  auto aggregate = iris::geometry::AllocateBVHAggregate(spheres);

  iris::Point origin(2.0, 0.0, 0.0);
  iris::Vector direction(-1.0, 0.0, 0.0);
  iris::Ray ray(origin, direction);

  auto hit_allocator = iris::testing::MakeHitAllocator(ray);
  auto* hit = aggregate->Trace(hit_allocator);
  EXPECT_EQ(hit->distance, 0.5);
}