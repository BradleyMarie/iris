#include "iris/geometry/triangle_mesh.h"

namespace iris {
namespace geometry {
namespace {

std::array<geometric_t, 3> Subtract(const Point& point0, const Point& point1) {
  return std::array<geometric_t, 3>{point0.x - point1.x, point0.y - point1.y,
                                    point0.z - point1.z};
}

std::array<geometric_t, 3> PermuteXDominant(
    const std::array<geometric_t, 3>& point) {
  return std::array<geometric_t, 3>{point[2], point[1], point[0]};
}

std::array<geometric_t, 3> PermuteYDominant(
    const std::array<geometric_t, 3>& point) {
  return std::array<geometric_t, 3>{point[0], point[2], point[1]};
}

class Triangle final : public Geometry {
 public:
  struct SharedData {
    std::vector<Point> points;
    std::vector<Vector> normals;
    std::vector<std::pair<geometric, geometric>> uv;
    ReferenceCounted<Material> materials[2];
    ReferenceCounted<EmissiveMaterial> emissive_materials[2];
    ReferenceCounted<NormalMap> normal_maps[2];
  };

  typedef std::array<geometric_t, 3> AdditionalData;

  static constexpr face_t kFrontFace = 0u;
  static constexpr face_t kBackFace = 1u;

  Triangle(const std::tuple<uint32_t, uint32_t, uint32_t>& vertices,
           std::shared_ptr<const SharedData> shared) noexcept
      : vertices_(vertices), shared_(std::move(shared)) {}

  virtual Vector ComputeSurfaceNormal(
      const Point& hit_point, face_t face,
      const void* additional_data) const override;

  virtual std::optional<TextureCoordinates> ComputeTextureCoordinates(
      const Point& hit_point, const std::optional<Differentials>& differentials,
      face_t face, const void* additional_data) const override;

