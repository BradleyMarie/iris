#include "iris/light.h"

#include "iris/internal/light_parameters.h"

namespace iris {

Light::Light(const internal::LightParameters& parameters) noexcept
    : invisible_(parameters.invisible) {}

Light::Light() noexcept : invisible_(true) {}

}  // namespace iris
