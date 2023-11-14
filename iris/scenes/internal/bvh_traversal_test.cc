#include "iris/scenes/internal/bvh_traversal.h"

#include <vector>

#include "googletest/include/gtest/gtest.h"
#include "iris/geometry/mock_geometry.h"
#include "iris/reference_counted.h"
#include "iris/scene_objects.h"
#include "iris/scenes/internal/bvh_builder.h"
#include "iris/testing/hit_allocator.h"
#include "iris/testing/intersector.h"

static const iris::face_t kFaces[] = {0u};

TEST(SceneIntersect, FirstNegative) {
  auto mock_geometry0 =
      iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  iris::BoundingBox bounds0(iris::Point(0.0, 0.0, 0.0),
                            iris::Point(0.5, 0.5, 1.0));
  EXPECT_CALL(*mock_geometry0, GetFaces())
      .WillRepeatedly(testing::Return(std::span{kFaces}));
  EXPECT_CALL(*mock_geometry0, IsEmissive(testing::_))
      .WillRepeatedly(testing::Return(false));
  EXPECT_CALL(*mock_geometry0, ComputeBounds(iris::Matrix::Identity()))
      .WillRepeatedly(testing::Return(bounds0));

  auto mock_geometry1 =
      iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  iris::BoundingBox bounds1(iris::Point(2.0, 2.0, 3.0),
                            iris::Point(2.5, 2.5, 4.0));
  EXPECT_CALL(*mock_geometry1, GetFaces())
      .WillRepeatedly(testing::Return(std::span{kFaces}));
  EXPECT_CALL(*mock_geometry1, IsEmissive(testing::_))
      .WillRepeatedly(testing::Return(false));
  EXPECT_CALL(*mock_geometry1, ComputeBounds(iris::Matrix::Identity()))
      .WillRepeatedly(testing::Return(bounds1));

  iris::SceneObjects::Builder scene_objects_builder;
  scene_objects_builder.Add(mock_geometry0);
  scene_objects_builder.Add(mock_geometry1);
  auto scene_objects = scene_objects_builder.Build();

  auto get_geometry = [&scene_objects](size_t index) {
    return scene_objects.GetGeometry(index);
  };

  auto result = iris::scenes::internal::BuildBVH(get_geometry,
                                                 scene_objects.NumGeometry());
  scene_objects.Reorder(result.geometry_sort_order);

  EXPECT_CALL(*mock_geometry0, Trace(testing::_, testing::_))
      .Times(1)
      .WillOnce(testing::Return(nullptr));
  EXPECT_CALL(*mock_geometry1, Trace(testing::_, testing::_)).Times(0);

  iris::Ray ray(iris::Point(0.25, 0.25, 5.5), iris::Vector(0.0, 0.0, -1.0));
  iris::Hit* closest_hit = nullptr;
  auto intersector =
      iris::testing::MakeIntersector(ray, 0.0, 100.0, closest_hit);

  iris::scenes::internal::Intersect(result.bvh[0], scene_objects, ray,
                                    intersector);
}

TEST(SceneIntersect, FirstPositive) {
  auto mock_geometry0 =
      iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  iris::BoundingBox bounds0(iris::Point(0.0, 0.0, 0.0),
                            iris::Point(0.5, 0.5, 1.0));
  EXPECT_CALL(*mock_geometry0, GetFaces())
      .WillRepeatedly(testing::Return(std::span{kFaces}));
  EXPECT_CALL(*mock_geometry0, IsEmissive(testing::_))
      .WillRepeatedly(testing::Return(false));
  EXPECT_CALL(*mock_geometry0, ComputeBounds(iris::Matrix::Identity()))
      .WillRepeatedly(testing::Return(bounds0));

  auto mock_geometry1 =
      iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  iris::BoundingBox bounds1(iris::Point(2.0, 2.0, 3.0),
                            iris::Point(2.5, 2.5, 4.0));
  EXPECT_CALL(*mock_geometry1, GetFaces())
      .WillRepeatedly(testing::Return(std::span{kFaces}));
  EXPECT_CALL(*mock_geometry1, IsEmissive(testing::_))
      .WillRepeatedly(testing::Return(false));
  EXPECT_CALL(*mock_geometry1, ComputeBounds(iris::Matrix::Identity()))
      .WillRepeatedly(testing::Return(bounds1));

  iris::SceneObjects::Builder scene_objects_builder;
  scene_objects_builder.Add(mock_geometry0);
  scene_objects_builder.Add(mock_geometry1);
  auto scene_objects = scene_objects_builder.Build();

  auto get_geometry = [&scene_objects](size_t index) {
    return scene_objects.GetGeometry(index);
  };

  auto result = iris::scenes::internal::BuildBVH(get_geometry,
                                                 scene_objects.NumGeometry());
  scene_objects.Reorder(result.geometry_sort_order);

  EXPECT_CALL(*mock_geometry0, Trace(testing::_, testing::_))
      .Times(1)
      .WillOnce(testing::Return(nullptr));
  EXPECT_CALL(*mock_geometry1, Trace(testing::_, testing::_)).Times(0);

  iris::Ray ray(iris::Point(0.25, 0.25, -5.5), iris::Vector(0.0, 0.0, 1.0));
  iris::Hit* closest_hit = nullptr;
  auto intersector =
      iris::testing::MakeIntersector(ray, 0.0, 100.0, closest_hit);

  iris::scenes::internal::Intersect(result.bvh[0], scene_objects, ray,
                                    intersector);
}

