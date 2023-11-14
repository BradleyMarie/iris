#ifndef _IRIS_TESTING_HIT_ALLOCATOR_
#define _IRIS_TESTING_HIT_ALLOCATOR_

#include <memory>

#include "iris/hit_allocator.h"

namespace iris {
namespace testing {

HitAllocator MakeHitAllocator(const Ray& ray);

// Hit Properties
face_t FrontFace(const Hit& hit);
face_t BackFace(const Hit& hit);
const void* AdditionalData(const Hit& hit);

}  // namespace testing
}  // namespace iris

#endif  // _IRIS_TESTING_HIT_ALLOCATOR_