#ifndef _FRONTENDS_PBRT_DIRECTIVES_
#define _FRONTENDS_PBRT_DIRECTIVES_

#include <filesystem>
#include <memory>
#include <optional>
#include <set>
#include <stack>

#include "google/protobuf/repeated_ptr_field.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {

class Directives {
 public:
  void Include(
      pbrt_proto::v3::PbrtProto directives, std::filesystem::path search_root,
      std::optional<std::filesystem::path> included_file = std::nullopt);
  const pbrt_proto::v3::Directive* Next(std::filesystem::path& search_root);

 private:
  struct State {
    std::unique_ptr<pbrt_proto::v3::PbrtProto> directives;
    std::filesystem::path search_root;
    std::optional<std::filesystem::path> file;
    google::protobuf::RepeatedPtrField<pbrt_proto::v3::Directive>::iterator cur;
    google::protobuf::RepeatedPtrField<pbrt_proto::v3::Directive>::iterator end;
  };

  std::stack<State> state_;
  std::set<std::filesystem::path> files_;
  pbrt_proto::v3::Directive next_;
};

}  // namespace pbrt_frontend
}  // namespace iris

#endif  // _FRONTENDS_PBRT_DIRECTIVES_