TEST(SceneIntersect, SecondNegative) {
  auto mock_geometry0 =
      iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  iris::BoundingBox bounds0(iris::Point(0.0, 0.0, 0.0),
                            iris::Point(0.5, 0.5, 1.0));
  EXPECT_CALL(*mock_geometry0, GetFaces())
      .WillRepeatedly(testing::Return(std::span{kFaces}));
  EXPECT_CALL(*mock_geometry0, IsEmissive(testing::_))
      .WillRepeatedly(testing::Return(false));
  EXPECT_CALL(*mock_geometry0, ComputeBounds(iris::Matrix::Identity()))
      .WillRepeatedly(testing::Return(bounds0));

  auto mock_geometry1 =
      iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  iris::BoundingBox bounds1(iris::Point(2.0, 2.0, 3.0),
                            iris::Point(2.5, 2.5, 4.0));
  EXPECT_CALL(*mock_geometry1, GetFaces())
      .WillRepeatedly(testing::Return(std::span{kFaces}));
  EXPECT_CALL(*mock_geometry1, IsEmissive(testing::_))
      .WillRepeatedly(testing::Return(false));
  EXPECT_CALL(*mock_geometry1, ComputeBounds(iris::Matrix::Identity()))
      .WillRepeatedly(testing::Return(bounds1));

  iris::SceneObjects::Builder scene_objects_builder;
  scene_objects_builder.Add(mock_geometry0);
  scene_objects_builder.Add(mock_geometry1);
  auto scene_objects = scene_objects_builder.Build();

  auto get_geometry = [&scene_objects](size_t index) {
    return scene_objects.GetGeometry(index);
  };

  auto result = iris::scenes::internal::BuildBVH(get_geometry,
                                                 scene_objects.NumGeometry());
  scene_objects.Reorder(result.geometry_sort_order);

  EXPECT_CALL(*mock_geometry1, Trace(testing::_, testing::_)).Times(0);
  EXPECT_CALL(*mock_geometry1, Trace(testing::_, testing::_))
      .Times(1)
      .WillOnce(testing::Return(nullptr));

  iris::Ray ray(iris::Point(2.25, 2.25, 5.5), iris::Vector(0.0, 0.0, -1.0));
  iris::Hit* closest_hit = nullptr;
  auto intersector =
      iris::testing::MakeIntersector(ray, 0.0, 100.0, closest_hit);

  iris::scenes::internal::Intersect(result.bvh[0], scene_objects, ray,
                                    intersector);
}

