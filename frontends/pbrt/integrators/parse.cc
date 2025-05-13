#include "frontends/pbrt/integrators/parse.h"

#include "frontends/pbrt/integrators/path.h"
#include "frontends/pbrt/integrators/result.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {

using pbrt_proto::v3::Integrator;

std::unique_ptr<IntegratorResult> ParseIntegrator(
    const Integrator& integrator) {
  std::unique_ptr<IntegratorResult> result;
  switch (integrator.integrator_type_case()) {
    case Integrator::kAmbientocclusion:
      break;
    case Integrator::kBdpt:
      break;
    case Integrator::kDirectlighting:
      break;
    case Integrator::kMlt:
      break;
    case Integrator::kPath:
      result = integrators::MakePath(integrator.path());
      break;
    case Integrator::kSppm:
      break;
    case Integrator::kVolpath:
      break;
    case Integrator::kWhitted:
      break;
    case Integrator::INTEGRATOR_TYPE_NOT_SET:
      break;
  }

  return result;
}

}  // namespace pbrt_frontend
}  // namespace iris
