#ifndef _FRONTENDS_PBRT_FILE_
#define _FRONTENDS_PBRT_FILE_

#include <filesystem>
#include <string>
#include <utility>

#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {

std::pair<pbrt_proto::v3::PbrtProto, std::filesystem::path> LoadFile(
    const std::filesystem::path& search_root, const std::string& path);

}  // namespace pbrt_frontend
}  // namespace iris

#endif  // _FRONTENDS_PBRT_FILE_
