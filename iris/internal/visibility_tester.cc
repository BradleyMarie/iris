#include "iris/internal/visibility_tester.h"

#include <cassert>
#include <limits>

#include "iris/internal/ray_tracer.h"
#include "iris/scene.h"
#include "iris/spectral_allocator.h"

namespace iris {
namespace internal {
namespace {

class SingleGeometryScene final : public Scene {
 public:
  SingleGeometryScene(const Geometry& geometry,
                      const Matrix* model_to_world) noexcept
      : geometry_(geometry), model_to_world_(model_to_world) {}

  void Trace(const Ray& ray, Intersector& intersector) const override;

 private:
  const Geometry& geometry_;
  const Matrix* model_to_world_;
};

void SingleGeometryScene::Trace(const Ray& ray,
                                Intersector& intersector) const {
  intersector.Intersect(geometry_, model_to_world_);
}

}  // namespace

std::optional<VisibilityTester::VisibleResult> VisibilityTester::Visible(
    const Ray& ray, const Geometry& geometry, const Matrix* model_to_world,
    face_t face) {
  // Optimization: Find the closest point on the geometry in isolation
  SingleGeometryScene geometry_scene(geometry, model_to_world);
  Hit* geometry_hit = ray_tracer_.TraceClosestHit(
      ray, minimum_distance_, std::numeric_limits<geometric>::infinity(),
      geometry_scene);
  if (!geometry_hit || geometry_hit->front != face) {
    return std::nullopt;
  }

  // Check if there are any intersections between the ray origin and the hit
  if (Hit* scene_hit = ray_tracer_.TraceAnyHit(ray, minimum_distance_,
                                               geometry_hit->distance, scene_);
      scene_hit && (scene_hit->geometry != &geometry ||
                    scene_hit->model_to_world != model_to_world ||
                    scene_hit->front != face)) {
    return std::nullopt;
  }

  // Compute Spectrum
  Point world_hit_point = ray.Endpoint(geometry_hit->distance);
  Point model_hit_point = model_to_world
                              ? model_to_world->InverseMultiply(world_hit_point)
                              : world_hit_point;

  TextureCoordinates texture_coordinates =
      geometry
          .ComputeTextureCoordinates(model_hit_point, std::nullopt, face,
                                     geometry_hit->additional_data)
          .value_or(TextureCoordinates{0.0, 0.0});

  const EmissiveMaterial* emissive_material =
      geometry.GetEmissiveMaterial(face);
  if (!emissive_material) {
    return std::nullopt;
  }

  SpectralAllocator spectral_allocator(arena_);
  const Spectrum* spectrum =
      emissive_material->Evaluate(texture_coordinates, spectral_allocator);
  if (!spectrum) {
    return std::nullopt;
  }

  Point model_origin =
      model_to_world ? model_to_world->InverseMultiply(ray.origin) : ray.origin;
  std::optional<visual_t> pdf = geometry.ComputePdfBySolidAngle(
      model_origin, face, geometry_hit->additional_data, model_hit_point);
  if (pdf.value_or(0.0) <= 0.0) {
    return std::nullopt;
  }

  return VisibilityTester::VisibleResult{*spectrum, world_hit_point, *pdf};
}

}  // namespace internal
}  // namespace iris