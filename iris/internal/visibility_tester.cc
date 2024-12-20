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

bool VisibilityTester::Intersects(const Ray& ray, const Geometry& geometry,
                                  const Matrix* model_to_world, face_t face) {
  SingleGeometryScene geometry_scene(geometry, model_to_world);
  for (Hit* geometry_hit = ray_tracer_.Trace(
           ray, minimum_distance_, std::numeric_limits<geometric>::infinity(),
           geometry_scene);
       geometry_hit;
       geometry_hit = static_cast<internal::Hit*>(geometry_hit->next)) {
    if (geometry_hit->front == face) {
      return true;
    }
  }

  return false;
}

std::optional<VisibilityTester::VisibleResult> VisibilityTester::Visible(
    const Ray& ray, const Geometry& geometry, const Matrix* model_to_world,
    face_t face) {
  // Optimization: Check that the face is visible along the ray before tracing
  // the entire scene
  if (!Intersects(ray, geometry, model_to_world, face)) {
    return std::nullopt;
  }

  // The distance from the previous intersection cannot be used as a bound
  // since it may be be discarded in a full scene intersection if part of
  // CSG
  Hit* scene_hit =
      ray_tracer_.Trace(ray, minimum_distance_,
                        std::numeric_limits<geometric>::infinity(), scene_);
  if (!scene_hit || scene_hit->geometry != &geometry ||
      scene_hit->model_to_world != model_to_world || scene_hit->front != face) {
    return std::nullopt;
  }

  // Compute Spectrum
  Point world_hit_point = ray.Endpoint(scene_hit->distance);
  Point model_hit_point = model_to_world
                              ? model_to_world->InverseMultiply(world_hit_point)
                              : world_hit_point;

  TextureCoordinates texture_coordinates =
      geometry
          .ComputeTextureCoordinates(model_hit_point, std::nullopt, face,
                                     scene_hit->additional_data)
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
      model_origin, face, scene_hit->additional_data, model_hit_point);
  if (pdf.value_or(0.0) <= 0.0) {
    return std::nullopt;
  }

  return VisibilityTester::VisibleResult{*spectrum, world_hit_point, *pdf};
}

}  // namespace internal
}  // namespace iris