#ifndef _IRIS_RAY_DIFFERENTIAL_
#define _IRIS_RAY_DIFFERENTIAL_

#include <optional>

#include "iris/ray.h"

namespace iris {

struct RayDifferential final : public Ray {
  explicit RayDifferential(const Ray& base) noexcept : Ray(base) {}

  RayDifferential(const Ray& base, const Ray& dx, const Ray& dy) noexcept
      : Ray(base), differentials(Differentials{dx, dy}) {}

  RayDifferential(const RayDifferential&) noexcept = default;

  bool operator==(const RayDifferential&) const = default;

  struct Differentials {
    bool operator==(const Differentials&) const = default;

    const Ray dx;
    const Ray dy;
  };

  const std::optional<Differentials> differentials;
};

static inline RayDifferential Normalize(const RayDifferential& ray) {
  Ray base = Normalize(static_cast<const Ray&>(ray));
  if (!ray.differentials.has_value()) {
    return RayDifferential(base);
  }

  return RayDifferential(base, Normalize(ray.differentials->dx),
                         Normalize(ray.differentials->dy));
}

}  // namespace iris

#endif  // _IRIS_RAY_DIFFERENTIAL_
