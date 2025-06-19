#include "iris/intersector.h"

#include "iris/float.h"
#include "iris/geometry.h"
#include "iris/hit.h"
#include "iris/hit_allocator.h"
#include "iris/internal/hit.h"
#include "iris/matrix.h"
#include "iris/ray.h"

namespace iris {

bool Intersector::Intersect(const Geometry& geometry) {
  HitAllocator allocator(ray_, hit_arena_);

  if (Hit* hit = geometry.TraceOneHit(allocator, minimum_distance_,
                                      maximum_distance_, find_closest_hit_);
      hit && hit->distance < closest_hit_distance_) {
    internal::Hit* full_hit = static_cast<internal::Hit*>(hit);
    full_hit->model_ray.emplace(ray_);

    closest_hit_distance_ = full_hit->distance;
    maximum_distance_ = full_hit->distance + full_hit->distance_error;
    hit_ = full_hit;
    done_ = !find_closest_hit_;
  }

  return done_;
}

bool Intersector::Intersect(const Geometry& geometry,
                            const Matrix* model_to_world) {
  Ray trace_ray =
      model_to_world ? model_to_world->InverseMultiplyWithError(ray_) : ray_;
  HitAllocator allocator(trace_ray, hit_arena_);

  if (Hit* hit = geometry.TraceOneHit(allocator, minimum_distance_,
                                      maximum_distance_, find_closest_hit_);
      hit && hit->distance < closest_hit_distance_) {
    internal::Hit* full_hit = static_cast<internal::Hit*>(hit);
    full_hit->model_ray.emplace(trace_ray);
    full_hit->model_to_world = model_to_world;

    closest_hit_distance_ = full_hit->distance;
    maximum_distance_ = full_hit->distance + full_hit->distance_error;
    hit_ = full_hit;
    done_ = !find_closest_hit_;
  }

  return done_;
}

bool Intersector::Intersect(const Geometry& geometry,
                            const Matrix& model_to_world) {
  Ray trace_ray = model_to_world.InverseMultiplyWithError(ray_);
  HitAllocator allocator(trace_ray, hit_arena_);

  if (Hit* hit = geometry.TraceOneHit(allocator, minimum_distance_,
                                      maximum_distance_, find_closest_hit_);
      hit && hit->distance < closest_hit_distance_) {
    internal::Hit* full_hit = static_cast<internal::Hit*>(hit);
    full_hit->model_ray.emplace(trace_ray);
    full_hit->model_to_world = &model_to_world;

    closest_hit_distance_ = full_hit->distance;
    maximum_distance_ = full_hit->distance + full_hit->distance_error;
    hit_ = full_hit;
    done_ = !find_closest_hit_;
  }

  return done_;
}

}  // namespace iris