TEST(SceneIntersect, SecondPositive) {
  auto mock_geometry0 =
      iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  iris::BoundingBox bounds0(iris::Point(0.0, 0.0, 0.0),
                            iris::Point(0.5, 0.5, 1.0));
  EXPECT_CALL(*mock_geometry0, GetFaces())
      .WillRepeatedly(testing::Return(std::span{kFaces}));
  EXPECT_CALL(*mock_geometry0, IsEmissive(testing::_))
      .WillRepeatedly(testing::Return(false));
  EXPECT_CALL(*mock_geometry0, ComputeBounds(iris::Matrix::Identity()))
      .WillRepeatedly(testing::Return(bounds0));

  auto mock_geometry1 =
      iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  iris::BoundingBox bounds1(iris::Point(2.0, 2.0, 3.0),
                            iris::Point(2.5, 2.5, 4.0));
  EXPECT_CALL(*mock_geometry1, GetFaces())
      .WillRepeatedly(testing::Return(std::span{kFaces}));
  EXPECT_CALL(*mock_geometry1, IsEmissive(testing::_))
      .WillRepeatedly(testing::Return(false));
  EXPECT_CALL(*mock_geometry1, ComputeBounds(iris::Matrix::Identity()))
      .WillRepeatedly(testing::Return(bounds1));

  iris::SceneObjects::Builder scene_objects_builder;
  scene_objects_builder.Add(mock_geometry0);
  scene_objects_builder.Add(mock_geometry1);
  auto scene_objects = scene_objects_builder.Build();

  auto get_geometry = [&scene_objects](size_t index) {
    return scene_objects.GetGeometry(index);
  };

  auto result = iris::scenes::internal::BuildBVH(get_geometry,
                                                 scene_objects.NumGeometry());
  scene_objects.Reorder(result.geometry_sort_order);

  EXPECT_CALL(*mock_geometry1, Trace(testing::_, testing::_)).Times(0);
  EXPECT_CALL(*mock_geometry1, Trace(testing::_, testing::_))
      .Times(1)
      .WillOnce(testing::Return(nullptr));

  iris::Ray ray(iris::Point(2.25, 2.25, -5.5), iris::Vector(0.0, 0.0, 1.0));
  iris::Hit* closest_hit = nullptr;
  auto intersector =
      iris::testing::MakeIntersector(ray, 0.0, 100.0, closest_hit);

  iris::scenes::internal::Intersect(result.bvh[0], scene_objects, ray,
                                    intersector);
}

TEST(SceneIntersect, Overlapped) {
  auto mock_geometry0 =
      iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  iris::BoundingBox bounds0(iris::Point(2.0, 2.0, 3.0),
                            iris::Point(2.5, 2.5, 4.0));
  EXPECT_CALL(*mock_geometry0, GetFaces())
      .WillRepeatedly(testing::Return(std::span{kFaces}));
  EXPECT_CALL(*mock_geometry0, IsEmissive(testing::_))
      .WillRepeatedly(testing::Return(false));
  EXPECT_CALL(*mock_geometry0, ComputeBounds(iris::Matrix::Identity()))
      .WillRepeatedly(testing::Return(bounds0));

  auto mock_geometry1 =
      iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  iris::BoundingBox bounds1(iris::Point(2.0, 2.0, 3.0),
                            iris::Point(2.5, 2.5, 4.0));
  EXPECT_CALL(*mock_geometry1, GetFaces())
      .WillRepeatedly(testing::Return(std::span{kFaces}));
  EXPECT_CALL(*mock_geometry1, IsEmissive(testing::_))
      .WillRepeatedly(testing::Return(false));
  EXPECT_CALL(*mock_geometry1, ComputeBounds(iris::Matrix::Identity()))
      .WillRepeatedly(testing::Return(bounds1));

  iris::SceneObjects::Builder scene_objects_builder;
  scene_objects_builder.Add(mock_geometry0);
  scene_objects_builder.Add(mock_geometry1);
  auto scene_objects = scene_objects_builder.Build();

  auto get_geometry = [&scene_objects](size_t index) {
    return scene_objects.GetGeometry(index);
  };

  auto result = iris::scenes::internal::BuildBVH(get_geometry,
                                                 scene_objects.NumGeometry());
  scene_objects.Reorder(result.geometry_sort_order);

  EXPECT_CALL(*mock_geometry0, Trace(testing::_, testing::_))
      .Times(1)
      .WillOnce(testing::Return(nullptr));
  EXPECT_CALL(*mock_geometry1, Trace(testing::_, testing::_))
      .Times(1)
      .WillOnce(testing::Return(nullptr));

  iris::Ray ray(iris::Point(2.25, 2.25, -5.5), iris::Vector(0.0, 0.0, 1.0));
  iris::Hit* closest_hit = nullptr;
  auto intersector =
      iris::testing::MakeIntersector(ray, 0.0, 100.0, closest_hit);

  iris::scenes::internal::Intersect(result.bvh[0], scene_objects, ray,
                                    intersector);
}

