#include "frontends/pbrt/samplers/stratified.h"

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <memory>

#include "iris/image_sampler.h"
#include "iris/image_samplers/stratified_image_sampler.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace samplers {

using ::iris::image_samplers::MakeStratifiedImageSampler;
using ::pbrt_proto::v3::Sampler;

std::unique_ptr<ImageSampler> MakeStratified(
    const Sampler::Stratified& stratified) {
  if (stratified.xsamples() > std::numeric_limits<uint16_t>::max()) {
    std::cerr << "ERROR: Out of range value for parameter: xsamples"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  if (stratified.ysamples() > std::numeric_limits<uint16_t>::max()) {
    std::cerr << "ERROR: Out of range value for parameter: ysamples"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  return MakeStratifiedImageSampler(
      static_cast<uint16_t>(stratified.xsamples()),
      static_cast<uint16_t>(stratified.ysamples()), stratified.jitter());
}

}  // namespace samplers
}  // namespace pbrt_frontend
}  // namespace iris
