#include "frontends/pbrt/file.h"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>

#include "absl/status/status.h"
#include "google/protobuf/io/zero_copy_stream_impl.h"
#include "google/protobuf/text_format.h"
#include "pbrt_proto/v3/convert.h"
#include "pbrt_proto/v3/v3.pb.h"

namespace iris {
namespace pbrt_frontend {

using ::google::protobuf::TextFormat;
using ::google::protobuf::io::IstreamInputStream;
using ::pbrt_proto::v3::Convert;
using ::pbrt_proto::v3::PbrtProto;

std::filesystem::path LoadFile(const std::filesystem::path& search_root,
                               const std::string& path,
                               pbrt_proto::v3::PbrtProto& output) {
  std::filesystem::path file_path(path);
  if (file_path.is_relative()) {
    file_path = search_root / file_path;
  }

  if (!std::filesystem::is_regular_file(file_path)) {
    std::cerr << "ERROR: Failed to open file: " << path << std::endl;
    exit(EXIT_FAILURE);
  }

  std::ifstream file(file_path, std::ios::binary | std::ios::in);
  if (file.fail()) {
    std::cerr << "ERROR: Failed to open file: " << path << std::endl;
    exit(EXIT_FAILURE);
  }

  if (path.ends_with(".v3.binpb")) {
    if (!output.ParseFromIstream(&file)) {
      std::cerr << "ERROR: Failed to parse binpb input: " << path << std::endl;
      exit(EXIT_FAILURE);
    }
  } else if (path.ends_with(".v3.txtpb")) {
    IstreamInputStream stream_wrapper(&file);
    if (!TextFormat::Parse(&stream_wrapper, &output)) {
      std::cerr << "ERROR: Failed to parse txtpb input: " << path << std::endl;
      exit(EXIT_FAILURE);
    }
  } else {
    if (absl::Status error = Convert(file, output); !error.ok()) {
      std::cerr << "ERROR: Failed to parse pbrt file with error: "
                << error.message() << std::endl;
      exit(EXIT_FAILURE);
    }
  }

  return file_path;
}

}  // namespace pbrt_frontend
}  // namespace iris
