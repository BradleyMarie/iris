#ifndef _FRONTENDS_PBRT_INTEGRATORS_PATH_
#define _FRONTENDS_PBRT_INTEGRATORS_PATH_

#include <memory>

#include "frontends/pbrt/integrators/result.h"
#include "pbrt_proto/v3/v3.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace integrators {

std::unique_ptr<IntegratorResult> MakePath(
    const pbrt_proto::v3::Integrator::Path& path);

}  // namespace integrators
}  // namespace pbrt_frontend
}  // namespace iris

#endif  // _FRONTENDS_PBRT_INTEGRATORS_PATH_
