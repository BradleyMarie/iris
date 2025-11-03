#include "iris/geometry/bvh_aggregate.h"

#include <optional>
#include <span>
#include <utility>
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
#include "iris/reference_countable.h"
#include "iris/reference_counted.h"
#include "iris/sampler.h"
#include "iris/scenes/internal/aligned_vector.h"
#include "iris/scenes/internal/bvh_builder.h"
#include "iris/scenes/internal/bvh_traversal.h"
#include "iris/texture_coordinates.h"
#include "iris/vector.h"

namespace iris {
namespace geometry {
namespace {

using ::iris::scenes::internal::AlignedVector;
using ::iris::scenes::internal::BuildBVH;
using ::iris::scenes::internal::BuildBVHResult;
using ::iris::scenes::internal::BVHNode;

class BVHAggregate final : public Geometry {
 public:
  BVHAggregate(AlignedVector<BVHNode> bvh,
               std::vector<ReferenceCounted<Geometry>> geometry)
      : bvh_(std::move(bvh)), geometry_(std::move(geometry)) {}

  Vector ComputeSurfaceNormal(const Point& hit_point, face_t face,
                              const void* additional_data) const override {
    std::unreachable();
  }

  ComputeHitPointResult ComputeHitPoint(
      const Ray& ray, geometric_t distance,
      const void* additional_data) const override {
    std::unreachable();
  }

  std::optional<visual_t> ComputeSurfaceArea(
      face_t face, const Matrix* model_to_world) const override {
    std::unreachable();
  }

  BoundingBox ComputeBounds(const Matrix* model_to_world) const override {
    if (model_to_world == nullptr) {
      return bvh_.front().Bounds();
    }

    return model_to_world->Multiply(bvh_.front().Bounds());
  }

  std::span<const face_t> GetFaces() const override { return {}; }

 private:
  Hit* Trace(const Ray& ray, geometric_t minimum_distance,
             geometric_t maximum_distance, TraceMode trace_mode,
             HitAllocator& hit_allocator) const override {
    return scenes::internal::Intersect(bvh_.front(), geometry_, ray,
                                       minimum_distance, maximum_distance,
                                       trace_mode, hit_allocator);
  }

  AlignedVector<BVHNode> bvh_;
  std::vector<ReferenceCounted<Geometry>> geometry_;
};

}  // namespace

ReferenceCounted<Geometry> AllocateBVHAggregate(
    std::vector<ReferenceCounted<Geometry>> geometry) {
  if (geometry.empty()) {
    return ReferenceCounted<Geometry>();
  }

  std::vector<ReferenceCounted<Geometry>> sorted_geometry;
  sorted_geometry.resize(geometry.size());

  BuildBVHResult result = BuildBVH(
      [&](size_t index) {
        return std::pair<const Geometry&, const Matrix*>(*geometry[index],
                                                         nullptr);
      },
      geometry.size(), /*for_scene=*/false);

  for (size_t index = 0; index < result.geometry_sort_order.size(); index++) {
    sorted_geometry[result.geometry_sort_order[index]] =
        std::move(geometry[index]);
  }

  return MakeReferenceCounted<BVHAggregate>(std::move(result.bvh),
                                            std::move(sorted_geometry));
}

}  // namespace geometry
}  // namespace iris
