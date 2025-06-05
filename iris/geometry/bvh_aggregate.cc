#include "iris/geometry/bvh_aggregate.h"

#include <algorithm>
#include <cassert>

#include "iris/scenes/internal/bvh_builder.h"
#include "iris/scenes/internal/bvh_traversal.h"

namespace iris {
namespace geometry {
namespace {

class BVHAggregate final : public Geometry {
 public:
  BVHAggregate(std::vector<scenes::internal::BVHNode> bvh,
               std::vector<ReferenceCounted<Geometry>> geometry)
      : bvh_(std::move(bvh)), geometry_(std::move(geometry)) {}

  Vector ComputeSurfaceNormal(const Point& hit_point, face_t face,
                              const void* additional_data) const override;

  ComputeHitPointResult ComputeHitPoint(
      const Ray& ray, geometric_t distance,
      const void* additional_data) const override;

  visual_t ComputeSurfaceArea(face_t face,
                              const Matrix* model_to_world) const override;

  BoundingBox ComputeBounds(const Matrix* model_to_world) const override;

  std::span<const face_t> GetFaces() const override;

 private:
  Hit* Trace(const Ray& ray, HitAllocator& hit_allocator) const override;

  std::vector<scenes::internal::BVHNode> bvh_;
  std::vector<ReferenceCounted<Geometry>> geometry_;
};

Vector BVHAggregate::ComputeSurfaceNormal(const Point& hit_point, face_t face,
                                          const void* additional_data) const {
  assert(false);
  return Vector(0.0, 0.0, 1.0);
}

Geometry::ComputeHitPointResult BVHAggregate::ComputeHitPoint(
    const Ray& ray, geometric_t distance, const void* additional_data) const {
  assert(false);
  return ComputeHitPointResult{Point(0.0, 0.0, 0.0),
                               PositionError(0.0, 0.0, 0.0)};
}

visual_t BVHAggregate::ComputeSurfaceArea(face_t face,
                                          const Matrix* model_to_world) const {
  assert(false);
  return static_cast<visual_t>(0.0);
}

BoundingBox BVHAggregate::ComputeBounds(const Matrix* model_to_world) const {
  if (model_to_world == nullptr) {
    return bvh_.front().Bounds();
  }

  return model_to_world->Multiply(bvh_.front().Bounds());
}

std::span<const face_t> BVHAggregate::GetFaces() const { return {}; }

Hit* BVHAggregate::Trace(const Ray& ray, HitAllocator& hit_allocator) const {
  return scenes::internal::Intersect(bvh_.front(), geometry_, ray,
                                     hit_allocator);
}

}  // namespace

ReferenceCounted<Geometry> AllocateBVHAggregate(
    std::vector<ReferenceCounted<Geometry>> geometry) {
  if (geometry.empty()) {
    return ReferenceCounted<Geometry>();
  }

  if (geometry.size() == 1) {
    return std::move(geometry[0]);
  }

  std::vector<ReferenceCounted<Geometry>> sorted_geometry;
  sorted_geometry.resize(geometry.size());

  auto result = scenes::internal::BuildBVH(
      [&](size_t index) {
        return std::pair<const Geometry&, const Matrix*>(*geometry[index],
                                                         nullptr);
      },
      geometry.size());

  for (size_t index = 0; index < result.geometry_sort_order.size(); index++) {
    assert(std::ranges::all_of(geometry[index]->GetFaces(), [&](face_t face) {
      return !geometry[index]->GetEmissiveMaterial(face);
    }));

    sorted_geometry[result.geometry_sort_order[index]] =
        std::move(geometry[index]);
  }

  return MakeReferenceCounted<BVHAggregate>(std::move(result.bvh),
                                            std::move(sorted_geometry));
}

}  // namespace geometry
}  // namespace iris