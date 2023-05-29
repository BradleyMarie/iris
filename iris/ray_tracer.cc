#include "iris/ray_tracer.h"

#include <limits>

#include "iris/bxdf_allocator.h"
#include "iris/internal/ray_tracer.h"
#include "iris/spectral_allocator.h"

namespace iris {
namespace {

std::pair<Vector, Vector> Transform(const Matrix& model_to_world,
                                    const std::pair<Vector, Vector>& vectors) {
  return std::make_pair(
      Normalize(model_to_world.InverseTransposeMultiply(vectors.first)),
      Normalize(model_to_world.InverseTransposeMultiply(vectors.second)));
}

std::pair<Vector, Vector> MaybeTransform(const Matrix* model_to_world,
                                         const Vector& surface_normal,
                                         const Vector& shading_normal) {
  auto vectors = std::make_pair(surface_normal, shading_normal);
  if (!model_to_world) {
    return vectors;
  }

  return Transform(*model_to_world, vectors);
}

std::pair<Ray, Ray> Transform(const Matrix& model_to_world,
                              const std::pair<Ray, Ray>& rays) {
  return std::make_pair(Normalize(model_to_world.InverseMultiply(rays.first)),
                        Normalize(model_to_world.InverseMultiply(rays.second)));
}

std::pair<Ray, Ray> MaybeTransform(const Matrix* model_to_world, const Ray& dx,
                                   const Ray& dy) {
  auto rays = std::make_pair(dx, dy);
  if (!model_to_world) {
    return rays;
  }

  return Transform(*model_to_world, rays);
}

std::optional<RayTracer::Differentials> MakeDifferentials(
    const Matrix* model_to_world,
    const std::optional<Geometry::Differentials>& differentials) {
  if (!differentials) {
    return std::nullopt;
  }

  if (!model_to_world) {
    return {{differentials->dx, differentials->dy}};
  }

  return {{model_to_world->Multiply(differentials->dx),
           model_to_world->Multiply(differentials->dy)}};
}

std::optional<NormalMap::Differentials> MakeNormalMapDifferentials(
    const Point& model_hit_point,
    const std::optional<Geometry::Differentials>& differentials,
    const NormalMap* normal_map) {
  if (!differentials || !normal_map) {
    return std::nullopt;
  }

  return {{differentials->dx - model_hit_point,
           differentials->dy - model_hit_point}};
}

std::pair<Vector, geometric_t> SelectNormal(const Vector& geometry_normal,
                                            const Vector& shading_normal) {
  geometric_t cos_theta =
      std::max(static_cast<geometric_t>(-1.0),
               std::min(DotProduct(geometry_normal, shading_normal),
                        static_cast<geometric_t>(1.0)));

  if (cos_theta < static_cast<geometric_t>(0.0)) {
    return {-shading_normal, -cos_theta};
  }

  return {shading_normal, cos_theta};
}

std::pair<Vector, std::optional<NormalMap::Differentials>> TransformNormals(
    const Vector& geometry_normal,
    const std::optional<NormalMap::Differentials>& differentials,
    const std::optional<Vector>& maybe_shading_normal) {
  if (!maybe_shading_normal) {
    return {geometry_normal, differentials};
  }

  auto [shading_normal, cos_theta] =
      SelectNormal(geometry_normal, *maybe_shading_normal);

  if (!differentials) {
    return {shading_normal, differentials};
  }

  geometric_t sin_theta =
      std::sqrt(static_cast<geometric_t>(1.0) - cos_theta * cos_theta);

  Vector axis = Normalize(CrossProduct(geometry_normal, shading_normal));

  geometric_t one_minus_cos_theta = static_cast<geometric_t>(1.0) - cos_theta;
  Vector rx(cos_theta + axis.x * axis.x * one_minus_cos_theta,
            axis.x * axis.y * one_minus_cos_theta - axis.z * sin_theta,
            axis.x * axis.z * one_minus_cos_theta + axis.y * sin_theta);
  Vector ry(axis.y * axis.x * one_minus_cos_theta + axis.z * sin_theta,
            cos_theta + axis.y * axis.y * one_minus_cos_theta,
            axis.y * axis.z * one_minus_cos_theta - axis.x * sin_theta);
  Vector rz(axis.z * axis.x * one_minus_cos_theta - axis.y * sin_theta,
            axis.z * axis.y * one_minus_cos_theta + axis.x * sin_theta,
            cos_theta + axis.z * axis.z * one_minus_cos_theta);

  return {shading_normal,
          {{Vector(DotProduct(rx, differentials->dp_dx),
                   DotProduct(ry, differentials->dp_dx),
                   DotProduct(rz, differentials->dp_dx)),
            Vector(DotProduct(rx, differentials->dp_dy),
                   DotProduct(ry, differentials->dp_dy),
                   DotProduct(rz, differentials->dp_dy))}}};
}

std::optional<RayTracer::SurfaceIntersection> MakeSurfaceIntersection(
    const iris::internal::Hit& hit,
    const std::optional<Geometry::Differentials> differentials,
    const TextureCoordinates& texture_coordinates, const Point& world_hit_point,
    const Point& model_hit_point, const Vector& model_surface_normal,
    SpectralAllocator& spectral_allocator, BxdfAllocator& bxdf_allocator) {
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

  auto model_shading_normals = TransformNormals(
      model_surface_normal,
      MakeNormalMapDifferentials(model_hit_point, differentials,
                                 shading_normal.normal_map),
      shading_normal.geometry);

  Vector model_shading_normal =
      shading_normal.normal_map
          ? shading_normal.normal_map->Evaluate(texture_coordinates,
                                                model_shading_normals.second,
                                                model_shading_normals.first)
          : model_shading_normals.first;

  auto vectors = MaybeTransform(hit.model_to_world, model_surface_normal,
                                model_shading_normal);

  return RayTracer::SurfaceIntersection{
      Bsdf(*bxdf, vectors.first, vectors.second), world_hit_point,
      MakeDifferentials(hit.model_to_world, differentials), vectors.first,
      vectors.second};
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

Point PlaneIntersection(const Ray& ray, const Point& point,
                        const Vector& normal) {
  Vector to_plane = point - ray.origin;
  geometric_t distance =
      DotProduct(to_plane, normal) / DotProduct(ray.direction, normal);
  return ray.Endpoint(distance);
}

std::optional<Geometry::Differentials> ComputeDifferentials(
    const Matrix* model_to_world, const RayDifferential& world_ray,
    const Point& model_hit_point, const Vector& model_surface_normal) {
  if (!world_ray.differentials) {
    return std::nullopt;
  }

  auto model_rays = MaybeTransform(model_to_world, world_ray.differentials->dx,
                                   world_ray.differentials->dy);

  return {{PlaneIntersection(model_rays.first, model_hit_point,
                             model_surface_normal),
           PlaneIntersection(model_rays.second, model_hit_point,
                             model_surface_normal)}};
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

  auto differentials = ComputeDifferentials(
      hit->model_to_world, ray, model_hit_point, model_surface_normal);

  TextureCoordinates texture_coordinates =
      hit->geometry
          ->ComputeTextureCoordinates(model_hit_point, differentials,
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
      *hit, differentials, texture_coordinates, world_hit_point,
      model_hit_point, model_surface_normal, spectral_allocator,
      bxdf_allocator);

  return RayTracer::TraceResult{spectrum, surface_intersection};
}

}  // namespace iris