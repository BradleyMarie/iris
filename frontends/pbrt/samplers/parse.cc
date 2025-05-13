#include "frontends/pbrt/samplers/parse.h"

#include <memory>

#include "frontends/pbrt/samplers/halton.h"
#include "frontends/pbrt/samplers/random.h"
#include "frontends/pbrt/samplers/sobol.h"
#include "frontends/pbrt/samplers/stratified.h"
#include "iris/image_sampler.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {

using ::pbrt_proto::v3::Sampler;

std::unique_ptr<ImageSampler> ParseSamper(const Sampler& sampler) {
  std::unique_ptr<ImageSampler> result;
  switch (sampler.sampler_type_case()) {
    case Sampler::kHalton:
      result = samplers::MakeHalton(sampler.halton());
      break;
    case Sampler::kMaxmindist:
      break;
    case Sampler::kRandom:
      result = samplers::MakeRandom(sampler.random());
      break;
    case Sampler::kSobol:
      result = samplers::MakeSobol(sampler.sobol());
      break;
    case Sampler::kStratified:
      result = samplers::MakeStratified(sampler.stratified());
      break;
    case Sampler::kZerotwosequence:
      break;
    case Sampler::SAMPLER_TYPE_NOT_SET:
      break;
  }

  return result;
}

}  // namespace pbrt_frontend
}  // namespace iris
