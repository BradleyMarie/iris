#include "frontends/pbrt/samplers/stratified.h"

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <memory>

#include "frontends/pbrt/defaults.h"
#include "iris/image_sampler.h"
#include "iris/image_samplers/stratified_image_sampler.h"
#include "pbrt_proto/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace samplers {

using ::iris::image_samplers::MakeStratifiedImageSampler;
using ::pbrt_proto::StratifiedSampler;

std::unique_ptr<ImageSampler> MakeStratified(
    const StratifiedSampler& stratified) {
  StratifiedSampler with_defaults = Defaults().samplers().stratified();
  with_defaults.MergeFrom(stratified);

  if (with_defaults.xsamples() > std::numeric_limits<uint16_t>::max()) {
    std::cerr << "ERROR: Out of range value for parameter: xsamples"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  if (with_defaults.ysamples() > std::numeric_limits<uint16_t>::max()) {
    std::cerr << "ERROR: Out of range value for parameter: ysamples"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  return MakeStratifiedImageSampler(
      static_cast<uint16_t>(with_defaults.xsamples()),
      static_cast<uint16_t>(with_defaults.ysamples()), with_defaults.jitter());
}

}  // namespace samplers
}  // namespace pbrt_frontend
}  // namespace iris
