#include "iris/hit_point.h"

#include <array>
#include <cmath>

#include "googletest/include/gtest/gtest.h"

TEST(HitPoint, ApproximateLocation) {
  iris::Point point(0.0, 0.0, 0.0);
  std::array<iris::geometric, 3> error = {1.0, 1.0, 1.0};
  iris::Vector surface_normal(1.0, 0.0, 0.0);
  iris::HitPoint hit_point(point, error, surface_normal);
  EXPECT_EQ(point, hit_point.ApproximateLocation());
}

TEST(HitPoint, CreateRay) {
  iris::Point point(0.0, 0.0, 0.0);
  std::array<iris::geometric, 3> error = {1.0, 1.0, 1.0};
  iris::Vector surface_normal_x(1.0, 0.0, 0.0);
  iris::Vector surface_normal_y(0.0, 1.0, 0.0);
  iris::Vector surface_normal_z(0.0, 0.0, 1.0);

  iris::HitPoint hit_point_x(point, error, surface_normal_x);
  iris::HitPoint hit_point_y(point, error, surface_normal_y);
  iris::HitPoint hit_point_z(point, error, surface_normal_z);

  iris::Vector direction(1.0, 1.0, 1.0);
  iris::Vector reverse_direction(-1.0, -1.0, -1.0);

  iris::Ray rx = hit_point_x.CreateRay(direction);
  EXPECT_EQ(rx.origin.x, std::nextafter(static_cast<iris::geometric>(1.0),
                                        static_cast<iris::geometric>(2.0)));
  EXPECT_EQ(rx.origin.y, std::nextafter(static_cast<iris::geometric>(0.0),
                                        static_cast<iris::geometric>(1.0)));
  EXPECT_EQ(rx.origin.z, std::nextafter(static_cast<iris::geometric>(0.0),
                                        static_cast<iris::geometric>(1.0)));
  EXPECT_EQ(rx.direction, direction);
  iris::Ray reverse_rx = hit_point_x.CreateRay(reverse_direction);
  EXPECT_EQ(reverse_rx.origin.x,
            std::nextafter(static_cast<iris::geometric>(-1.0),
                           static_cast<iris::geometric>(-2.0)));
  EXPECT_EQ(reverse_rx.origin.y,
            std::nextafter(static_cast<iris::geometric>(0.0),
                           static_cast<iris::geometric>(-1.0)));
  EXPECT_EQ(reverse_rx.origin.z,
            std::nextafter(static_cast<iris::geometric>(0.0),
                           static_cast<iris::geometric>(-1.0)));
  EXPECT_EQ(reverse_rx.direction, reverse_direction);

  iris::Ray ry = hit_point_y.CreateRay(direction);
  EXPECT_EQ(ry.origin.x, std::nextafter(static_cast<iris::geometric>(0.0),
                                        static_cast<iris::geometric>(1.0)));
  EXPECT_EQ(ry.origin.y, std::nextafter(static_cast<iris::geometric>(1.0),
                                        static_cast<iris::geometric>(2.0)));
  EXPECT_EQ(ry.origin.z, std::nextafter(static_cast<iris::geometric>(0.0),
                                        static_cast<iris::geometric>(1.0)));
  EXPECT_EQ(ry.direction, direction);
  iris::Ray reverse_ry = hit_point_y.CreateRay(reverse_direction);
  EXPECT_EQ(reverse_ry.origin.x,
            std::nextafter(static_cast<iris::geometric>(0.0),
                           static_cast<iris::geometric>(-1.0)));
  EXPECT_EQ(reverse_ry.origin.y,
            std::nextafter(static_cast<iris::geometric>(-1.0),
                           static_cast<iris::geometric>(-2.0)));
  EXPECT_EQ(reverse_ry.origin.z,
            std::nextafter(static_cast<iris::geometric>(0.0),
                           static_cast<iris::geometric>(-1.0)));
  EXPECT_EQ(reverse_ry.direction, reverse_direction);

  iris::Ray rz = hit_point_z.CreateRay(direction);
  EXPECT_EQ(rz.origin.x, std::nextafter(static_cast<iris::geometric>(0.0),
                                        static_cast<iris::geometric>(1.0)));
  EXPECT_EQ(rz.origin.y, std::nextafter(static_cast<iris::geometric>(0.0),
                                        static_cast<iris::geometric>(1.0)));
  EXPECT_EQ(rz.origin.z, std::nextafter(static_cast<iris::geometric>(1.0),
                                        static_cast<iris::geometric>(2.0)));
  EXPECT_EQ(rz.direction, direction);
  iris::Ray reverse_rz = hit_point_z.CreateRay(reverse_direction);
  EXPECT_EQ(reverse_rz.origin.x,
            std::nextafter(static_cast<iris::geometric>(0.0),
                           static_cast<iris::geometric>(-1.0)));
  EXPECT_EQ(reverse_rz.origin.y,
            std::nextafter(static_cast<iris::geometric>(0.0),
                           static_cast<iris::geometric>(-1.0)));
  EXPECT_EQ(reverse_rz.origin.z,
            std::nextafter(static_cast<iris::geometric>(-1.0),
                           static_cast<iris::geometric>(-2.0)));
  EXPECT_EQ(reverse_rz.direction, reverse_direction);
}

TEST(HitPoint, CreateRayTo) {
  iris::Point point(0.0, 0.0, 0.0);
  std::array<iris::geometric, 3> error = {1.0, 1.0, 1.0};
  iris::Vector surface_normal(1.0, 0.0, 0.0);
  iris::HitPoint hit_point(point, error, surface_normal);

  iris::Point to_point(2.0, 0.0, 0.0);
  iris::Ray rx = hit_point.CreateRayTo(to_point);
  EXPECT_EQ(rx.origin.x, std::nextafter(static_cast<iris::geometric>(1.0),
                                        static_cast<iris::geometric>(2.0)));
  EXPECT_EQ(rx.origin.y, std::nextafter(static_cast<iris::geometric>(0.0),
                                        static_cast<iris::geometric>(1.0)));
  EXPECT_EQ(rx.origin.z, std::nextafter(static_cast<iris::geometric>(0.0),
                                        static_cast<iris::geometric>(1.0)));
  EXPECT_EQ(rx.direction, iris::Vector(1.0, 0.0, 0.0));
}

TEST(HitPoint, CreateRayToWithDistance) {
  iris::Point point(0.0, 0.0, 0.0);
  std::array<iris::geometric, 3> error = {1.0, 1.0, 1.0};
  iris::Vector surface_normal(1.0, 0.0, 0.0);
  iris::HitPoint hit_point(point, error, surface_normal);

  iris::Point to_point(2.0, 0.0, 0.0);

  iris::geometric_t distance;
  iris::Ray rx = hit_point.CreateRayTo(to_point, &distance);
  EXPECT_EQ(rx.origin.x, std::nextafter(static_cast<iris::geometric>(1.0),
                                        static_cast<iris::geometric>(2.0)));
  EXPECT_EQ(rx.origin.y, std::nextafter(static_cast<iris::geometric>(0.0),
                                        static_cast<iris::geometric>(1.0)));
  EXPECT_EQ(rx.origin.z, std::nextafter(static_cast<iris::geometric>(0.0),
                                        static_cast<iris::geometric>(1.0)));
  EXPECT_EQ(rx.direction, iris::Vector(1.0, 0.0, 0.0));
  EXPECT_EQ(distance, 2.0);
}