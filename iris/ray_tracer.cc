#include "iris/ray_tracer.h"

#include <cmath>
#include <limits>

#include "iris/bxdf_allocator.h"
#include "iris/internal/ray_tracer.h"
#include "iris/spectral_allocator.h"

namespace iris {
namespace {

Geometry::ComputeHitPointResult MaybeTransformHitPoint(
    const Geometry::ComputeHitPointResult& model_hit_point,
    const Matrix* model_to_world) {
  if (!model_to_world) {
    return model_hit_point;
  }

  auto [world_point, world_error] = model_to_world->MultiplyWithError(
      model_hit_point.point, &model_hit_point.error);
  return {world_point, world_error};
}

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
    const TextureCoordinates& texture_coordinates,
    const Geometry::ComputeHitPointResult& world_hit_point,
    const Vector& world_surface_normal, const Vector& model_surface_normal,
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

  auto shading_normals =
      hit.geometry->ComputeShadingNormal(hit.front, hit.additional_data);

  auto world_shading_normals = TransformNormals(
      world_hit_point.point, world_surface_normal, hit.model_to_world,
      world_differentials, shading_normals);

  Vector world_shading_normal =
      shading_normals.normal_map
          ? Normalize(shading_normals.normal_map->Evaluate(
                texture_coordinates, world_shading_normals.second,
                world_shading_normals.first))
          : world_shading_normals.first;

  // This ideally wouldn't be required, but it is possible that the shading
  // normal we compute ends up out of alignment with the surface normal. Since
  // the visual errors this can cause can be obvious, we instead force the
  // shading normal back into alignment with the surface normal.
  Vector aligned_world_shading_normal =
      world_shading_normal.AlignWith(world_surface_normal);

  return RayTracer::SurfaceIntersection{
      Bsdf(*bxdf, world_surface_normal, aligned_world_shading_normal),
      HitPoint(world_hit_point.point, world_hit_point.error,
               world_surface_normal),
      MakeDifferentials(world_differentials), world_surface_normal,
      aligned_world_shading_normal};
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
    const RayDifferential& world_ray, const Matrix* model_to_world,
    const Point& model_hit_point, const Vector& model_surface_normal) {
  if (!world_ray.differentials) {
    return std::nullopt;
  }

  Ray model_dx = model_to_world ? model_to_world->InverseMultiplyWithError(
                                      world_ray.differentials->dx)
                                : world_ray.differentials->dx;
  Ray model_dy = model_to_world ? model_to_world->InverseMultiplyWithError(
                                      world_ray.differentials->dy)
                                : world_ray.differentials->dy;

  geometric_t distance_dx =
      PlaneIntersection(model_dx, model_hit_point, model_surface_normal);
  geometric_t distance_dy =
      PlaneIntersection(model_dy, model_hit_point, model_surface_normal);

  return {{model_dx.Endpoint(distance_dx), model_dy.Endpoint(distance_dy)}};
}

std::optional<Geometry::Differentials> MaybeTransformDifferentials(
    const std::optional<Geometry::Differentials>& model_differentials,
    const Matrix* model_to_world) {
  if (!model_differentials || !model_to_world) {
    return model_differentials;
  }

  return {{model_to_world->Multiply(model_differentials->dx),
           model_to_world->Multiply(model_differentials->dy)}};
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

  auto model_hit_point = hit->geometry->ComputeHitPoint(
      *hit->model_ray, hit->distance, hit->additional_data);
  auto world_hit_point =
      MaybeTransformHitPoint(model_hit_point, hit->model_to_world);

  Vector model_surface_normal = hit->geometry->ComputeSurfaceNormal(
      model_hit_point.point, hit->front, hit->additional_data);
  Vector world_surface_normal =
      MaybeTransformNormal(hit->model_to_world, model_surface_normal);
  assert(DotProduct(world_surface_normal, ray.direction) < 0.001);

  auto model_differentials = ComputeGeometryDifferentials(
      ray, hit->model_to_world, model_hit_point.point, model_surface_normal);
  auto world_differentials =
      MaybeTransformDifferentials(model_differentials, hit->model_to_world);

  TextureCoordinates texture_coordinates =
      hit->geometry
          ->ComputeTextureCoordinates(model_hit_point.point,
                                      model_differentials, hit->front,
                                      hit->additional_data)
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
      world_surface_normal, model_surface_normal, spectral_allocator,
      bxdf_allocator);

  return RayTracer::TraceResult{spectrum, surface_intersection};
}

}  // namespace iris