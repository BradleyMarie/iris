#include "iris/geometry/sphere.h"

#define _USE_MATH_CONSTANTS
#include <algorithm>
#include <cassert>
#include <cmath>

namespace iris {
namespace geometry {
namespace {

class Sphere final : public Geometry {
 public:
  static constexpr face_t kFrontFace = 0u;
  static constexpr face_t kBackFace = 1u;

  Sphere(const Point& center, geometric radius,
         ReferenceCounted<Material> front_material,
         ReferenceCounted<Material> back_material,
         ReferenceCounted<EmissiveMaterial> front_emissive_material,
         ReferenceCounted<EmissiveMaterial> back_emissive_material,
         ReferenceCounted<NormalMap> front_normal_map,
         ReferenceCounted<NormalMap> back_normal_map) noexcept
      : center_(center),
        radius_(radius),
        radius_squared_(radius * radius),
        materials_{std::move(front_material), std::move(back_material)},
        emissive_materials_{std::move(front_emissive_material),
                            std::move(back_emissive_material)},
        normal_maps_{std::move(front_normal_map), std::move(back_normal_map)} {
    assert(std::isfinite(radius) && radius > 0.0);
  }

  virtual Vector ComputeSurfaceNormal(
      const Point& hit_point, face_t face,
      const void* additional_data) const override;

  virtual std::optional<TextureCoordinates> ComputeTextureCoordinates(
      const Point& hit_point, const std::optional<Differentials>& differentials,
      face_t face, const void* additional_data) const override;

  virtual std::variant<Vector, const NormalMap*> ComputeShadingNormal(
      face_t face, const void* additional_data) const override;

  virtual const Material* GetMaterial(
      face_t face, const void* additional_data) const override;

  virtual bool IsEmissive(face_t face) const override;

  virtual const EmissiveMaterial* GetEmissiveMaterial(
      face_t face, const void* additional_data) const override;

  virtual std::variant<std::monostate, Point, Vector> SampleBySolidAngle(
      const Point& origin, face_t face, Sampler& sampler) const override;

  virtual std::optional<visual_t> ComputePdfBySolidAngle(
      const Point& origin, face_t face, const Point& on_face) const override;

  virtual BoundingBox ComputeBounds(
      const Matrix& model_to_world) const override;

  virtual std::span<const face_t> GetFaces() const override;

 private:
  virtual Hit* Trace(const Ray& ray,
                     HitAllocator& hit_allocator) const override;

