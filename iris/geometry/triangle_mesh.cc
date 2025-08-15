#include "iris/geometry/triangle_mesh.h"

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <memory>
#include <optional>
#include <span>
#include <tuple>
#include <variant>
#include <vector>

#include "iris/bounding_box.h"
#include "iris/emissive_material.h"
#include "iris/float.h"
#include "iris/geometry.h"
#include "iris/hit_allocator.h"
#include "iris/integer.h"
#include "iris/material.h"
#include "iris/matrix.h"
#include "iris/normal_map.h"
#include "iris/point.h"
#include "iris/position_error.h"
#include "iris/ray.h"
#include "iris/reference_counted.h"
#include "iris/sampler.h"
#include "iris/texture_coordinates.h"
#include "iris/textures/mask_texture.h"
#include "iris/vector.h"

namespace iris {
namespace geometry {
namespace {

std::array<geometric_t, 3> Subtract(const Point& point0, const Point& point1) {
  return std::array<geometric_t, 3>{point0.x - point1.x, point0.y - point1.y,
                                    point0.z - point1.z};
}

std::array<geometric_t, 3> PermuteXDominant(
    const std::array<geometric_t, 3>& point) {
  return std::array<geometric_t, 3>{point[1], point[2], point[0]};
}

std::array<geometric_t, 3> PermuteYDominant(
    const std::array<geometric_t, 3>& point) {
  return std::array<geometric_t, 3>{point[2], point[0], point[1]};
}

Vector DoComputeSurfaceNormal(const Point& p0, const Point& p1,
                              const Point& p2) {
  return CrossProduct(p1 - p0, p2 - p0);
}

geometric_t AbsMax(geometric_t a, geometric_t b, geometric_t c) {
  return std::max(std::max(std::abs(a), std::abs(b)), std::abs(c));
}

Point MaybeMultiply(const Matrix* model_to_world, const Point& point) {
  if (!model_to_world) {
    return point;
  }

  return model_to_world->Multiply(point);
}

class Triangle final : public Geometry {
 public:
  struct SharedData {
    std::vector<Point> points;
    std::vector<Vector> normals;
    std::vector<std::pair<geometric, geometric>> uv;
    ReferenceCounted<textures::MaskTexture> alpha_mask;
    ReferenceCounted<Material> materials[2];
    ReferenceCounted<EmissiveMaterial> emissive_materials[2];
    ReferenceCounted<NormalMap> normal_maps[2];
  };

  struct AdditionalData {
    std::array<geometric_t, 3> barycentric_coordinates;
    Vector surface_normal;
  };

  static constexpr face_t kFrontFace = 0u;
  static constexpr face_t kBackFace = 1u;

  Triangle(const std::tuple<uint32_t, uint32_t, uint32_t>& vertices,
           std::shared_ptr<const SharedData> shared, face_t face_index) noexcept
      : shared_(std::move(shared)),
        vertices_(vertices),
        face_index_(face_index) {}

  Vector ComputeSurfaceNormal(const Point& hit_point, face_t face,
                              const void* additional_data) const override;

  std::optional<TextureCoordinates> ComputeTextureCoordinates(
      const Point& hit_point, const std::optional<Differentials>& differentials,
      face_t face, const void* additional_data) const override;

  Geometry::ComputeShadingNormalResult ComputeShadingNormal(
      face_t face, const void* additional_data) const override;

  ComputeHitPointResult ComputeHitPoint(
      const Ray& ray, geometric_t distance,
      const void* additional_data) const override;

  const Material* GetMaterial(face_t face) const override;

  const EmissiveMaterial* GetEmissiveMaterial(face_t face) const override;

  visual_t ComputeSurfaceArea(face_t face,
                              const Matrix* model_to_world) const override;

  std::variant<std::monostate, Point, Vector> SampleBySolidAngle(
      const Point& origin, face_t face, Sampler& sampler) const override;

  std::optional<visual_t> ComputePdfBySolidAngle(
      const Point& origin, face_t face, const void* additional_data,
      const Point& on_face) const override;

  BoundingBox ComputeBounds(const Matrix* model_to_world) const override;

  std::span<const face_t> GetFaces() const override;

 private:
  Hit* Trace(const Ray& ray, geometric_t minimum_distance,
             geometric_t maximum_distance, TraceMode trace_mode,
             HitAllocator& hit_allocator) const override;

  std::tuple<Vector, face_t, face_t> ComputeSurfaceNormalAndFaces(
      const Ray& ray) const;

