#ifndef _FRONTENDS_PBRT_INTEGRATORS_PARSE_
#define _FRONTENDS_PBRT_INTEGRATORS_PARSE_

#include <memory>

#include "frontends/pbrt/integrators/result.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {

std::unique_ptr<IntegratorResult> ParseIntegrator(
    const pbrt_proto::v3::Integrator& integrator);

}  // namespace pbrt_frontend
}  // namespace iris

#endif  // _FRONTENDS_PBRT_INTEGRATORS_PARSE_