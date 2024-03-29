#include "iris/ray_tracer.h"

#include <limits>

#include "iris/bxdf_allocator.h"
#include "iris/internal/ray_tracer.h"
#include "iris/spectral_allocator.h"

namespace iris {
namespace {

Vector MaybeTransformVector(const Matrix* model_to_world,
                            const Vector& surface_normal) {
  if (!model_to_world) {
    return surface_normal;
  }

  return model_to_world->InverseTransposeMultiply(surface_normal);
}

Vector MaybeTransformNormal(const Matrix* model_to_world,
                            const Vector& surface_normal) {
  return Normalize(MaybeTransformVector(model_to_world, surface_normal));
}

std::optional<Vector> MaybeTransformNormal(
    const Matrix* model_to_world, const std::optional<Vector>& surface_normal) {
  if (!surface_normal) {
    return std::nullopt;
  }

  return MaybeTransformNormal(model_to_world, *surface_normal);
}

std::optional<RayTracer::Differentials> MakeDifferentials(
    const std::optional<Geometry::Differentials>& differentials) {
  if (!differentials) {
    return std::nullopt;
  }

  return {{differentials->dx, differentials->dy}};
}

std::pair<Vector, std::optional<NormalMap::Differentials>> TransformNormals(
    const Point& world_hit_point, const Vector& world_surface_normal,
    const Matrix* model_to_world,
    const std::optional<Geometry::Differentials> world_differentials,
    const Geometry::ComputeShadingNormalResult& shading_normals) {
  auto world_shading_normal =
      MaybeTransformNormal(model_to_world, shading_normals.surface_normal)
          .value_or(world_surface_normal);

  if (!shading_normals.normal_map) {
    return {world_shading_normal, std::nullopt};
  }

  if (shading_normals.dp_duv.has_value()) {
    if (!model_to_world) {
      return {
          world_shading_normal,
          {{NormalMap::Differentials::DU_DV,
            {shading_normals.dp_duv->first, shading_normals.dp_duv->second}}}};
    }

    return {world_shading_normal,
            {{NormalMap::Differentials::DU_DV,
              {model_to_world->Multiply(shading_normals.dp_duv->first),
               model_to_world->Multiply(shading_normals.dp_duv->second)}}}};
  }

  if (!world_differentials) {
    return {world_shading_normal, std::nullopt};
  }

  Vector dp_dx = world_differentials->dx - world_hit_point;
  Vector dp_dy = world_differentials->dy - world_hit_point;

  geometric_t cos_theta =
      ClampedDotProduct(world_surface_normal, world_shading_normal);
  if (cos_theta >= static_cast<geometric_t>(1.0)) {
    return {world_shading_normal,
            {{NormalMap::Differentials::DX_DY, {dp_dx, dp_dy}}}};
  }

  geometric_t sin_theta =
      std::sqrt(static_cast<geometric_t>(1.0) - cos_theta * cos_theta);
  geometric_t one_minus_cos_theta = static_cast<geometric_t>(1.0) - cos_theta;

  Vector axis =
      Normalize(CrossProduct(world_surface_normal, world_shading_normal));

  Vector rx(cos_theta + axis.x * axis.x * one_minus_cos_theta,
            axis.x * axis.y * one_minus_cos_theta - axis.z * sin_theta,
            axis.x * axis.z * one_minus_cos_theta + axis.y * sin_theta);
  Vector ry(axis.y * axis.x * one_minus_cos_theta + axis.z * sin_theta,
            cos_theta + axis.y * axis.y * one_minus_cos_theta,
            axis.y * axis.z * one_minus_cos_theta - axis.x * sin_theta);
  Vector rz(axis.z * axis.x * one_minus_cos_theta - axis.y * sin_theta,
            axis.z * axis.y * one_minus_cos_theta + axis.x * sin_theta,
            cos_theta + axis.z * axis.z * one_minus_cos_theta);

  Vector transformed_dp_dx(DotProduct(rx, dp_dx), DotProduct(ry, dp_dx),
                           DotProduct(rz, dp_dx));
  Vector transformed_dp_dy(DotProduct(rx, dp_dy), DotProduct(ry, dp_dy),
                           DotProduct(rz, dp_dy));

  return {world_shading_normal,
          {{NormalMap::Differentials::DX_DY,
            {transformed_dp_dx, transformed_dp_dy}}}};
}

std::optional<RayTracer::SurfaceIntersection> MakeSurfaceIntersection(
    const iris::internal::Hit& hit,
    const std::optional<Geometry::Differentials> world_differentials,
    const TextureCoordinates& texture_coordinates, const Point& world_hit_point,
    const Vector& world_surface_normal, SpectralAllocator& spectral_allocator,
    BxdfAllocator& bxdf_allocator) {
  auto* material = hit.geometry->GetMaterial(hit.front, hit.additional_data);
  if (!material) {
    return std::nullopt;
  }

  auto* bxdf = material->Evaluate(texture_coordinates, spectral_allocator,
                                  bxdf_allocator);
  if (!bxdf) {
    return std::nullopt;
  }

  auto shading_normals =
      hit.geometry->ComputeShadingNormal(hit.front, hit.additional_data);

  auto world_shading_normals = TransformNormals(
      world_hit_point, world_surface_normal, hit.model_to_world,
      world_differentials, shading_normals);

  // When the object is transformed, it is arguably more correct to apply the
  // normal map in model space instead of world space. This might be good to
  // change in the future.
  Vector world_shading_normal =
      shading_normals.normal_map
          ? Normalize(shading_normals.normal_map->Evaluate(
                texture_coordinates, world_shading_normals.second,
                world_shading_normals.first))
          : world_shading_normals.first;

  // Transformation may cause world shading normal to no longer be aligned with
  // the true surface normal, so if it is not aligned simply reverse it and hope
  // for the best visually on the assumption that this should be rare. This
  // might be a good argument for evaluating BSDFs in model space. If this is
  // done the asserts should be updated to check that the model surface and
  // model shading normals are in the same hemisphere.
  Vector aligned_world_shading_normal =
      world_shading_normal.AlignWith(world_surface_normal);

  return RayTracer::SurfaceIntersection{
      Bsdf(*bxdf, world_surface_normal, aligned_world_shading_normal),
      world_hit_point, MakeDifferentials(world_differentials),
      world_surface_normal, aligned_world_shading_normal};
}

RayTracer::TraceResult HandleMiss(const Ray& ray,
                                  const EnvironmentalLight* environmental_light,
                                  SpectralAllocator& spectral_allocator) {
  if (!environmental_light) {
    return RayTracer::TraceResult{nullptr, std::nullopt};
  }

  return RayTracer::TraceResult{
      environmental_light->Emission(ray.direction, spectral_allocator),
      std::nullopt};
}

geometric_t PlaneIntersection(const Ray& ray, const Point& point,
                              const Vector& normal) {
  Vector to_plane = point - ray.origin;
  geometric_t distance =
      DotProduct(to_plane, normal) / DotProduct(ray.direction, normal);
  return distance;
}

std::optional<Geometry::Differentials> ComputeGeometryDifferentials(
    const RayDifferential& world_ray, const Point& world_hit_point,
    const Vector& world_surface_normal) {
  if (!world_ray.differentials) {
    return std::nullopt;
  }

  geometric_t distance_dx = PlaneIntersection(
      world_ray.differentials->dx, world_hit_point, world_surface_normal);
  geometric_t distance_dy = PlaneIntersection(
      world_ray.differentials->dy, world_hit_point, world_surface_normal);

  return {{world_ray.differentials->dx.Endpoint(distance_dx),
           world_ray.differentials->dy.Endpoint(distance_dy)}};
}

std::optional<Geometry::Differentials> MaybeTransformDifferentials(
    const std::optional<Geometry::Differentials>& world_differentials,
    const Matrix* model_to_world) {
  if (!world_differentials || !model_to_world) {
    return world_differentials;
  }

  return {{model_to_world->InverseMultiply(world_differentials->dx),
           model_to_world->InverseMultiply(world_differentials->dy)}};
}

}  // namespace

RayTracer::TraceResult RayTracer::Trace(const RayDifferential& ray) {
  SpectralAllocator spectral_allocator(arena_);

  auto* hit =
      ray_tracer_.Trace(ray, minimum_distance_,
                        std::numeric_limits<geometric_t>::infinity(), scene_);
  if (!hit) {
    return HandleMiss(ray, environmental_light_, spectral_allocator);
  }

  Point world_hit_point = ray.Endpoint(hit->distance);
  Point model_hit_point =
      hit->model_to_world
          ? hit->model_to_world->InverseMultiply(world_hit_point)
          : world_hit_point;

  Vector model_surface_normal = hit->geometry->ComputeSurfaceNormal(
      model_hit_point, hit->front, hit->additional_data);
  Vector world_surface_normal =
      MaybeTransformNormal(hit->model_to_world, model_surface_normal);
  assert(DotProduct(world_surface_normal, ray.direction) < 0.001);

  auto world_differentials =
      ComputeGeometryDifferentials(ray, world_hit_point, world_surface_normal);
  auto model_differentials =
      MaybeTransformDifferentials(world_differentials, hit->model_to_world);

  TextureCoordinates texture_coordinates =
      hit->geometry
          ->ComputeTextureCoordinates(model_hit_point, model_differentials,
                                      hit->front, hit->additional_data)
          .value_or(TextureCoordinates{0.0, 0.0});

  const Spectrum* spectrum = nullptr;
  if (auto* emissive_material = hit->geometry->GetEmissiveMaterial(
          hit->front, hit->additional_data)) {
    spectrum =
        emissive_material->Evaluate(texture_coordinates, spectral_allocator);
  }

  BxdfAllocator bxdf_allocator(arena_);
  auto surface_intersection = MakeSurfaceIntersection(
      *hit, world_differentials, texture_coordinates, world_hit_point,
      world_surface_normal, spectral_allocator, bxdf_allocator);

  return RayTracer::TraceResult{spectrum, surface_intersection};
}

}  // namespace iris