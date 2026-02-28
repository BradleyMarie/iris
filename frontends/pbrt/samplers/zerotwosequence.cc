#include "frontends/pbrt/samplers/zerotwosequence.h"

#include <memory>

#include "frontends/pbrt/defaults.h"
#include "iris/image_sampler.h"
#include "iris/image_samplers/sobol_image_sampler.h"
#include "pbrt_proto/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace samplers {

using ::iris::image_samplers::MakeSobolImageSampler;
using ::iris::image_samplers::SobolScrambler;
using ::pbrt_proto::ZeroTwoSequenceSampler;

std::unique_ptr<ImageSampler> MakeZeroTwoSequence(
    const ZeroTwoSequenceSampler& zerotwosequence) {
  ZeroTwoSequenceSampler with_defaults =
      Defaults().samplers().zerotwosequence();
  with_defaults.MergeFrom(zerotwosequence);

  return MakeSobolImageSampler(with_defaults.pixelsamples(),
                               SobolScrambler::FastOwen);
}

}  // namespace samplers
}  // namespace pbrt_frontend
}  // namespace iris
