#ifndef _IRIS_TESTING_INTERSECTOR_
#define _IRIS_TESTING_INTERSECTOR_

#include <memory>

#include "iris/intersector.h"

namespace iris {
namespace testing {

Intersector MakeIntersector(const Ray& ray, geometric_t minimum_distance,
                            geometric_t maximum_distance, Hit*& closest_hit);

}  // namespace testing
}  // namespace iris

#endif  // _IRIS_TESTING_INTERSECTOR_