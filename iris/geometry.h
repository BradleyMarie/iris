#ifndef _IRIS_GEOMETRY_
#define _IRIS_GEOMETRY_

#include "iris/hit_allocator.h"
#include "iris/ray.h"

namespace iris {

class Geometry {
 public:
  Hit* Trace(HitAllocator& hit_allocator) const;

 private:
  virtual Hit* Trace(const Ray& ray, HitAllocator& hit_allocator) const = 0;
};

}  // namespace iris

#endif  // _IRIS_GEOMETRY_