  const Point center_;
  const geometric radius_;
  const geometric_t radius_squared_;
  const ReferenceCounted<Material> materials_[2];
  const ReferenceCounted<EmissiveMaterial> emissive_materials_[2];
  const ReferenceCounted<NormalMap> normal_maps_[2];
};

Vector Sphere::ComputeSurfaceNormal(const Point& hit_point, face_t face,
                                    const void* additional_data) const {
  Vector normal =
      (face == kFrontFace) ? hit_point - center_ : center_ - hit_point;
  return Normalize(normal);
}

std::optional<TextureCoordinates> Sphere::ComputeTextureCoordinates(
    const Point& hit_point, const std::optional<Differentials>& differentials,
    face_t face, const void* additional_data) const {
  return std::nullopt;
}

std::variant<Vector, const NormalMap*> Sphere::ComputeShadingNormal(
    face_t face, const void* additional_data) const {
  return normal_maps_[face].Get();
}

const Material* Sphere::GetMaterial(face_t face,
                                    const void* additional_data) const {
  return materials_[face].Get();
}

bool Sphere::IsEmissive(face_t face) const {
  return emissive_materials_[face].Get();
}

const EmissiveMaterial* Sphere::GetEmissiveMaterial(
    face_t face, const void* additional_data) const {
  return emissive_materials_[face].Get();
}

std::variant<std::monostate, Point, Vector> Sphere::SampleBySolidAngle(
    const Point& origin, face_t face, Sampler& sampler) const {
  Vector to_center = center_ - origin;
  geometric_t distance_to_center_squared = DotProduct(to_center, to_center);

  if (face == kFrontFace) {
    if (distance_to_center_squared < radius_squared_) {
      return std::variant<std::monostate, Point, Vector>();
    }

    geometric_t distance_to_center;
    Vector vz = Normalize(to_center, nullptr, &distance_to_center);
    auto [vx, vy] = CoordinateSystem(vz);

    geometric_t sin_theta =
        std::min(static_cast<geometric_t>(1.0), radius_ / distance_to_center);
    geometric_t cos_theta =
        std::sqrt(static_cast<geometric_t>(1.0) - sin_theta * sin_theta);

    geometric_t sampled_cos_theta =
        std::lerp(cos_theta, static_cast<geometric_t>(1.0), sampler.Next());
    geometric_t sampled_sin_theta = std::sqrt(
        static_cast<geometric_t>(1.0) - sampled_cos_theta * sampled_cos_theta);

    geometric_t sampled_phi = std::lerp(-M_PI, M_PI, sampler.Next());

    return sampled_sin_theta *
               (std::cos(sampled_phi) * vx + std::sin(sampled_phi) * vy) +
           sampled_cos_theta * vz;
  }

  if (distance_to_center_squared >= radius_squared_) {
    return std::variant<std::monostate, Point, Vector>();
  }

  geometric_t z = std::lerp(-radius_, radius_, sampler.Next());
  geometric_t phi = std::lerp(-M_PI, M_PI, sampler.Next());

  geometric_t r = std::sqrt(
      std::max(static_cast<geometric_t>(0.0), radius_squared_ - z * z));
  geometric_t x = r * std::cos(phi);
  geometric_t y = r * std::sin(phi);

  return center_ + iris::Vector(x, y, z);
}

std::optional<visual_t> Sphere::ComputePdfBySolidAngle(
    const Point& origin, face_t face, const Point& on_face) const {
  if (face == kFrontFace) {
    Vector to_center = center_ - origin;
    geometric_t distance_to_center_squared = DotProduct(to_center, to_center);

    geometric_t sin_theta_squared =
        radius_squared_ / distance_to_center_squared;
    geometric_t cos_theta_squared =
        std::max(static_cast<geometric_t>(0.0),
                 static_cast<geometric_t>(1.0) - sin_theta_squared);
    geometric_t cos_theta = std::sqrt(cos_theta_squared);
    return 1.0 / (2.0 * M_PI * (1.0 - cos_theta));
  }

  geometric_t distance_to_sample_squared;
  Vector to_face = Normalize(on_face - origin, &distance_to_sample_squared);
  Vector surface_normal = ComputeSurfaceNormal(on_face, kFrontFace, nullptr);
  geometric_t cos_theta = DotProduct(surface_normal, to_face);

  return distance_to_sample_squared /
         (cos_theta * 4.0 * M_PI * radius_squared_);
}

BoundingBox Sphere::ComputeBounds(const Matrix& model_to_world) const {
  Vector x_offset(radius_, 0.0, 0.0);
  Vector y_offset(0.0, radius_, 0.0);
  Vector z_offset(0.0, 0.0, radius_);

  return BoundingBox(
      model_to_world.Multiply(center_ + x_offset + y_offset + z_offset),
      model_to_world.Multiply(center_ + x_offset + y_offset - z_offset),
      model_to_world.Multiply(center_ + x_offset - y_offset + z_offset),
      model_to_world.Multiply(center_ + x_offset - y_offset - z_offset),
      model_to_world.Multiply(center_ - x_offset + y_offset + z_offset),
      model_to_world.Multiply(center_ - x_offset + y_offset - z_offset),
      model_to_world.Multiply(center_ - x_offset - y_offset + z_offset),
      model_to_world.Multiply(center_ - x_offset - y_offset - z_offset));
}

std::span<const face_t> Sphere::GetFaces() const {
  static const face_t faces[2] = {kFrontFace, kBackFace};
  return faces;
}

Hit* Sphere::Trace(const Ray& ray, HitAllocator& hit_allocator) const {
  Vector to_center = center_ - ray.origin;

  geometric_t distance_to_chord_midpoint = DotProduct(to_center, ray.direction);
  geometric_t distance_to_center_squared = DotProduct(to_center, to_center);
  geometric_t distance_to_chord_midpoint_squared =
      distance_to_chord_midpoint * distance_to_chord_midpoint;
  geometric_t distance_from_chord_to_center_squared =
      distance_to_center_squared - distance_to_chord_midpoint_squared;

  // The ray completely misses the sphere. No intersections are possible.
  if (radius_squared_ < distance_from_chord_to_center_squared) {
    return nullptr;
  }

  geometric_t half_chord_length =
      std::sqrt(radius_squared_ - distance_from_chord_to_center_squared);

  // Ray intersects sphere and originates inside the sphere
  if (distance_to_center_squared < radius_squared_) {
    geometric_t forwards_distance =
        distance_to_chord_midpoint + half_chord_length;
    Hit* forwards_hit = &hit_allocator.Allocate(nullptr, forwards_distance,
                                                kBackFace, kFrontFace);

    geometric_t backwards_distance =
        distance_to_chord_midpoint - half_chord_length;
    return &hit_allocator.Allocate(forwards_hit, backwards_distance, kBackFace,
                                   kFrontFace);
  }

  // Ignore rays that intersect only at a single point
  if (half_chord_length == 0.0) {
    return nullptr;
  }

  // Ray intersects sphere and originates outside the sphere.
  face_t first_face, second_face;
  if (distance_to_chord_midpoint >= 0.0) {
    first_face = kFrontFace;
    second_face = kBackFace;
  } else {
    first_face = kBackFace;
    second_face = kFrontFace;
  }

  geometric_t farther_distance = distance_to_chord_midpoint + half_chord_length;
  Hit* farther_hit = &hit_allocator.Allocate(nullptr, farther_distance,
                                             second_face, first_face);

  geometric_t closer_distance = distance_to_chord_midpoint - half_chord_length;
  return &hit_allocator.Allocate(farther_hit, closer_distance, first_face,
                                 second_face);
}

}  // namespace

ReferenceCounted<Geometry> AllocateSphere(
    const Point& center, geometric radius,
    ReferenceCounted<Material> front_material,
    ReferenceCounted<Material> back_material,
    ReferenceCounted<EmissiveMaterial> front_emissive_material,
    ReferenceCounted<EmissiveMaterial> back_emissive_material,
    ReferenceCounted<NormalMap> front_normal_map,
    ReferenceCounted<NormalMap> back_normal_map) {
  return MakeReferenceCounted<Sphere>(
      center, radius, front_material, back_material, front_emissive_material,
      back_emissive_material, front_normal_map, back_normal_map);
}

}  // namespace geometry
}  // namespace iris