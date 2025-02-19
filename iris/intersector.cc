#include "iris/intersector.h"

#include <cassert>

#include "iris/hit_allocator.h"
#include "iris/internal/hit.h"

namespace iris {

bool Intersector::Intersect(const Geometry& geometry) {
  HitAllocator allocator(ray_, hit_arena_);

  for (auto* hit_list = geometry.Trace(allocator); hit_list;
       hit_list = hit_list->next) {
    assert(!hit_list->next || hit_list->distance < hit_list->next->distance);

    auto* full_hit = static_cast<internal::Hit*>(hit_list);
    if (full_hit->distance - full_hit->distance_error > minimum_distance_ &&
        full_hit->distance < closest_hit_distance_ &&
        full_hit->distance + full_hit->distance_error < maximum_distance_) {
      full_hit->model_ray.emplace(ray_);
      hit_ = hit_list;
      closest_hit_distance_ = hit_list->distance;

      if (find_any_hit_) {
        done_ = true;
        break;
      }
    }
  }

  return done_;
}

bool Intersector::Intersect(const Geometry& geometry,
                            const Matrix* model_to_world) {
  Ray trace_ray =
      model_to_world ? model_to_world->InverseMultiplyWithError(ray_) : ray_;
  HitAllocator allocator(trace_ray, hit_arena_);

  for (auto* hit_list = geometry.Trace(allocator); hit_list;
       hit_list = hit_list->next) {
    assert(!hit_list->next || hit_list->distance < hit_list->next->distance);

    auto* full_hit = static_cast<internal::Hit*>(hit_list);
    if (full_hit->distance - full_hit->distance_error > minimum_distance_ &&
        full_hit->distance < closest_hit_distance_ &&
        full_hit->distance + full_hit->distance_error < maximum_distance_) {
      full_hit->model_ray.emplace(trace_ray);
      full_hit->model_to_world = model_to_world;
      hit_ = hit_list;
      closest_hit_distance_ = hit_list->distance;

      if (find_any_hit_) {
        done_ = true;
        break;
      }
    }
  }

  return done_;
}

bool Intersector::Intersect(const Geometry& geometry,
                            const Matrix& model_to_world) {
  Ray trace_ray = model_to_world.InverseMultiplyWithError(ray_);
  HitAllocator allocator(trace_ray, hit_arena_);

  for (auto* hit_list = geometry.Trace(allocator); hit_list;
       hit_list = hit_list->next) {
    assert(!hit_list->next || hit_list->distance < hit_list->next->distance);

    auto* full_hit = static_cast<internal::Hit*>(hit_list);
    if (full_hit->distance - full_hit->distance_error > minimum_distance_ &&
        full_hit->distance < closest_hit_distance_ &&
        full_hit->distance + full_hit->distance_error < maximum_distance_) {
      full_hit->model_ray.emplace(trace_ray);
      full_hit->model_to_world = &model_to_world;
      hit_ = hit_list;
      closest_hit_distance_ = hit_list->distance;

      if (find_any_hit_) {
        done_ = true;
        break;
      }
    }
  }

  return done_;
}

}  // namespace iris