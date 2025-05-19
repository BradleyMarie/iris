#include "frontends/pbrt/defaults.h"

#include <string_view>

#include "absl/log/check.h"
#include "frontends/pbrt/defaults_embed.h"
#include "pbrt_proto/v3/defaults.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace {

pbrt_proto::v3::Defaults BuildDefaults() {
  pbrt_proto::v3::Defaults result;
  CHECK(result.ParseFromString(std::string_view(kDefaults, kDefaultsSize)));
  return result;
}

const static pbrt_proto::v3::Defaults kDefaults = BuildDefaults();

}  // namespace

const pbrt_proto::v3::Defaults& Defaults() { return kDefaults; }

}  // namespace pbrt_frontend
}  // namespace iris
