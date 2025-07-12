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
      std::cerr << "ERROR: Unsupported Integrator type: ambientocclusion"
                << std::endl;
      exit(EXIT_FAILURE);
      break;
    case Integrator::kBdpt:
      std::cerr << "ERROR: Unsupported Integrator type: bdpt" << std::endl;
      exit(EXIT_FAILURE);
      break;
    case Integrator::kDirectlighting:
      std::cerr << "ERROR: Unsupported Integrator type: directlighting"
                << std::endl;
      exit(EXIT_FAILURE);
      break;
    case Integrator::kMlt:
      std::cerr << "ERROR: Unsupported Integrator type: mlt" << std::endl;
      exit(EXIT_FAILURE);
      break;
    case Integrator::kPath:
      result = integrators::MakePath(integrator.path());
      break;
    case Integrator::kSppm:
      std::cerr << "ERROR: Unsupported Integrator type: sppm" << std::endl;
      exit(EXIT_FAILURE);
      break;
    case Integrator::kVolpath:
      std::cerr << "ERROR: Unsupported Integrator type: volpath" << std::endl;
      exit(EXIT_FAILURE);
      break;
    case Integrator::kWhitted:
      std::cerr << "ERROR: Unsupported Integrator type: whitted" << std::endl;
      exit(EXIT_FAILURE);
      break;
    case Integrator::INTEGRATOR_TYPE_NOT_SET:
      break;
  }

  return result;
}

}  // namespace pbrt_frontend
}  // namespace iris