TEST(NestedIntersect, NoHit) {
  auto mock_geometry0 =
      iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  iris::BoundingBox bounds0(iris::Point(0.0, 0.0, 0.0),
                            iris::Point(0.5, 0.5, 1.0));
  EXPECT_CALL(*mock_geometry0, ComputeBounds(iris::Matrix::Identity()))
      .WillRepeatedly(testing::Return(bounds0));

  auto mock_geometry1 =
      iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  iris::BoundingBox bounds1(iris::Point(2.0, 2.0, 3.0),
                            iris::Point(2.5, 2.5, 4.0));
  EXPECT_CALL(*mock_geometry1, ComputeBounds(iris::Matrix::Identity()))
      .WillRepeatedly(testing::Return(bounds1));

  std::vector<iris::ReferenceCounted<iris::Geometry>> geometry;
  geometry.push_back(mock_geometry0);
  geometry.push_back(mock_geometry1);

  auto get_geometry = [&geometry](size_t index) {
    return std::pair<const iris::Geometry&, const iris::Matrix*>(
        *geometry.at(index).Get(), nullptr);
  };

  auto result = iris::scenes::internal::BuildBVH(get_geometry, geometry.size());

  std::vector<iris::ReferenceCounted<iris::Geometry>> sorted_geometry;
  sorted_geometry.resize(geometry.size());
  for (size_t index = 0; index < result.geometry_sort_order.size(); index++) {
    sorted_geometry[result.geometry_sort_order[index]] = geometry[index];
  }

  iris::Ray ray(iris::Point(0.25, 0.25, 5.5), iris::Vector(0.0, 0.0, -1.0));
  auto hit_allocator = iris::testing::MakeHitAllocator(ray);

  EXPECT_CALL(*mock_geometry0, Trace(testing::_, testing::_))
      .Times(1)
      .WillOnce(testing::Return(nullptr));
  EXPECT_CALL(*mock_geometry1, Trace(testing::_, testing::_)).Times(0);

  auto* hit = iris::scenes::internal::Intersect(result.bvh[0], sorted_geometry,
                                                ray, hit_allocator);
  EXPECT_EQ(nullptr, hit);
}

TEST(NestedIntersect, FirstNegative) {
  auto mock_geometry0 =
      iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  iris::BoundingBox bounds0(iris::Point(0.0, 0.0, 0.0),
                            iris::Point(0.5, 0.5, 1.0));
  EXPECT_CALL(*mock_geometry0, ComputeBounds(iris::Matrix::Identity()))
      .WillRepeatedly(testing::Return(bounds0));

  auto mock_geometry1 =
      iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  iris::BoundingBox bounds1(iris::Point(2.0, 2.0, 3.0),
                            iris::Point(2.5, 2.5, 4.0));
  EXPECT_CALL(*mock_geometry1, ComputeBounds(iris::Matrix::Identity()))
      .WillRepeatedly(testing::Return(bounds1));

  std::vector<iris::ReferenceCounted<iris::Geometry>> geometry;
  geometry.push_back(mock_geometry0);
  geometry.push_back(mock_geometry1);

  auto get_geometry = [&geometry](size_t index) {
    return std::pair<const iris::Geometry&, const iris::Matrix*>(
        *geometry.at(index).Get(), nullptr);
  };

  auto result = iris::scenes::internal::BuildBVH(get_geometry, geometry.size());

  std::vector<iris::ReferenceCounted<iris::Geometry>> sorted_geometry;
  sorted_geometry.resize(geometry.size());
  for (size_t index = 0; index < result.geometry_sort_order.size(); index++) {
    sorted_geometry[result.geometry_sort_order[index]] = geometry[index];
  }

  iris::Ray ray(iris::Point(0.25, 0.25, 5.5), iris::Vector(0.0, 0.0, -1.0));
  auto hit_allocator = iris::testing::MakeHitAllocator(ray);

  auto& expected_hit = hit_allocator.Allocate(nullptr, -1.0, 0u, 0u);

  EXPECT_CALL(*mock_geometry0, Trace(testing::_, testing::_))
      .Times(1)
      .WillOnce(testing::Return(&expected_hit));
  EXPECT_CALL(*mock_geometry1, Trace(testing::_, testing::_)).Times(0);

  auto* actual_hit = iris::scenes::internal::Intersect(
      result.bvh[0], sorted_geometry, ray, hit_allocator);
  EXPECT_EQ(nullptr, actual_hit);
}