  virtual Geometry::ComputeShadingNormalResult ComputeShadingNormal(
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

  visual_t ComputeArea(face_t face) const;

  Vector ComputeSurfaceNormal() const;

  std::optional<Vector> MaybeComputeShadingNormal(
      face_t face, const void* additional_data) const;

  std::array<geometric_t, 2> UVCoordinates(const Point& point) const;

  const std::tuple<uint32_t, uint32_t, uint32_t> vertices_;
  const std::shared_ptr<const SharedData> shared_;
};

Vector Triangle::ComputeSurfaceNormal() const {
  Vector v0_to_v1 = shared_->points[std::get<1>(vertices_)] -
                    shared_->points[std::get<0>(vertices_)];
  Vector v0_to_v2 = shared_->points[std::get<2>(vertices_)] -
                    shared_->points[std::get<0>(vertices_)];
  return CrossProduct(v0_to_v1, v0_to_v2);
}

Vector Triangle::ComputeSurfaceNormal(const Point& hit_point, face_t face,
                                      const void* additional_data) const {
  Vector surface_normal = ComputeSurfaceNormal();
  Vector normalized = Normalize(surface_normal);

  if (face == kFrontFace) {
    return normalized;
  }

  return -normalized;
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

std::optional<TextureCoordinates> Triangle::ComputeTextureCoordinates(
    const Point& hit_point, const std::optional<Differentials>& differentials,
    face_t face, const void* additional_data) const {
  if (shared_->uv.empty()) {
    return std::nullopt;
  }

  const auto* barycentric = static_cast<const AdditionalData*>(additional_data);

  geometric_t u =
      shared_->uv[std::get<0>(vertices_)].first * (*barycentric)[0] +
      shared_->uv[std::get<1>(vertices_)].first * (*barycentric)[1] +
      shared_->uv[std::get<2>(vertices_)].first * (*barycentric)[2];
  geometric_t v =
      shared_->uv[std::get<0>(vertices_)].second * (*barycentric)[0] +
      shared_->uv[std::get<1>(vertices_)].second * (*barycentric)[1] +
      shared_->uv[std::get<2>(vertices_)].second * (*barycentric)[2];

  if (!differentials) {
    return TextureCoordinates{{u, v}};
  }

  auto uv_dx = UVCoordinates(differentials->dx);
  auto uv_dy = UVCoordinates(differentials->dy);

  return TextureCoordinates{
      {u, v}, {{uv_dx[0] - u, uv_dy[0] - u, uv_dx[1] - v, uv_dy[1] - v}}};
}

std::optional<Vector> Triangle::MaybeComputeShadingNormal(
    face_t face, const void* additional_data) const {
  if (shared_->normals.empty()) {
    return std::nullopt;
  }

  const auto* barycentric = static_cast<const AdditionalData*>(additional_data);
  Vector shading_normal =
      shared_->normals[std::get<0>(vertices_)] * (*barycentric)[0] +
      shared_->normals[std::get<1>(vertices_)] * (*barycentric)[1] +
      shared_->normals[std::get<2>(vertices_)] * (*barycentric)[2];
  Vector normalized = Normalize(shading_normal);

  if (face == kFrontFace) {
    return normalized;
  }

  return -normalized;
}

Geometry::ComputeShadingNormalResult Triangle::ComputeShadingNormal(
    face_t face, const void* additional_data) const {
  return {MaybeComputeShadingNormal(face, additional_data),
          shared_->normal_maps[face].Get()};
}

const Material* Triangle::GetMaterial(face_t face,
                                      const void* additional_data) const {
  return shared_->materials[face].Get();
}

bool Triangle::IsEmissive(face_t face) const {
  return shared_->emissive_materials[face].Get();
}

const EmissiveMaterial* Triangle::GetEmissiveMaterial(
    face_t face, const void* additional_data) const {
  return shared_->emissive_materials[face].Get();
}

std::variant<std::monostate, Point, Vector> Triangle::SampleBySolidAngle(
    const Point& origin, face_t face, Sampler& sampler) const {
  geometric_t u = sampler.Next();
  geometric_t v = sampler.Next();

  if (u + v > (geometric_t)1.0) {
    u = (geometric_t)1.0 - u;
    v = (geometric_t)1.0 - v;
  }

  Vector v0_to_v1 = shared_->points[std::get<1>(vertices_)] -
                    shared_->points[std::get<0>(vertices_)];
  Vector v0_to_v2 = shared_->points[std::get<2>(vertices_)] -
                    shared_->points[std::get<0>(vertices_)];

  return shared_->points[std::get<0>(vertices_)] + v0_to_v1 * u + v0_to_v2 * v;
}

std::optional<visual_t> Triangle::ComputePdfBySolidAngle(
    const Point& origin, face_t face, const Point& on_face) const {
  geometric_t distance_to_sample_squared;
  Vector to_sample = Normalize(on_face - origin, &distance_to_sample_squared);

  geometric_t cos_theta =
      DotProduct(to_sample, Normalize(ComputeSurfaceNormal()));
  if (face == kFrontFace) {
    cos_theta = -cos_theta;
  }

  return distance_to_sample_squared / (cos_theta * ComputeArea(face));
}

visual_t Triangle::ComputeArea(face_t face) const {
  Vector surface_normal = ComputeSurfaceNormal();
  return surface_normal.Length() * 0.5;
}

BoundingBox Triangle::ComputeBounds(const Matrix& model_to_world) const {
  return BoundingBox(
      model_to_world.Multiply(shared_->points[std::get<0>(vertices_)]),
      model_to_world.Multiply(shared_->points[std::get<1>(vertices_)]),
      model_to_world.Multiply(shared_->points[std::get<2>(vertices_)]));
}

std::span<const face_t> Triangle::GetFaces() const {
  static const face_t faces[2] = {kFrontFace, kBackFace};
  return faces;
}

Hit* Triangle::Trace(const Ray& ray, HitAllocator& hit_allocator) const {
  auto v0 = Subtract(shared_->points[std::get<0>(vertices_)], ray.origin);
  auto v1 = Subtract(shared_->points[std::get<1>(vertices_)], ray.origin);
  auto v2 = Subtract(shared_->points[std::get<2>(vertices_)], ray.origin);

  auto dominant_axis = ray.direction.DominantAxis();

  geometric_t shear_x, shear_y, direction_z;
  switch (dominant_axis) {
    case Vector::X_AXIS:
      v0 = PermuteXDominant(v0);
      v1 = PermuteXDominant(v1);
      v2 = PermuteXDominant(v2);

      shear_x = -ray.direction.z / ray.direction.x;
      shear_y = -ray.direction.y / ray.direction.x;
      direction_z = ray.direction.x;
      break;
    case Vector::Y_AXIS:
      v0 = PermuteYDominant(v0);
      v1 = PermuteYDominant(v1);
      v2 = PermuteYDominant(v2);

      shear_x = -ray.direction.x / ray.direction.y;
      shear_y = -ray.direction.z / ray.direction.y;
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

  geometric_t b0 = v1[0] * v2[1] - v1[1] * v2[0];
  geometric_t b1 = v2[0] * v0[1] - v2[1] * v0[0];
  geometric_t b2 = v0[0] * v1[1] - v0[1] * v1[0];

  if constexpr (std::is_same<geometric_t, float>::value) {
    if (b0 == 0.0 || b1 == 0.0 || b2 == 0.0) {
      b0 = ((double)v1[0] * (double)v2[1] - (double)v1[1] * (double)v2[0]);
      b1 = ((double)v2[0] * (double)v0[1] - (double)v2[1] * (double)v0[0]);
      b2 = ((double)v0[0] * (double)v1[1] - (double)v0[1] * (double)v1[0]);
    }
  }

  if constexpr (std::is_same<geometric_t, double>::value) {
    if (b0 == 0.0 || b1 == 0.0 || b2 == 0.0) {
      b0 = ((long double)v1[0] * (long double)v2[1] -
            (long double)v1[1] * (long double)v2[0]);
      b1 = ((long double)v2[0] * (long double)v0[1] -
            (long double)v2[1] * (long double)v0[0]);
      b2 = ((long double)v0[0] * (long double)v1[1] -
            (long double)v0[1] * (long double)v1[0]);
    }
  }

  if ((b0 < 0.0 || b1 < 0.0 || b2 < 0.0) &&
      (b0 > 0.0 || b1 > 0.0 || b2 > 0.0)) {
    return nullptr;
  }

  geometric_t determinant = b0 + b1 + b2;
  if (determinant == 0.0) {
    return nullptr;
  }

  geometric_t shear_z = (geometric_t)1.0 / direction_z;
  v0[2] = v0[2] * shear_z;
  v1[2] = v1[2] * shear_z;
  v2[2] = v2[2] * shear_z;

  geometric_t distance = b0 * v0[2] + b1 * v1[2] + b2 * v2[2];

  geometric_t inverse_determinant = (geometric_t)1.0 / determinant;
  distance *= inverse_determinant;

  AdditionalData additional_data{b0 * inverse_determinant,
                                 b1 * inverse_determinant,
                                 b2 * inverse_determinant};

  geometric_t dp = DotProduct(ray.direction, ComputeSurfaceNormal());

  face_t front_face, back_face;
  if (dp < 0.0) {
    front_face = kFrontFace;
    back_face = kBackFace;
  } else {
    front_face = kBackFace;
    back_face = kFrontFace;
  }

  return &hit_allocator.Allocate(nullptr, distance, front_face, back_face,
                                 additional_data);
}

}  // namespace

std::vector<ReferenceCounted<Geometry>> AllocateTriangleMesh(
    std::span<const Point> points,
    std::span<const std::tuple<uint32_t, uint32_t, uint32_t>> indices,
    std::span<const Vector> normals,
    std::span<const std::pair<geometric, geometric>> uv,
    ReferenceCounted<Material> front_material,
    ReferenceCounted<Material> back_material,
    ReferenceCounted<EmissiveMaterial> front_emissive_material,
    ReferenceCounted<EmissiveMaterial> back_emissive_material,
    ReferenceCounted<NormalMap> front_normal_map,
    ReferenceCounted<NormalMap> back_normal_map) {
  auto shared_data =
      std::make_shared<Triangle::SharedData>(Triangle::SharedData{
          std::vector<Point>(points.begin(), points.end()),
          std::vector<Vector>(normals.begin(), normals.end()),
          std::vector<std::pair<geometric, geometric>>(uv.begin(), uv.end()),
          {std::move(front_material), std::move(back_material)},
          {std::move(front_emissive_material),
           std::move(back_emissive_material)},
          {std::move(front_normal_map), std::move(back_normal_map)}});

  std::vector<ReferenceCounted<Geometry>> result;
  for (const auto& entry : indices) {
    assert(shared_data->normals.empty() ||
           std::get<0>(entry) < shared_data->normals.size());
    assert(shared_data->normals.empty() ||
           std::get<1>(entry) < shared_data->normals.size());
    assert(shared_data->normals.empty() ||
           std::get<2>(entry) < shared_data->normals.size());
    assert(shared_data->uv.empty() ||
           std::get<0>(entry) < shared_data->uv.size());
    assert(shared_data->uv.empty() ||
           std::get<1>(entry) < shared_data->uv.size());
    assert(shared_data->uv.empty() ||
           std::get<2>(entry) < shared_data->uv.size());

    if (shared_data->points.at(std::get<0>(entry)) ==
            shared_data->points.at(std::get<1>(entry)) ||
        shared_data->points.at(std::get<1>(entry)) ==
            shared_data->points.at(std::get<2>(entry)) ||
        shared_data->points.at(std::get<2>(entry)) ==
            shared_data->points.at(std::get<0>(entry))) {
      continue;
    }

    result.push_back(MakeReferenceCounted<Triangle>(entry, shared_data));
  }

  return result;
}

}  // namespace geometry
}  // namespace iris