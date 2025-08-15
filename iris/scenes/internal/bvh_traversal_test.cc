#include "iris/scenes/internal/bvh_traversal.h"

#include <span>
#include <vector>

#include "googletest/include/gtest/gtest.h"
#include "iris/bounding_box.h"
#include "iris/geometry.h"
#include "iris/geometry/mock_geometry.h"
#include "iris/hit.h"
#include "iris/hit_allocator.h"
#include "iris/integer.h"
#include "iris/intersector.h"
#include "iris/point.h"
#include "iris/ray.h"
#include "iris/reference_counted.h"
#include "iris/scene_objects.h"
#include "iris/scenes/internal/bvh_builder.h"
#include "iris/testing/hit_allocator.h"
#include "iris/testing/intersector.h"
#include "iris/vector.h"

namespace iris {
namespace scenes {
namespace internal {
namespace {

using ::iris::geometry::MockGeometry;
using ::iris::scenes::internal::BuildBVH;
using ::iris::scenes::internal::BuildBVHResult;
using ::iris::scenes::internal::Intersect;
using ::iris::testing::MakeHitAllocator;
using ::iris::testing::MakeIntersector;
using ::testing::_;
using ::testing::Return;

static const face_t kFaces[] = {0u};

TEST(SceneIntersect, FirstNegative) {
  ReferenceCounted<MockGeometry> mock_geometry0 =
      MakeReferenceCounted<MockGeometry>();
  BoundingBox bounds0(Point(0.0, 0.0, 0.0), Point(0.5, 0.5, 1.0));
  EXPECT_CALL(*mock_geometry0, GetFaces())
      .WillRepeatedly(Return(std::span{kFaces}));
  EXPECT_CALL(*mock_geometry0, ComputeBounds(nullptr))
      .WillRepeatedly(Return(bounds0));

  ReferenceCounted<MockGeometry> mock_geometry1 =
      MakeReferenceCounted<MockGeometry>();
  BoundingBox bounds1(Point(2.0, 2.0, 3.0), Point(2.5, 2.5, 4.0));
  EXPECT_CALL(*mock_geometry1, GetFaces())
      .WillRepeatedly(Return(std::span{kFaces}));
  EXPECT_CALL(*mock_geometry1, ComputeBounds(nullptr))
      .WillRepeatedly(Return(bounds1));

  SceneObjects::Builder scene_objects_builder;
  scene_objects_builder.Add(mock_geometry0);
  scene_objects_builder.Add(mock_geometry1);
  SceneObjects scene_objects = scene_objects_builder.Build();

  BuildBVHResult result =
      BuildBVH([&scene_objects](
                   size_t index) { return scene_objects.GetGeometry(index); },
               scene_objects.NumGeometry(), /*for_scene=*/false);
  scene_objects.Reorder(result.geometry_sort_order);

  EXPECT_CALL(*mock_geometry0, Trace(_, _, _, _, _))
      .Times(1)
      .WillOnce(Return(nullptr));
  EXPECT_CALL(*mock_geometry1, Trace(_, _, _, _, _)).Times(0);

  Ray ray(Point(0.25, 0.25, 5.5), Vector(0.0, 0.0, -1.0));
  Hit* closest_hit = nullptr;
  Intersector intersector = MakeIntersector(ray, 0.0, 100.0, closest_hit);

  Intersect(result.bvh[0], scene_objects, ray, intersector);
}

TEST(SceneIntersect, FirstPositive) {
  ReferenceCounted<MockGeometry> mock_geometry0 =
      MakeReferenceCounted<MockGeometry>();
  BoundingBox bounds0(Point(0.0, 0.0, 0.0), Point(0.5, 0.5, 1.0));
  EXPECT_CALL(*mock_geometry0, GetFaces())
      .WillRepeatedly(Return(std::span{kFaces}));
  EXPECT_CALL(*mock_geometry0, ComputeBounds(nullptr))
      .WillRepeatedly(Return(bounds0));

  ReferenceCounted<MockGeometry> mock_geometry1 =
      MakeReferenceCounted<MockGeometry>();
  BoundingBox bounds1(Point(2.0, 2.0, 3.0), Point(2.5, 2.5, 4.0));
  EXPECT_CALL(*mock_geometry1, GetFaces())
      .WillRepeatedly(Return(std::span{kFaces}));
  EXPECT_CALL(*mock_geometry1, ComputeBounds(nullptr))
      .WillRepeatedly(Return(bounds1));

  SceneObjects::Builder scene_objects_builder;
  scene_objects_builder.Add(mock_geometry0);
  scene_objects_builder.Add(mock_geometry1);
  SceneObjects scene_objects = scene_objects_builder.Build();

  BuildBVHResult result =
      BuildBVH([&scene_objects](
                   size_t index) { return scene_objects.GetGeometry(index); },
               scene_objects.NumGeometry(), /*for_scene=*/false);
  scene_objects.Reorder(result.geometry_sort_order);

  EXPECT_CALL(*mock_geometry0, Trace(_, _, _, _, _))
      .Times(1)
      .WillOnce(Return(nullptr));
  EXPECT_CALL(*mock_geometry1, Trace(_, _, _, _, _)).Times(0);

  Ray ray(Point(0.25, 0.25, -5.5), Vector(0.0, 0.0, 1.0));
  Hit* closest_hit = nullptr;
  Intersector intersector = MakeIntersector(ray, 0.0, 100.0, closest_hit);

  Intersect(result.bvh[0], scene_objects, ray, intersector);
}

TEST(SceneIntersect, SecondNegative) {
  ReferenceCounted<MockGeometry> mock_geometry0 =
      MakeReferenceCounted<MockGeometry>();
  BoundingBox bounds0(Point(0.0, 0.0, 0.0), Point(0.5, 0.5, 1.0));
  EXPECT_CALL(*mock_geometry0, GetFaces())
      .WillRepeatedly(Return(std::span{kFaces}));
  EXPECT_CALL(*mock_geometry0, ComputeBounds(nullptr))
      .WillRepeatedly(Return(bounds0));

  ReferenceCounted<MockGeometry> mock_geometry1 =
      MakeReferenceCounted<MockGeometry>();
  BoundingBox bounds1(Point(2.0, 2.0, 3.0), Point(2.5, 2.5, 4.0));
  EXPECT_CALL(*mock_geometry1, GetFaces())
      .WillRepeatedly(Return(std::span{kFaces}));
  EXPECT_CALL(*mock_geometry1, ComputeBounds(nullptr))
      .WillRepeatedly(Return(bounds1));

  SceneObjects::Builder scene_objects_builder;
  scene_objects_builder.Add(mock_geometry0);
  scene_objects_builder.Add(mock_geometry1);
  SceneObjects scene_objects = scene_objects_builder.Build();

  BuildBVHResult result =
      BuildBVH([&scene_objects](
                   size_t index) { return scene_objects.GetGeometry(index); },
               scene_objects.NumGeometry(), /*for_scene=*/false);
  scene_objects.Reorder(result.geometry_sort_order);

  EXPECT_CALL(*mock_geometry1, Trace(_, _, _, _, _)).Times(0);
  EXPECT_CALL(*mock_geometry1, Trace(_, _, _, _, _))
      .Times(1)
      .WillOnce(Return(nullptr));

  Ray ray(Point(2.25, 2.25, 5.5), Vector(0.0, 0.0, -1.0));
  Hit* closest_hit = nullptr;
  Intersector intersector = MakeIntersector(ray, 0.0, 100.0, closest_hit);

  Intersect(result.bvh[0], scene_objects, ray, intersector);
}

TEST(SceneIntersect, SecondPositive) {
  ReferenceCounted<MockGeometry> mock_geometry0 =
      MakeReferenceCounted<MockGeometry>();
  BoundingBox bounds0(Point(0.0, 0.0, 0.0), Point(0.5, 0.5, 1.0));
  EXPECT_CALL(*mock_geometry0, GetFaces())
      .WillRepeatedly(Return(std::span{kFaces}));
  EXPECT_CALL(*mock_geometry0, ComputeBounds(nullptr))
      .WillRepeatedly(Return(bounds0));

  ReferenceCounted<MockGeometry> mock_geometry1 =
      MakeReferenceCounted<MockGeometry>();
  BoundingBox bounds1(Point(2.0, 2.0, 3.0), Point(2.5, 2.5, 4.0));
  EXPECT_CALL(*mock_geometry1, GetFaces())
      .WillRepeatedly(Return(std::span{kFaces}));
  EXPECT_CALL(*mock_geometry1, ComputeBounds(nullptr))
      .WillRepeatedly(Return(bounds1));

  SceneObjects::Builder scene_objects_builder;
  scene_objects_builder.Add(mock_geometry0);
  scene_objects_builder.Add(mock_geometry1);
  SceneObjects scene_objects = scene_objects_builder.Build();

  BuildBVHResult result =
      BuildBVH([&scene_objects](
                   size_t index) { return scene_objects.GetGeometry(index); },
               scene_objects.NumGeometry(), /*for_scene=*/false);
  scene_objects.Reorder(result.geometry_sort_order);

  EXPECT_CALL(*mock_geometry1, Trace(_, _, _, _, _)).Times(0);
  EXPECT_CALL(*mock_geometry1, Trace(_, _, _, _, _))
      .Times(1)
      .WillOnce(Return(nullptr));

  Ray ray(Point(2.25, 2.25, -5.5), Vector(0.0, 0.0, 1.0));
  Hit* closest_hit = nullptr;
  Intersector intersector = MakeIntersector(ray, 0.0, 100.0, closest_hit);

  Intersect(result.bvh[0], scene_objects, ray, intersector);
}

TEST(SceneIntersect, Overlapped) {
  ReferenceCounted<MockGeometry> mock_geometry0 =
      MakeReferenceCounted<MockGeometry>();
  BoundingBox bounds0(Point(2.0, 2.0, 3.0), Point(2.5, 2.5, 4.0));
  EXPECT_CALL(*mock_geometry0, GetFaces())
      .WillRepeatedly(Return(std::span{kFaces}));
  EXPECT_CALL(*mock_geometry0, ComputeBounds(nullptr))
      .WillRepeatedly(Return(bounds0));

  ReferenceCounted<MockGeometry> mock_geometry1 =
      MakeReferenceCounted<MockGeometry>();
  BoundingBox bounds1(Point(2.0, 2.0, 3.0), Point(2.5, 2.5, 4.0));
  EXPECT_CALL(*mock_geometry1, GetFaces())
      .WillRepeatedly(Return(std::span{kFaces}));
  EXPECT_CALL(*mock_geometry1, ComputeBounds(nullptr))
      .WillRepeatedly(Return(bounds1));

  SceneObjects::Builder scene_objects_builder;
  scene_objects_builder.Add(mock_geometry0);
  scene_objects_builder.Add(mock_geometry1);
  SceneObjects scene_objects = scene_objects_builder.Build();

  BuildBVHResult result =
      BuildBVH([&scene_objects](
                   size_t index) { return scene_objects.GetGeometry(index); },
               scene_objects.NumGeometry(), /*for_scene=*/false);
  scene_objects.Reorder(result.geometry_sort_order);

  EXPECT_CALL(*mock_geometry0, Trace(_, _, _, _, _))
      .Times(1)
      .WillOnce(Return(nullptr));
  EXPECT_CALL(*mock_geometry1, Trace(_, _, _, _, _))
      .Times(1)
      .WillOnce(Return(nullptr));

  Ray ray(Point(2.25, 2.25, -5.5), Vector(0.0, 0.0, 1.0));
  Hit* closest_hit = nullptr;
  Intersector intersector = MakeIntersector(ray, 0.0, 100.0, closest_hit);

  Intersect(result.bvh[0], scene_objects, ray, intersector);
}

TEST(NestedIntersect, NoHit) {
  ReferenceCounted<MockGeometry> mock_geometry0 =
      MakeReferenceCounted<MockGeometry>();
  BoundingBox bounds0(Point(0.0, 0.0, 0.0), Point(0.5, 0.5, 1.0));
  EXPECT_CALL(*mock_geometry0, ComputeBounds(nullptr))
      .WillRepeatedly(Return(bounds0));

  ReferenceCounted<MockGeometry> mock_geometry1 =
      MakeReferenceCounted<MockGeometry>();
  BoundingBox bounds1(Point(2.0, 2.0, 3.0), Point(2.5, 2.5, 4.0));
  EXPECT_CALL(*mock_geometry1, ComputeBounds(nullptr))
      .WillRepeatedly(Return(bounds1));

  std::vector<ReferenceCounted<Geometry>> geometry;
  geometry.push_back(mock_geometry0);
  geometry.push_back(mock_geometry1);

  BuildBVHResult result = BuildBVH(
      [&geometry](size_t index) {
        return std::pair<const Geometry&, const Matrix*>(
            *geometry.at(index).Get(), nullptr);
      },
      geometry.size(), /*for_scene=*/false);

  std::vector<ReferenceCounted<Geometry>> sorted_geometry;
  sorted_geometry.resize(geometry.size());
  for (size_t index = 0; index < result.geometry_sort_order.size(); index++) {
    sorted_geometry[result.geometry_sort_order[index]] = geometry[index];
  }

  Ray ray(Point(0.25, 0.25, 5.5), Vector(0.0, 0.0, -1.0));
  HitAllocator hit_allocator = MakeHitAllocator(ray);

  EXPECT_CALL(*mock_geometry0, Trace(_, _, _, _, _))
      .Times(1)
      .WillOnce(Return(nullptr));
  EXPECT_CALL(*mock_geometry1, Trace(_, _, _, _, _)).Times(0);

  Hit* hit = Intersect(
      result.bvh[0], sorted_geometry, ray, static_cast<geometric_t>(0.0),
      static_cast<geometric_t>(10000.0), Geometry::CLOSEST_HIT, hit_allocator);
  EXPECT_EQ(nullptr, hit);
}

TEST(NestedIntersect, FirstNegative) {
  ReferenceCounted<MockGeometry> mock_geometry0 =
      MakeReferenceCounted<MockGeometry>();
  BoundingBox bounds0(Point(0.0, 0.0, 0.0), Point(0.5, 0.5, 1.0));
  EXPECT_CALL(*mock_geometry0, ComputeBounds(nullptr))
      .WillRepeatedly(Return(bounds0));

  ReferenceCounted<MockGeometry> mock_geometry1 =
      MakeReferenceCounted<MockGeometry>();
  BoundingBox bounds1(Point(2.0, 2.0, 3.0), Point(2.5, 2.5, 4.0));
  EXPECT_CALL(*mock_geometry1, ComputeBounds(nullptr))
      .WillRepeatedly(Return(bounds1));

  std::vector<ReferenceCounted<Geometry>> geometry;
  geometry.push_back(mock_geometry0);
  geometry.push_back(mock_geometry1);

  BuildBVHResult result = BuildBVH(
      [&geometry](size_t index) {
        return std::pair<const Geometry&, const Matrix*>(
            *geometry.at(index).Get(), nullptr);
      },
      geometry.size(), /*for_scene=*/false);

  std::vector<ReferenceCounted<Geometry>> sorted_geometry;
  sorted_geometry.resize(geometry.size());
  for (size_t index = 0; index < result.geometry_sort_order.size(); index++) {
    sorted_geometry[result.geometry_sort_order[index]] = geometry[index];
  }

  Ray ray(Point(0.25, 0.25, 5.5), Vector(0.0, 0.0, -1.0));
  HitAllocator hit_allocator = MakeHitAllocator(ray);

  Hit& expected_hit = hit_allocator.Allocate(nullptr, -1.0, 0.0, 0u, 0u, false);

  EXPECT_CALL(*mock_geometry0, Trace(_, _, _, _, _))
      .Times(1)
      .WillOnce(Return(&expected_hit));
  EXPECT_CALL(*mock_geometry1, Trace(_, _, _, _, _)).Times(0);

  Hit* actual_hit = Intersect(
      result.bvh[0], sorted_geometry, ray, static_cast<geometric_t>(0.0),
      static_cast<geometric_t>(10000.0), Geometry::CLOSEST_HIT, hit_allocator);
  EXPECT_EQ(nullptr, actual_hit);
}

TEST(NestedIntersect, FirstPositive) {
  ReferenceCounted<MockGeometry> mock_geometry0 =
      MakeReferenceCounted<MockGeometry>();
  BoundingBox bounds0(Point(0.0, 0.0, 0.0), Point(0.5, 0.5, 1.0));
  EXPECT_CALL(*mock_geometry0, ComputeBounds(nullptr))
      .WillRepeatedly(Return(bounds0));

  ReferenceCounted<MockGeometry> mock_geometry1 =
      MakeReferenceCounted<MockGeometry>();
  BoundingBox bounds1(Point(2.0, 2.0, 3.0), Point(2.5, 2.5, 4.0));
  EXPECT_CALL(*mock_geometry1, ComputeBounds(nullptr))
      .WillRepeatedly(Return(bounds1));

  std::vector<ReferenceCounted<Geometry>> geometry;
  geometry.push_back(mock_geometry0);
  geometry.push_back(mock_geometry1);

  BuildBVHResult result = BuildBVH(
      [&geometry](size_t index) {
        return std::pair<const Geometry&, const Matrix*>(
            *geometry.at(index).Get(), nullptr);
      },
      geometry.size(), /*for_scene=*/false);

  std::vector<ReferenceCounted<Geometry>> sorted_geometry;
  sorted_geometry.resize(geometry.size());
  for (size_t index = 0; index < result.geometry_sort_order.size(); index++) {
    sorted_geometry[result.geometry_sort_order[index]] = geometry[index];
  }

  Ray ray(Point(0.25, 0.25, 5.5), Vector(0.0, 0.0, -1.0));
  HitAllocator hit_allocator = MakeHitAllocator(ray);

  Hit& expected_hit = hit_allocator.Allocate(nullptr, 1.0, 0.0, 0u, 0u, false);

  EXPECT_CALL(*mock_geometry0, Trace(_, _, _, _, _))
      .Times(1)
      .WillOnce(Return(&expected_hit));
  EXPECT_CALL(*mock_geometry1, Trace(_, _, _, _, _)).Times(0);

  Hit* actual_hit = Intersect(
      result.bvh[0], sorted_geometry, ray, static_cast<geometric_t>(0.0),
      static_cast<geometric_t>(10000.0), Geometry::CLOSEST_HIT, hit_allocator);
  EXPECT_EQ(&expected_hit, actual_hit);
}

TEST(NestedIntersect, SecondNegative) {
  ReferenceCounted<MockGeometry> mock_geometry0 =
      MakeReferenceCounted<MockGeometry>();
  BoundingBox bounds0(Point(0.0, 0.0, 0.0), Point(0.5, 0.5, 1.0));
  EXPECT_CALL(*mock_geometry0, ComputeBounds(nullptr))
      .WillRepeatedly(Return(bounds0));

  ReferenceCounted<MockGeometry> mock_geometry1 =
      MakeReferenceCounted<MockGeometry>();
  BoundingBox bounds1(Point(2.0, 2.0, 3.0), Point(2.5, 2.5, 4.0));
  EXPECT_CALL(*mock_geometry1, ComputeBounds(nullptr))
      .WillRepeatedly(Return(bounds1));

  std::vector<ReferenceCounted<Geometry>> geometry;
  geometry.push_back(mock_geometry0);
  geometry.push_back(mock_geometry1);

  BuildBVHResult result = BuildBVH(
      [&geometry](size_t index) {
        return std::pair<const Geometry&, const Matrix*>(
            *geometry.at(index).Get(), nullptr);
      },
      geometry.size(), /*for_scene=*/false);

  std::vector<ReferenceCounted<Geometry>> sorted_geometry;
  sorted_geometry.resize(geometry.size());
  for (size_t index = 0; index < result.geometry_sort_order.size(); index++) {
    sorted_geometry[result.geometry_sort_order[index]] = geometry[index];
  }

  Ray ray(Point(2.25, 2.25, 5.5), Vector(0.0, 0.0, -1.0));
  HitAllocator hit_allocator = MakeHitAllocator(ray);

  Hit& expected_hit = hit_allocator.Allocate(nullptr, -1.0, 0.0, 0u, 0u, false);

  EXPECT_CALL(*mock_geometry1, Trace(_, _, _, _, _)).Times(0);
  EXPECT_CALL(*mock_geometry1, Trace(_, _, _, _, _))
      .Times(1)
      .WillOnce(Return(&expected_hit));

  Hit* actual_hit = Intersect(
      result.bvh[0], sorted_geometry, ray, static_cast<geometric_t>(0.0),
      static_cast<geometric_t>(10000.0), Geometry::CLOSEST_HIT, hit_allocator);
  EXPECT_EQ(nullptr, actual_hit);
}

TEST(NestedIntersect, SecondPositive) {
  ReferenceCounted<MockGeometry> mock_geometry0 =
      MakeReferenceCounted<MockGeometry>();
  BoundingBox bounds0(Point(0.0, 0.0, 0.0), Point(0.5, 0.5, 1.0));
  EXPECT_CALL(*mock_geometry0, ComputeBounds(nullptr))
      .WillRepeatedly(Return(bounds0));

  ReferenceCounted<MockGeometry> mock_geometry1 =
      MakeReferenceCounted<MockGeometry>();
  BoundingBox bounds1(Point(2.0, 2.0, 3.0), Point(2.5, 2.5, 4.0));
  EXPECT_CALL(*mock_geometry1, ComputeBounds(nullptr))
      .WillRepeatedly(Return(bounds1));

  std::vector<ReferenceCounted<Geometry>> geometry;
  geometry.push_back(mock_geometry0);
  geometry.push_back(mock_geometry1);

  BuildBVHResult result = BuildBVH(
      [&geometry](size_t index) {
        return std::pair<const Geometry&, const Matrix*>(
            *geometry.at(index).Get(), nullptr);
      },
      geometry.size(), /*for_scene=*/false);

  std::vector<ReferenceCounted<Geometry>> sorted_geometry;
  sorted_geometry.resize(geometry.size());
  for (size_t index = 0; index < result.geometry_sort_order.size(); index++) {
    sorted_geometry[result.geometry_sort_order[index]] = geometry[index];
  }

  Ray ray(Point(2.25, 2.25, -5.5), Vector(0.0, 0.0, 1.0));
  HitAllocator hit_allocator = MakeHitAllocator(ray);

  Hit& expected_hit = hit_allocator.Allocate(nullptr, 1.0, 0.0, 0u, 0u, false);

  EXPECT_CALL(*mock_geometry1, Trace(_, _, _, _, _)).Times(0);
  EXPECT_CALL(*mock_geometry1, Trace(_, _, _, _, _))
      .Times(1)
      .WillOnce(Return(&expected_hit));

  Hit* actual_hit = Intersect(
      result.bvh[0], sorted_geometry, ray, static_cast<geometric_t>(0.0),
      static_cast<geometric_t>(10000.0), Geometry::CLOSEST_HIT, hit_allocator);
  EXPECT_EQ(&expected_hit, actual_hit);
}

TEST(NestedIntersect, OverlappedClosest) {
  ReferenceCounted<MockGeometry> mock_geometry0 =
      MakeReferenceCounted<MockGeometry>();
  BoundingBox bounds0(Point(2.0, 2.0, 3.0), Point(2.5, 2.5, 4.0));
  EXPECT_CALL(*mock_geometry0, ComputeBounds(nullptr))
      .WillRepeatedly(Return(bounds0));

  ReferenceCounted<MockGeometry> mock_geometry1 =
      MakeReferenceCounted<MockGeometry>();
  BoundingBox bounds1(Point(2.0, 2.0, 3.0), Point(2.5, 2.5, 4.0));
  EXPECT_CALL(*mock_geometry1, ComputeBounds(nullptr))
      .WillRepeatedly(Return(bounds1));

  std::vector<ReferenceCounted<Geometry>> geometry;
  geometry.push_back(mock_geometry0);
  geometry.push_back(mock_geometry1);

  BuildBVHResult result = BuildBVH(
      [&geometry](size_t index) {
        return std::pair<const Geometry&, const Matrix*>(
            *geometry.at(index).Get(), nullptr);
      },
      geometry.size(), /*for_scene=*/false);

  std::vector<ReferenceCounted<Geometry>> sorted_geometry;
  sorted_geometry.resize(geometry.size());
  for (size_t index = 0; index < result.geometry_sort_order.size(); index++) {
    sorted_geometry[result.geometry_sort_order[index]] = geometry[index];
  }

  Ray ray(Point(2.25, 2.25, -5.5), Vector(0.0, 0.0, 1.0));
  HitAllocator hit_allocator = MakeHitAllocator(ray);

  Hit& first_hit = hit_allocator.Allocate(nullptr, 100.0, 0.0, 0u, 0u, false);
  Hit& expected_hit = hit_allocator.Allocate(nullptr, 99.0, 0.0, 0u, 0u, false);

  EXPECT_CALL(*mock_geometry0, Trace(_, _, _, _, _))
      .Times(1)
      .WillOnce(Return(&first_hit));
  EXPECT_CALL(*mock_geometry1, Trace(_, _, _, _, _))
      .Times(1)
      .WillOnce(Return(&expected_hit));

  Hit* actual_hit = Intersect(
      result.bvh[0], sorted_geometry, ray, static_cast<geometric_t>(0.0),
      static_cast<geometric_t>(10000.0), Geometry::CLOSEST_HIT, hit_allocator);
  EXPECT_EQ(&expected_hit, actual_hit);
  EXPECT_EQ(nullptr, actual_hit->next);
}

TEST(NestedIntersect, OverlappedAny) {
  ReferenceCounted<MockGeometry> mock_geometry0 =
      MakeReferenceCounted<MockGeometry>();
  BoundingBox bounds0(Point(2.0, 2.0, 3.0), Point(2.5, 2.5, 4.0));
  EXPECT_CALL(*mock_geometry0, ComputeBounds(nullptr))
      .WillRepeatedly(Return(bounds0));

  ReferenceCounted<MockGeometry> mock_geometry1 =
      MakeReferenceCounted<MockGeometry>();
  BoundingBox bounds1(Point(2.0, 2.0, 3.0), Point(2.5, 2.5, 4.0));
  EXPECT_CALL(*mock_geometry1, ComputeBounds(nullptr))
      .WillRepeatedly(Return(bounds1));

  std::vector<ReferenceCounted<Geometry>> geometry;
  geometry.push_back(mock_geometry0);
  geometry.push_back(mock_geometry1);

  BuildBVHResult result = BuildBVH(
      [&geometry](size_t index) {
        return std::pair<const Geometry&, const Matrix*>(
            *geometry.at(index).Get(), nullptr);
      },
      geometry.size(), /*for_scene=*/false);

  std::vector<ReferenceCounted<Geometry>> sorted_geometry;
  sorted_geometry.resize(geometry.size());
  for (size_t index = 0; index < result.geometry_sort_order.size(); index++) {
    sorted_geometry[result.geometry_sort_order[index]] = geometry[index];
  }

  Ray ray(Point(2.25, 2.25, -5.5), Vector(0.0, 0.0, 1.0));
  HitAllocator hit_allocator = MakeHitAllocator(ray);

  Hit& first_hit = hit_allocator.Allocate(nullptr, 100.0, 0.0, 0u, 0u, false);

  EXPECT_CALL(*mock_geometry0, Trace(_, _, _, _, _))
      .Times(1)
      .WillOnce(Return(&first_hit));
  EXPECT_CALL(*mock_geometry1, Trace(_, _, _, _, _)).Times(0);

  Hit* actual_hit = Intersect(
      result.bvh[0], sorted_geometry, ray, static_cast<geometric_t>(0.0),
      static_cast<geometric_t>(10000.0), Geometry::ANY_HIT, hit_allocator);
  EXPECT_EQ(&first_hit, actual_hit);
  EXPECT_EQ(nullptr, actual_hit->next);
}

TEST(NestedIntersect, OverlappedAll) {
  ReferenceCounted<MockGeometry> mock_geometry0 =
      MakeReferenceCounted<MockGeometry>();
  BoundingBox bounds0(Point(2.0, 2.0, 3.0), Point(2.5, 2.5, 4.0));
  EXPECT_CALL(*mock_geometry0, ComputeBounds(nullptr))
      .WillRepeatedly(Return(bounds0));

  ReferenceCounted<MockGeometry> mock_geometry1 =
      MakeReferenceCounted<MockGeometry>();
  BoundingBox bounds1(Point(2.0, 2.0, 3.0), Point(2.5, 2.5, 4.0));
  EXPECT_CALL(*mock_geometry1, ComputeBounds(nullptr))
      .WillRepeatedly(Return(bounds1));

  std::vector<ReferenceCounted<Geometry>> geometry;
  geometry.push_back(mock_geometry0);
  geometry.push_back(mock_geometry1);

  BuildBVHResult result = BuildBVH(
      [&geometry](size_t index) {
        return std::pair<const Geometry&, const Matrix*>(
            *geometry.at(index).Get(), nullptr);
      },
      geometry.size(), /*for_scene=*/false);

  std::vector<ReferenceCounted<Geometry>> sorted_geometry;
  sorted_geometry.resize(geometry.size());
  for (size_t index = 0; index < result.geometry_sort_order.size(); index++) {
    sorted_geometry[result.geometry_sort_order[index]] = geometry[index];
  }

  Ray ray(Point(2.25, 2.25, -5.5), Vector(0.0, 0.0, 1.0));
  HitAllocator hit_allocator = MakeHitAllocator(ray);

  Hit& second_hit = hit_allocator.Allocate(nullptr, 100.0, 0.0, 0u, 0u, false);
  Hit& first_hit =
      hit_allocator.Allocate(&second_hit, 100.0, 0.0, 0u, 0u, false);
  Hit& third_hit = hit_allocator.Allocate(nullptr, 99.0, 0.0, 0u, 0u, false);

  EXPECT_CALL(*mock_geometry0, Trace(_, _, _, _, _))
      .Times(1)
      .WillOnce(Return(&first_hit));
  EXPECT_CALL(*mock_geometry1, Trace(_, _, _, _, _))
      .Times(1)
      .WillOnce(Return(&third_hit));

  Hit* actual_hit = Intersect(
      result.bvh[0], sorted_geometry, ray, static_cast<geometric_t>(0.0),
      static_cast<geometric_t>(10000.0), Geometry::ALL_HITS, hit_allocator);
  ASSERT_EQ(&first_hit, actual_hit);
  ASSERT_EQ(&second_hit, actual_hit->next);
  ASSERT_EQ(&third_hit, actual_hit->next->next);
  EXPECT_EQ(nullptr, actual_hit->next->next->next);
}

}  // namespace
}  // namespace internal
}  // namespace scenes
}  // namespace iris
