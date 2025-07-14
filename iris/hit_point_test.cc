#include "iris/hit_point.h"

#include <cmath>

#include "googletest/include/gtest/gtest.h"
#include "iris/float.h"
#include "iris/point.h"
#include "iris/position_error.h"
#include "iris/ray.h"
#include "iris/vector.h"

namespace iris {
namespace {

TEST(HitPoint, ApproximateLocation) {
  Point point(0.0, 0.0, 0.0);
  PositionError error(1.0, 1.0, 1.0);
  Vector surface_normal(1.0, 0.0, 0.0);
  HitPoint hit_point(point, error, surface_normal);
  EXPECT_EQ(point, hit_point.ApproximateLocation());
}

TEST(HitPoint, CreateRay) {
  Point point(0.0, 0.0, 0.0);
  PositionError error(1.0, 1.0, 1.0);
  Vector surface_normal_x(1.0, 0.0, 0.0);
  Vector surface_normal_y(0.0, 1.0, 0.0);
  Vector surface_normal_z(0.0, 0.0, 1.0);

  HitPoint hit_point_x(point, error, surface_normal_x);
  HitPoint hit_point_y(point, error, surface_normal_y);
  HitPoint hit_point_z(point, error, surface_normal_z);

  Vector direction(1.0, 1.0, 1.0);
  Vector reverse_direction(-1.0, -1.0, -1.0);

  Ray rx = hit_point_x.CreateRay(direction);
  EXPECT_EQ(rx.origin.x, std::nextafter(static_cast<geometric>(1.0),
                                        static_cast<geometric>(2.0)));
  EXPECT_EQ(rx.origin.y, std::nextafter(static_cast<geometric>(0.0),
                                        static_cast<geometric>(1.0)));
  EXPECT_EQ(rx.origin.z, std::nextafter(static_cast<geometric>(0.0),
                                        static_cast<geometric>(1.0)));
  EXPECT_EQ(rx.direction, direction);
  Ray reverse_rx = hit_point_x.CreateRay(reverse_direction);
  EXPECT_EQ(reverse_rx.origin.x, std::nextafter(static_cast<geometric>(-1.0),
                                                static_cast<geometric>(-2.0)));
  EXPECT_EQ(reverse_rx.origin.y, std::nextafter(static_cast<geometric>(0.0),
                                                static_cast<geometric>(-1.0)));
  EXPECT_EQ(reverse_rx.origin.z, std::nextafter(static_cast<geometric>(0.0),
                                                static_cast<geometric>(-1.0)));
  EXPECT_EQ(reverse_rx.direction, reverse_direction);

  Ray ry = hit_point_y.CreateRay(direction);
  EXPECT_EQ(ry.origin.x, std::nextafter(static_cast<geometric>(0.0),
                                        static_cast<geometric>(1.0)));
  EXPECT_EQ(ry.origin.y, std::nextafter(static_cast<geometric>(1.0),
                                        static_cast<geometric>(2.0)));
  EXPECT_EQ(ry.origin.z, std::nextafter(static_cast<geometric>(0.0),
                                        static_cast<geometric>(1.0)));
  EXPECT_EQ(ry.direction, direction);
  Ray reverse_ry = hit_point_y.CreateRay(reverse_direction);
  EXPECT_EQ(reverse_ry.origin.x, std::nextafter(static_cast<geometric>(0.0),
                                                static_cast<geometric>(-1.0)));
  EXPECT_EQ(reverse_ry.origin.y, std::nextafter(static_cast<geometric>(-1.0),
                                                static_cast<geometric>(-2.0)));
  EXPECT_EQ(reverse_ry.origin.z, std::nextafter(static_cast<geometric>(0.0),
                                                static_cast<geometric>(-1.0)));
  EXPECT_EQ(reverse_ry.direction, reverse_direction);

  Ray rz = hit_point_z.CreateRay(direction);
  EXPECT_EQ(rz.origin.x, std::nextafter(static_cast<geometric>(0.0),
                                        static_cast<geometric>(1.0)));
  EXPECT_EQ(rz.origin.y, std::nextafter(static_cast<geometric>(0.0),
                                        static_cast<geometric>(1.0)));
  EXPECT_EQ(rz.origin.z, std::nextafter(static_cast<geometric>(1.0),
                                        static_cast<geometric>(2.0)));
  EXPECT_EQ(rz.direction, direction);
  Ray reverse_rz = hit_point_z.CreateRay(reverse_direction);
  EXPECT_EQ(reverse_rz.origin.x, std::nextafter(static_cast<geometric>(0.0),
                                                static_cast<geometric>(-1.0)));
  EXPECT_EQ(reverse_rz.origin.y, std::nextafter(static_cast<geometric>(0.0),
                                                static_cast<geometric>(-1.0)));
  EXPECT_EQ(reverse_rz.origin.z, std::nextafter(static_cast<geometric>(-1.0),
                                                static_cast<geometric>(-2.0)));
  EXPECT_EQ(reverse_rz.direction, reverse_direction);
}

TEST(HitPoint, CreateRayTo) {
  Point point(0.0, 0.0, 0.0);
  PositionError error(1.0, 1.0, 1.0);
  Vector surface_normal(1.0, 0.0, 0.0);
  HitPoint hit_point(point, error, surface_normal);

  Point to_point(2.0, 0.0, 0.0);
  Ray rx = hit_point.CreateRayTo(to_point);
  EXPECT_EQ(rx.origin.x, std::nextafter(static_cast<geometric>(1.0),
                                        static_cast<geometric>(2.0)));
  EXPECT_EQ(rx.origin.y, std::nextafter(static_cast<geometric>(0.0),
                                        static_cast<geometric>(1.0)));
  EXPECT_EQ(rx.origin.z, std::nextafter(static_cast<geometric>(0.0),
                                        static_cast<geometric>(1.0)));
  EXPECT_EQ(rx.direction, Vector(1.0, 0.0, 0.0));
}

TEST(HitPoint, CreateRayToWithDistances) {
  Point point(0.0, 0.0, 0.0);
  PositionError error(1.0, 1.0, 1.0);
  Vector surface_normal(1.0, 0.0, 0.0);
  HitPoint hit_point(point, error, surface_normal);

  Point to_point(2.0, 0.0, 0.0);

  geometric_t distance_squared, distance;
  Ray rx = hit_point.CreateRayTo(to_point, &distance_squared, &distance);
  EXPECT_EQ(rx.origin.x, std::nextafter(static_cast<geometric>(1.0),
                                        static_cast<geometric>(2.0)));
  EXPECT_EQ(rx.origin.y, std::nextafter(static_cast<geometric>(0.0),
                                        static_cast<geometric>(1.0)));
  EXPECT_EQ(rx.origin.z, std::nextafter(static_cast<geometric>(0.0),
                                        static_cast<geometric>(1.0)));
  EXPECT_EQ(rx.direction, Vector(1.0, 0.0, 0.0));
  EXPECT_EQ(distance_squared, 4.0);
  EXPECT_EQ(distance, 2.0);
}

}  // namespace
}  // namespace iris