TEST(NestedIntersect, FirstPositive) {
  auto mock_geometry0 =
      iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  iris::BoundingBox bounds0(iris::Point(0.0, 0.0, 0.0),
                            iris::Point(0.5, 0.5, 1.0));
  EXPECT_CALL(*mock_geometry0, ComputeBounds(iris::Matrix::Identity()))
      .WillRepeatedly(testing::Return(bounds0));

  auto mock_geometry1 =
      iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  iris::BoundingBox bounds1(iris::Point(2.0, 2.0, 3.0),
                            iris::Point(2.5, 2.5, 4.0));
  EXPECT_CALL(*mock_geometry1, ComputeBounds(iris::Matrix::Identity()))
      .WillRepeatedly(testing::Return(bounds1));

  std::vector<iris::ReferenceCounted<iris::Geometry>> geometry;
  geometry.push_back(mock_geometry0);
  geometry.push_back(mock_geometry1);

  auto get_geometry = [&geometry](size_t index) {
    return std::pair<const iris::Geometry&, const iris::Matrix*>(
        *geometry.at(index).Get(), nullptr);
  };

  auto result = iris::scenes::internal::BuildBVH(get_geometry, geometry.size());

  std::vector<iris::ReferenceCounted<iris::Geometry>> sorted_geometry;
  sorted_geometry.resize(geometry.size());
  for (size_t index = 0; index < result.geometry_sort_order.size(); index++) {
    sorted_geometry[result.geometry_sort_order[index]] = geometry[index];
  }

  iris::Ray ray(iris::Point(0.25, 0.25, 5.5), iris::Vector(0.0, 0.0, -1.0));
  auto hit_allocator = iris::testing::MakeHitAllocator(ray);

  auto& expected_hit = hit_allocator.Allocate(nullptr, 1.0, 0u, 0u);

  EXPECT_CALL(*mock_geometry0, Trace(testing::_, testing::_))
      .Times(1)
      .WillOnce(testing::Return(&expected_hit));
  EXPECT_CALL(*mock_geometry1, Trace(testing::_, testing::_)).Times(0);

  auto* actual_hit = iris::scenes::internal::Intersect(
      result.bvh[0], sorted_geometry, ray, hit_allocator);
  EXPECT_EQ(&expected_hit, actual_hit);
}

TEST(NestedIntersect, SecondNegative) {
  auto mock_geometry0 =
      iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  iris::BoundingBox bounds0(iris::Point(0.0, 0.0, 0.0),
                            iris::Point(0.5, 0.5, 1.0));
  EXPECT_CALL(*mock_geometry0, ComputeBounds(iris::Matrix::Identity()))
      .WillRepeatedly(testing::Return(bounds0));

  auto mock_geometry1 =
      iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  iris::BoundingBox bounds1(iris::Point(2.0, 2.0, 3.0),
                            iris::Point(2.5, 2.5, 4.0));
  EXPECT_CALL(*mock_geometry1, ComputeBounds(iris::Matrix::Identity()))
      .WillRepeatedly(testing::Return(bounds1));

  std::vector<iris::ReferenceCounted<iris::Geometry>> geometry;
  geometry.push_back(mock_geometry0);
  geometry.push_back(mock_geometry1);

  auto get_geometry = [&geometry](size_t index) {
    return std::pair<const iris::Geometry&, const iris::Matrix*>(
        *geometry.at(index).Get(), nullptr);
  };

  auto result = iris::scenes::internal::BuildBVH(get_geometry, geometry.size());

  std::vector<iris::ReferenceCounted<iris::Geometry>> sorted_geometry;
  sorted_geometry.resize(geometry.size());
  for (size_t index = 0; index < result.geometry_sort_order.size(); index++) {
    sorted_geometry[result.geometry_sort_order[index]] = geometry[index];
  }

  iris::Ray ray(iris::Point(2.25, 2.25, 5.5), iris::Vector(0.0, 0.0, -1.0));
  auto hit_allocator = iris::testing::MakeHitAllocator(ray);

  auto& expected_hit = hit_allocator.Allocate(nullptr, -1.0, 0u, 0u);

  EXPECT_CALL(*mock_geometry1, Trace(testing::_, testing::_)).Times(0);
  EXPECT_CALL(*mock_geometry1, Trace(testing::_, testing::_))
      .Times(1)
      .WillOnce(testing::Return(&expected_hit));

  auto* actual_hit = iris::scenes::internal::Intersect(
      result.bvh[0], sorted_geometry, ray, hit_allocator);
  EXPECT_EQ(nullptr, actual_hit);
}

