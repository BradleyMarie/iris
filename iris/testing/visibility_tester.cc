#include "iris/testing/visibility_tester.h"

#include <cmath>
#include <functional>
#include <span>

#include "iris/bounding_box.h"
#include "iris/float.h"
#include "iris/geometry.h"
#include "iris/hit_allocator.h"
#include "iris/integer.h"
#include "iris/internal/arena.h"
#include "iris/internal/ray_tracer.h"
#include "iris/internal/visibility_tester.h"
#include "iris/intersector.h"
#include "iris/matrix.h"
#include "iris/point.h"
#include "iris/position_error.h"
#include "iris/ray.h"
#include "iris/scene.h"
#include "iris/vector.h"
#include "iris/visibility_tester.h"

namespace iris {
namespace testing {
namespace {

class NeverVisibleScene final : public Scene {
 private:
  class AlwaysHitsGeometry : public Geometry {
   public:
    Hit* Trace(const Ray& ray, geometric_t minimum_distance,
               geometric_t maximum_distance, TraceMode trace_mode,
               HitAllocator& hit_allocator) const override {
      return &hit_allocator.Allocate(nullptr,
                                     std::nextafter(static_cast<visual_t>(0.0),
                                                    static_cast<visual_t>(1.0)),
                                     static_cast<geometric_t>(0.0), 1, 2);
    }

    ComputeHitPointResult ComputeHitPoint(
        const Ray& ray, geometric_t distance,
        const void* additional_data) const override {
      return {ray.Endpoint(distance), PositionError(0.0, 0.0, 0.0)};
    }

    Vector ComputeSurfaceNormal(const Point& hit_point, face_t face,
                                const void* additional_data) const override {
      return Vector(1.0, 0.0, 0.0);
    }

    BoundingBox ComputeBounds(const Matrix* model_to_world) const override {
      return BoundingBox(Point(0.0, 0.0, 0.0));
    }

    visual_t ComputeSurfaceArea(face_t face,
                                const Matrix* model_to_world) const override {
      return 0.0;
    }

    std::span<const face_t> GetFaces() const override {
      static const face_t faces[] = {1};
      return faces;
    }
  };

 public:
  void Trace(const Ray& ray, Intersector& intersector) const override {
    intersector.Intersect(geometry_);
  }

  AlwaysHitsGeometry geometry_;
};

class AlwaysVisibleScene final : public Scene {
 public:
  void Trace(const Ray& ray, Intersector& intersector) const override {}
};

class SingleGeometryScene final : public Scene {
 public:
  SingleGeometryScene(const Geometry& geometry,
                      const Matrix* model_to_world) noexcept
      : geometry_(geometry), model_to_world_(model_to_world) {}

  void Trace(const Ray& ray, Intersector& intersector) const override {
    intersector.Intersect(geometry_, model_to_world_);
  }

 private:
  const Geometry& geometry_;
  const Matrix* model_to_world_;
};

}  // namespace

VisibilityTester& GetAlwaysVisibleVisibilityTester() {
  static AlwaysVisibleScene visible_scene;
  thread_local internal::RayTracer ray_tracer;
  thread_local internal::Arena arena;
  thread_local internal::VisibilityTester visibility_tester(visible_scene, 0.0,
                                                            ray_tracer, arena);
  return visibility_tester;
}

VisibilityTester& GetNeverVisibleVisibilityTester() {
  static NeverVisibleScene never_visible_scene;
  thread_local internal::RayTracer ray_tracer;
  thread_local internal::Arena arena;
  thread_local internal::VisibilityTester visibility_tester(
      never_visible_scene, 0.0, ray_tracer, arena);
  return visibility_tester;
}

void ScopedSingleGeometryVisibilityTester(
    const Geometry& geometry, const Matrix* matrix,
    std::function<void(VisibilityTester&)> callback) {
  internal::RayTracer ray_tracer;
  internal::Arena arena;
  SingleGeometryScene scene(geometry, matrix);
  internal::VisibilityTester visibility_tester(scene, 0.0, ray_tracer, arena);
  callback(visibility_tester);
}

}  // namespace testing
}  // namespace iris
