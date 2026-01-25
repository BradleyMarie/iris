#ifndef _FRONTENDS_PBRT_SAMPLERS_STRATIFIED_
#define _FRONTENDS_PBRT_SAMPLERS_STRATIFIED_

#include <memory>

#include "iris/image_sampler.h"
#include "pbrt_proto/v3/v3.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace samplers {

std::unique_ptr<ImageSampler> MakeStratified(
    const pbrt_proto::v3::Sampler::Stratified& stratified);

}  // namespace samplers
}  // namespace pbrt_frontend
}  // namespace iris

#endif  // _FRONTENDS_PBRT_SAMPLERS_STRATIFIED_