TEST(NestedIntersect, SecondPositive) {
  auto mock_geometry0 =
      iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  iris::BoundingBox bounds0(iris::Point(0.0, 0.0, 0.0),
                            iris::Point(0.5, 0.5, 1.0));
  EXPECT_CALL(*mock_geometry0, ComputeBounds(iris::Matrix::Identity()))
      .WillRepeatedly(testing::Return(bounds0));

  auto mock_geometry1 =
      iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  iris::BoundingBox bounds1(iris::Point(2.0, 2.0, 3.0),
                            iris::Point(2.5, 2.5, 4.0));
  EXPECT_CALL(*mock_geometry1, ComputeBounds(iris::Matrix::Identity()))
      .WillRepeatedly(testing::Return(bounds1));

  std::vector<iris::ReferenceCounted<iris::Geometry>> geometry;
  geometry.push_back(mock_geometry0);
  geometry.push_back(mock_geometry1);

  auto get_geometry = [&geometry](size_t index) {
    return std::pair<const iris::Geometry&, const iris::Matrix*>(
        *geometry.at(index).Get(), nullptr);
  };

  auto result = iris::scenes::internal::BuildBVH(get_geometry, geometry.size());

  std::vector<iris::ReferenceCounted<iris::Geometry>> sorted_geometry;
  sorted_geometry.resize(geometry.size());
  for (size_t index = 0; index < result.geometry_sort_order.size(); index++) {
    sorted_geometry[result.geometry_sort_order[index]] = geometry[index];
  }

  iris::Ray ray(iris::Point(2.25, 2.25, -5.5), iris::Vector(0.0, 0.0, 1.0));
  auto hit_allocator = iris::testing::MakeHitAllocator(ray);

  auto& expected_hit = hit_allocator.Allocate(nullptr, 1.0, 0u, 0u);

  EXPECT_CALL(*mock_geometry1, Trace(testing::_, testing::_)).Times(0);
  EXPECT_CALL(*mock_geometry1, Trace(testing::_, testing::_))
      .Times(1)
      .WillOnce(testing::Return(&expected_hit));

  auto* actual_hit = iris::scenes::internal::Intersect(
      result.bvh[0], sorted_geometry, ray, hit_allocator);
  EXPECT_EQ(&expected_hit, actual_hit);
}

TEST(NestedIntersect, Overlapped) {
  auto mock_geometry0 =
      iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  iris::BoundingBox bounds0(iris::Point(2.0, 2.0, 3.0),
                            iris::Point(2.5, 2.5, 4.0));
  EXPECT_CALL(*mock_geometry0, ComputeBounds(iris::Matrix::Identity()))
      .WillRepeatedly(testing::Return(bounds0));

  auto mock_geometry1 =
      iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  iris::BoundingBox bounds1(iris::Point(2.0, 2.0, 3.0),
                            iris::Point(2.5, 2.5, 4.0));
  EXPECT_CALL(*mock_geometry1, ComputeBounds(iris::Matrix::Identity()))
      .WillRepeatedly(testing::Return(bounds1));

  std::vector<iris::ReferenceCounted<iris::Geometry>> geometry;
  geometry.push_back(mock_geometry0);
  geometry.push_back(mock_geometry1);

  auto get_geometry = [&geometry](size_t index) {
    return std::pair<const iris::Geometry&, const iris::Matrix*>(
        *geometry.at(index).Get(), nullptr);
  };

  auto result = iris::scenes::internal::BuildBVH(get_geometry, geometry.size());

  std::vector<iris::ReferenceCounted<iris::Geometry>> sorted_geometry;
  sorted_geometry.resize(geometry.size());
  for (size_t index = 0; index < result.geometry_sort_order.size(); index++) {
    sorted_geometry[result.geometry_sort_order[index]] = geometry[index];
  }

  iris::Ray ray(iris::Point(2.25, 2.25, -5.5), iris::Vector(0.0, 0.0, 1.0));
  auto hit_allocator = iris::testing::MakeHitAllocator(ray);

  auto& first_hit = hit_allocator.Allocate(nullptr, 2.0, 0u, 0u);
  auto& expected_hit = hit_allocator.Allocate(nullptr, 1.0, 0u, 0u);

  EXPECT_CALL(*mock_geometry0, Trace(testing::_, testing::_))
      .Times(1)
      .WillOnce(testing::Return(&first_hit));
  EXPECT_CALL(*mock_geometry1, Trace(testing::_, testing::_))
      .Times(1)
      .WillOnce(testing::Return(&expected_hit));

  auto* actual_hit = iris::scenes::internal::Intersect(
      result.bvh[0], sorted_geometry, ray, hit_allocator);
  EXPECT_EQ(&expected_hit, actual_hit);
}