  std::optional<Vector> MaybeComputeShadingNormal(
      const void* additional_data) const;

  std::optional<std::pair<Vector, Vector>> MaybeComputeNormalTangents() const;

  std::optional<TextureCoordinates> ComputeTextureCoordinates(
      const Point& hit_point, const std::optional<Differentials>& differentials,
      geometric_t b0, geometric_t b1, geometric_t b2) const;
  std::array<geometric_t, 2> UVCoordinates(const Point& point) const;

  std::shared_ptr<const SharedData> shared_;
  std::tuple<uint32_t, uint32_t, uint32_t> vertices_;
  face_t face_index_;
};

std::tuple<Vector, face_t, face_t> Triangle::ComputeSurfaceNormalAndFaces(
    const Ray& ray) const {
  Vector surface_normal =
      DoComputeSurfaceNormal(shared_->points[std::get<0>(vertices_)],
                             shared_->points[std::get<1>(vertices_)],
                             shared_->points[std::get<2>(vertices_)]);

  if (DotProduct(surface_normal, ray.direction) < 0.0) {
    return {surface_normal, kFrontFace, kBackFace};
  }

  return {-surface_normal, kBackFace, kFrontFace};
}

Vector Triangle::ComputeSurfaceNormal(const Point& hit_point, face_t face,
                                      const void* additional_data) const {
  const AdditionalData* additional =
      static_cast<const AdditionalData*>(additional_data);
  return additional->surface_normal;
}

std::array<geometric_t, 2> Triangle::UVCoordinates(const Point& point) const {
  Vector v0_to_v1 = shared_->points[std::get<1>(vertices_)] -
                    shared_->points[std::get<0>(vertices_)];
  Vector v0_to_v2 = shared_->points[std::get<2>(vertices_)] -
                    shared_->points[std::get<0>(vertices_)];
  Vector v0_to_p = point - shared_->points[std::get<0>(vertices_)];

  geometric_t d00 = DotProduct(v0_to_v1, v0_to_v1);
  geometric_t d01 = DotProduct(v0_to_v1, v0_to_v2);
  geometric_t d11 = DotProduct(v0_to_v2, v0_to_v2);
  geometric_t d20 = DotProduct(v0_to_p, v0_to_v1);
  geometric_t d21 = DotProduct(v0_to_p, v0_to_v2);
  geometric_t denom = d00 * d11 - d01 * d01;

  geometric_t b1 = (d11 * d20 - d01 * d21) / denom;
  geometric_t b2 = (d00 * d21 - d01 * d20) / denom;
  geometric_t b0 = static_cast<geometric_t>(1.0) - b1 - b2;

  geometric_t u = shared_->uv[std::get<0>(vertices_)].first * b0 +
                  shared_->uv[std::get<1>(vertices_)].first * b1 +
                  shared_->uv[std::get<2>(vertices_)].first * b2;
  geometric_t v = shared_->uv[std::get<0>(vertices_)].second * b0 +
                  shared_->uv[std::get<1>(vertices_)].second * b1 +
                  shared_->uv[std::get<2>(vertices_)].second * b2;

  return {u, v};
}

std::optional<Geometry::TextureCoordinates> Triangle::ComputeTextureCoordinates(
    const Point& hit_point, const std::optional<Differentials>& differentials,
    geometric_t b0, geometric_t b1, geometric_t b2) const {
  if (shared_->uv.empty()) {
    return std::nullopt;
  }

  geometric_t u = shared_->uv[std::get<0>(vertices_)].first * b0 +
                  shared_->uv[std::get<1>(vertices_)].first * b1 +
                  shared_->uv[std::get<2>(vertices_)].first * b2;
  geometric_t v = shared_->uv[std::get<0>(vertices_)].second * b0 +
                  shared_->uv[std::get<1>(vertices_)].second * b1 +
                  shared_->uv[std::get<2>(vertices_)].second * b2;

  if (!differentials) {
    return Geometry::TextureCoordinates{face_index_, {u, v}};
  }

  auto [u_dx, v_dx] = UVCoordinates(differentials->dx);
  auto [u_dy, v_dy] = UVCoordinates(differentials->dy);
  return Geometry::TextureCoordinates{face_index_, {u, v},   u_dx - u,
                                      u_dy - u,    v_dx - v, v_dy - v};
}

std::optional<Geometry::TextureCoordinates> Triangle::ComputeTextureCoordinates(
    const Point& hit_point, const std::optional<Differentials>& differentials,
    face_t face, const void* additional_data) const {
  const AdditionalData* additional =
      static_cast<const AdditionalData*>(additional_data);
  return ComputeTextureCoordinates(hit_point, differentials,
                                   additional->barycentric_coordinates[0],
                                   additional->barycentric_coordinates[1],
                                   additional->barycentric_coordinates[2]);
}

std::optional<Vector> Triangle::MaybeComputeShadingNormal(
    const void* additional_data) const {
  if (shared_->normals.empty()) {
    return std::nullopt;
  }

  const AdditionalData* additional =
      static_cast<const AdditionalData*>(additional_data);
  geometric_t b0 = additional->barycentric_coordinates[0];
  geometric_t b1 = additional->barycentric_coordinates[1];
  geometric_t b2 = additional->barycentric_coordinates[2];

  Vector shading_normal = shared_->normals[std::get<0>(vertices_)] * b0 +
                          shared_->normals[std::get<1>(vertices_)] * b1 +
                          shared_->normals[std::get<2>(vertices_)] * b2;
  if (shading_normal.IsZero()) {
    return std::nullopt;
  }

  return shading_normal.AlignWith(additional->surface_normal);
}

std::optional<std::pair<Vector, Vector>> Triangle::MaybeComputeNormalTangents()
    const {
  if (shared_->uv.empty()) {
    return std::nullopt;
  }

  geometric_t duv02_u = shared_->uv[std::get<0>(vertices_)].first -
                        shared_->uv[std::get<2>(vertices_)].first;
  geometric_t duv02_v = shared_->uv[std::get<0>(vertices_)].second -
                        shared_->uv[std::get<2>(vertices_)].second;
  geometric_t duv12_u = shared_->uv[std::get<1>(vertices_)].first -
                        shared_->uv[std::get<2>(vertices_)].first;
  geometric_t duv12_v = shared_->uv[std::get<1>(vertices_)].second -
                        shared_->uv[std::get<2>(vertices_)].second;

  Vector dp02 = shared_->points[std::get<0>(vertices_)] -
                shared_->points[std::get<2>(vertices_)];
  Vector dp12 = shared_->points[std::get<1>(vertices_)] -
                shared_->points[std::get<2>(vertices_)];

  geometric_t determinant = duv02_u * duv12_v - duv02_v * duv12_u;
  if (std::abs(determinant) < 1e-8) {
    return std::nullopt;
  }

  Vector dp_du = (duv12_v * dp02 - duv02_v * dp12) / determinant;
  Vector dp_dv = (-duv12_u * dp02 + duv02_u * dp12) / determinant;

  return std::make_pair(dp_du, dp_dv);
}

Geometry::ComputeShadingNormalResult Triangle::ComputeShadingNormal(
    face_t face, const void* additional_data) const {
  return {MaybeComputeShadingNormal(additional_data),
          MaybeComputeNormalTangents(), shared_->normal_maps[face].Get()};
}

Geometry::ComputeHitPointResult Triangle::ComputeHitPoint(
    const Ray& ray, geometric_t distance, const void* additional_data) const {
  const AdditionalData* additional =
      static_cast<const AdditionalData*>(additional_data);

  geometric x = additional->barycentric_coordinates[0] *
                    shared_->points[std::get<0>(vertices_)].x +
                additional->barycentric_coordinates[1] *
                    shared_->points[std::get<1>(vertices_)].x +
                additional->barycentric_coordinates[2] *
                    shared_->points[std::get<2>(vertices_)].x;
  geometric y = additional->barycentric_coordinates[0] *
                    shared_->points[std::get<0>(vertices_)].y +
                additional->barycentric_coordinates[1] *
                    shared_->points[std::get<1>(vertices_)].y +
                additional->barycentric_coordinates[2] *
                    shared_->points[std::get<2>(vertices_)].y;
  geometric z = additional->barycentric_coordinates[0] *
                    shared_->points[std::get<0>(vertices_)].z +
                additional->barycentric_coordinates[1] *
                    shared_->points[std::get<1>(vertices_)].z +
                additional->barycentric_coordinates[2] *
                    shared_->points[std::get<2>(vertices_)].z;

  geometric_t x_error = std::abs(additional->barycentric_coordinates[0] *
                                 shared_->points[std::get<0>(vertices_)].x) +
                        std::abs(additional->barycentric_coordinates[1] *
                                 shared_->points[std::get<1>(vertices_)].x) +
                        std::abs(additional->barycentric_coordinates[2] *
                                 shared_->points[std::get<2>(vertices_)].x);
  geometric_t y_error = std::abs(additional->barycentric_coordinates[0] *
                                 shared_->points[std::get<0>(vertices_)].y) +
                        std::abs(additional->barycentric_coordinates[1] *
                                 shared_->points[std::get<1>(vertices_)].y) +
                        std::abs(additional->barycentric_coordinates[2] *
                                 shared_->points[std::get<2>(vertices_)].y);
  geometric_t z_error = std::abs(additional->barycentric_coordinates[0] *
                                 shared_->points[std::get<0>(vertices_)].z) +
                        std::abs(additional->barycentric_coordinates[1] *
                                 shared_->points[std::get<1>(vertices_)].z) +
                        std::abs(additional->barycentric_coordinates[2] *
                                 shared_->points[std::get<2>(vertices_)].z);

  return {Point(x, y, z),
          PositionError(x_error, y_error, z_error) * RoundingError(7)};
}

const Material* Triangle::GetMaterial(face_t face) const {
  return shared_->materials[face].Get();
}

const EmissiveMaterial* Triangle::GetEmissiveMaterial(face_t face) const {
  return shared_->emissive_materials[face].Get();
}

visual_t Triangle::ComputeSurfaceArea(face_t face,
                                      const Matrix* model_to_world) const {
  Point p0 =
      MaybeMultiply(model_to_world, shared_->points[std::get<0>(vertices_)]);
  Point p1 =
      MaybeMultiply(model_to_world, shared_->points[std::get<1>(vertices_)]);
  Point p2 =
      MaybeMultiply(model_to_world, shared_->points[std::get<2>(vertices_)]);
  return static_cast<visual_t>(0.5) * CrossProduct(p1 - p0, p2 - p0).Length();
}

std::variant<std::monostate, Point, Vector> Triangle::SampleBySolidAngle(
    const Point& origin, face_t face, Sampler& sampler) const {
  geometric_t u = sampler.Next();
  geometric_t v = sampler.Next();

  if (u + v > static_cast<geometric_t>(1.0)) {
    u = static_cast<geometric_t>(1.0) - u;
    v = static_cast<geometric_t>(1.0) - v;
  }

  Vector v0_to_v1 = shared_->points[std::get<1>(vertices_)] -
                    shared_->points[std::get<0>(vertices_)];
  Vector v0_to_v2 = shared_->points[std::get<2>(vertices_)] -
                    shared_->points[std::get<0>(vertices_)];

  return shared_->points[std::get<0>(vertices_)] + v0_to_v1 * u + v0_to_v2 * v;
}

std::optional<visual_t> Triangle::ComputePdfBySolidAngle(
    const Point& origin, face_t face, const void* additional_data,
    const Point& on_face) const {
  geometric_t distance_to_sample_squared;
  Vector to_sample = Normalize(on_face - origin, &distance_to_sample_squared);

  const AdditionalData* additional =
      static_cast<const AdditionalData*>(additional_data);

  geometric_t cos_theta =
      ClampedAbsDotProduct(to_sample, Normalize(additional->surface_normal));
  geometric_t surface_area =
      additional->surface_normal.Length() * static_cast<geometric_t>(0.5);

  return distance_to_sample_squared / (cos_theta * surface_area);
}

BoundingBox Triangle::ComputeBounds(const Matrix* model_to_world) const {
  if (model_to_world == nullptr) {
    return BoundingBox(shared_->points[std::get<0>(vertices_)],
                       shared_->points[std::get<1>(vertices_)],
                       shared_->points[std::get<2>(vertices_)]);
  }

  return BoundingBox(
      model_to_world->Multiply(shared_->points[std::get<0>(vertices_)]),
      model_to_world->Multiply(shared_->points[std::get<1>(vertices_)]),
      model_to_world->Multiply(shared_->points[std::get<2>(vertices_)]));
}

std::span<const face_t> Triangle::GetFaces() const {
  static const face_t faces[2] = {kFrontFace, kBackFace};
  return faces;
}

Hit* Triangle::Trace(const Ray& ray, geometric_t minimum_distance,
                     geometric_t maximum_distance, TraceMode trace_mode,
                     HitAllocator& hit_allocator) const {
  std::array<iris::geometric_t, 3> v0 =
      Subtract(shared_->points[std::get<0>(vertices_)], ray.origin);
  std::array<iris::geometric_t, 3> v1 =
      Subtract(shared_->points[std::get<1>(vertices_)], ray.origin);
  std::array<iris::geometric_t, 3> v2 =
      Subtract(shared_->points[std::get<2>(vertices_)], ray.origin);

  Vector::Axis dominant_axis = ray.direction.DominantAxis();

  geometric_t shear_x, shear_y, direction_z;
  switch (dominant_axis) {
    case Vector::X_AXIS:
      v0 = PermuteXDominant(v0);
      v1 = PermuteXDominant(v1);
      v2 = PermuteXDominant(v2);

      shear_x = -ray.direction.y / ray.direction.x;
      shear_y = -ray.direction.z / ray.direction.x;
      direction_z = ray.direction.x;
      break;
    case Vector::Y_AXIS:
      v0 = PermuteYDominant(v0);
      v1 = PermuteYDominant(v1);
      v2 = PermuteYDominant(v2);

      shear_x = -ray.direction.z / ray.direction.y;
      shear_y = -ray.direction.x / ray.direction.y;
      direction_z = ray.direction.y;
      break;
    case Vector::Z_AXIS:
      shear_x = -ray.direction.x / ray.direction.z;
      shear_y = -ray.direction.y / ray.direction.z;
      direction_z = ray.direction.z;
      break;
  }

  v0[0] += shear_x * v0[2];
  v0[1] += shear_y * v0[2];
  v1[0] += shear_x * v1[2];
  v1[1] += shear_y * v1[2];
  v2[0] += shear_x * v2[2];
  v2[1] += shear_y * v2[2];

  geometric_t e0 = v1[0] * v2[1] - v1[1] * v2[0];
  geometric_t e1 = v2[0] * v0[1] - v2[1] * v0[0];
  geometric_t e2 = v0[0] * v1[1] - v0[1] * v1[0];

  if constexpr (std::is_same<geometric_t, float>::value) {
    if (e0 == 0.0 || e1 == 0.0 || e2 == 0.0) {
      e0 = ((double)v1[0] * (double)v2[1] - (double)v1[1] * (double)v2[0]);
      e1 = ((double)v2[0] * (double)v0[1] - (double)v2[1] * (double)v0[0]);
      e2 = ((double)v0[0] * (double)v1[1] - (double)v0[1] * (double)v1[0]);
    }
  }

  if constexpr (std::is_same<geometric_t, double>::value) {
    if (e0 == 0.0 || e1 == 0.0 || e2 == 0.0) {
      e0 = ((long double)v1[0] * (long double)v2[1] -
            (long double)v1[1] * (long double)v2[0]);
      e1 = ((long double)v2[0] * (long double)v0[1] -
            (long double)v2[1] * (long double)v0[0]);
      e2 = ((long double)v0[0] * (long double)v1[1] -
            (long double)v0[1] * (long double)v1[0]);
    }
  }

  if ((e0 < 0.0 || e1 < 0.0 || e2 < 0.0) &&
      (e0 > 0.0 || e1 > 0.0 || e2 > 0.0)) {
    return nullptr;
  }

  geometric_t determinant = e0 + e1 + e2;
  if (determinant == 0.0) {
    return nullptr;
  }

  geometric_t b0 = e0 / determinant;
  geometric_t b1 = e1 / determinant;
  geometric_t b2 = e2 / determinant;

  geometric_t barycentric0 = std::min(static_cast<geometric_t>(1.0), b0);
  geometric_t amount_remaining = static_cast<geometric_t>(1.0) - barycentric0;
  geometric_t barycentric1 = std::min(amount_remaining, b1);
  amount_remaining -= barycentric1;
  geometric_t barycentric2 =
      std::max(static_cast<geometric_t>(0.0), amount_remaining);

  v0[2] /= direction_z;
  v1[2] /= direction_z;
  v2[2] /= direction_z;

  geometric_t distance = b0 * v0[2] + b1 * v1[2] + b2 * v2[2];

  if (shared_->alpha_mask) {
    Point endpoint = ray.Endpoint(distance);
    if (std::optional<Geometry::TextureCoordinates> texture_coordinates =
            ComputeTextureCoordinates(endpoint, std::nullopt, barycentric0,
                                      barycentric1, barycentric2);
        texture_coordinates) {
      iris::TextureCoordinates coords = {
          endpoint,
          Vector(static_cast<geometric>(0.0), static_cast<geometric>(0.0),
                 static_cast<geometric>(0.0)),
          Vector(static_cast<geometric>(0.0), static_cast<geometric>(0.0),
                 static_cast<geometric>(0.0)),
          {texture_coordinates->uv[0], texture_coordinates->uv[1]},
          texture_coordinates->du_dx,
          texture_coordinates->du_dy,
          texture_coordinates->dv_dx,
          texture_coordinates->dv_dy,
      };

      if (!shared_->alpha_mask->Included(coords)) {
        return nullptr;
      }
    }
  }

  geometric_t max_x = AbsMax(v0[0], v1[0], v2[0]);
  geometric_t max_y = AbsMax(v0[1], v1[1], v2[1]);
  geometric_t max_z = AbsMax(v0[2], v1[2], v2[2]);
  geometric_t max_e = AbsMax(e0, e1, e2);

  geometric_t delta_x = RoundingError(5) * (max_x + max_z);
  geometric_t delta_y = RoundingError(5) * (max_y + max_z);
  geometric_t delta_z = RoundingError(3) * max_z;

  geometric_t delta_e =
      static_cast<geometric_t>(2.0) *
      (RoundingError(2) * max_x * max_y + delta_y * max_x + delta_x * max_y);

  geometric_t delta_error =
      static_cast<geometric_t>(3.0) *
      (RoundingError(3) * max_e * max_z + delta_e * max_z + delta_z * max_e) /
      std::abs(determinant);

  auto [surface_normal, front_face, back_face] =
      ComputeSurfaceNormalAndFaces(ray);

  return &hit_allocator.Allocate(
      nullptr, distance, delta_error, front_face, back_face, /*is_chiral=*/true,
      AdditionalData{{barycentric0, barycentric1, barycentric2},
                     surface_normal});
}

}  // namespace

std::vector<ReferenceCounted<Geometry>> AllocateTriangleMesh(
    std::span<const Point> points,
    std::span<const std::tuple<uint32_t, uint32_t, uint32_t>> indices,
    std::span<const face_t> face_indices, std::span<const Vector> normals,
    std::span<const std::pair<geometric, geometric>> uv,
    ReferenceCounted<textures::MaskTexture> alpha_mask,
    ReferenceCounted<Material> front_material,
    ReferenceCounted<Material> back_material,
    ReferenceCounted<EmissiveMaterial> front_emissive_material,
    ReferenceCounted<EmissiveMaterial> back_emissive_material,
    ReferenceCounted<NormalMap> front_normal_map,
    ReferenceCounted<NormalMap> back_normal_map) {
  std::shared_ptr<Triangle::SharedData> shared_data =
      std::make_shared<Triangle::SharedData>(Triangle::SharedData{
          std::vector<Point>(points.begin(), points.end()),
          std::vector<Vector>(normals.begin(), normals.end()),
          std::vector<std::pair<geometric, geometric>>(uv.begin(), uv.end()),
          std::move(alpha_mask),
          {std::move(front_material), std::move(back_material)},
          {std::move(front_emissive_material),
           std::move(back_emissive_material)},
          {std::move(front_normal_map), std::move(back_normal_map)}});

  std::vector<ReferenceCounted<Geometry>> result;
  for (size_t i = 0; i < indices.size(); i++) {
    const auto& [v0, v1, v2] = indices[i];

    assert(shared_data->normals.empty() || v0 < shared_data->normals.size());
    assert(shared_data->normals.empty() || v1 < shared_data->normals.size());
    assert(shared_data->normals.empty() || v2 < shared_data->normals.size());
    assert(shared_data->uv.empty() || v0 < shared_data->uv.size());
    assert(shared_data->uv.empty() || v1 < shared_data->uv.size());
    assert(shared_data->uv.empty() || v2 < shared_data->uv.size());

    if (shared_data->points.at(v0) == shared_data->points.at(v1) ||
        shared_data->points.at(v1) == shared_data->points.at(v2) ||
        shared_data->points.at(v2) == shared_data->points.at(v0)) {
      continue;
    }

    face_t face_index = 0;
    if (i < face_indices.size()) {
      face_index = face_indices[i];
    }

    result.push_back(MakeReferenceCounted<Triangle>(std::make_tuple(v0, v1, v2),
                                                    shared_data, face_index));
  }

  return result;
}

}  // namespace geometry
}  // namespace iris
