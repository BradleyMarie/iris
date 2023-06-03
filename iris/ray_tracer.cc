#include "iris/ray_tracer.h"

#include <limits>

#include "iris/bxdf_allocator.h"
#include "iris/internal/ray_tracer.h"
#include "iris/spectral_allocator.h"

namespace iris {
namespace {

Vector MaybeTransformNormal(const Matrix* model_to_world,
                            const Vector& surface_normal) {
  if (!model_to_world) {
    return surface_normal;
  }

  return Normalize(model_to_world->InverseTransposeMultiply(surface_normal));
}

std::optional<RayTracer::Differentials> MakeDifferentials(
    const std::optional<Geometry::Differentials>& differentials) {
  if (!differentials) {
    return std::nullopt;
  }

  return {{differentials->dx, differentials->dy}};
}

std::optional<NormalMap::Differentials> MakeNormalMapDifferentials(
    const Point& hit_point,
    const std::optional<Geometry::Differentials>& differentials,
    const NormalMap* normal_map) {
  if (!differentials || !normal_map) {
    return std::nullopt;
  }

  Vector dp_dx = differentials->dx - hit_point;
  Vector dp_dy = differentials->dy - hit_point;

  return {{dp_dx, dp_dy}};
}

std::pair<Vector, std::optional<NormalMap::Differentials>> TransformNormals(
    const Vector& world_surface_normal,
    const std::optional<NormalMap::Differentials>& world_differentials,
    const Matrix* model_to_world,
    const std::optional<Vector>& maybe_model_shading_normal) {
  if (!maybe_model_shading_normal) {
    return {world_surface_normal, world_differentials};
  }

  auto world_shading_normal =
      MaybeTransformNormal(model_to_world, *maybe_model_shading_normal);
  if (!world_differentials) {
    return {world_shading_normal, world_differentials};
  }

  geometric_t cos_theta =
      DotProduct(world_surface_normal, world_shading_normal);
  if (cos_theta >= static_cast<geometric_t>(1.0)) {
    return {world_surface_normal, world_differentials};
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

  Vector dp_dx(DotProduct(rx, world_differentials->dp_dx),
               DotProduct(ry, world_differentials->dp_dx),
               DotProduct(rz, world_differentials->dp_dx));
  Vector dp_dy(DotProduct(rx, world_differentials->dp_dy),
               DotProduct(ry, world_differentials->dp_dy),
               DotProduct(rz, world_differentials->dp_dy));

  return {world_shading_normal, {{dp_dx, dp_dy}}};
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

  auto shading_normal =
      hit.geometry->ComputeShadingNormal(hit.front, hit.additional_data);

  auto world_normal_map_differentials = MakeNormalMapDifferentials(
      world_hit_point, world_differentials, shading_normal.normal_map);

  auto world_shading_normals =
      TransformNormals(world_surface_normal, world_normal_map_differentials,
                       hit.model_to_world, shading_normal.geometry);

  Vector world_shading_normal =
      shading_normal.normal_map
          ? shading_normal.normal_map->Evaluate(texture_coordinates,
                                                world_shading_normals.second,
                                                world_shading_normals.first)
          : world_shading_normals.first;
  assert(DotProduct(world_surface_normal, world_shading_normal) > 0.0);

  return RayTracer::SurfaceIntersection{
      Bsdf(*bxdf, world_surface_normal, world_shading_normal), world_hit_point,
      MakeDifferentials(world_differentials), world_surface_normal,
      world_shading_normal};
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

  auto world_differentials =
      ComputeGeometryDifferentials(ray, world_hit_point, world_surface_normal);
  auto model_differentials =
      MaybeTransformDifferentials(world_differentials, hit->model_to_world);
  assert(DotProduct(ray.direction, world_surface_normal) < 0.0);

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