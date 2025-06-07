#ifndef _IRIS_INTEGRATORS_MOCK_INTEGRATOR_
#define _IRIS_INTEGRATORS_MOCK_INTEGRATOR_

#include <cstdint>
#include <memory>

#include "iris/float.h"
#include "iris/integrator.h"

namespace iris {
namespace integrators {

std::unique_ptr<Integrator> MakePathIntegrator(
    visual maximum_path_continue_probability,
    visual always_continue_path_throughput, uint8_t min_bounces,
    uint8_t max_bounces);

}  // namespace integrators
}  // namespace iris

#endif  // _IRIS_INTEGRATORS_MOCK_INTEGRATOR_
