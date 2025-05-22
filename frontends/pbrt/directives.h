#ifndef _FRONTENDS_PBRT_DIRECTIVES_
#define _FRONTENDS_PBRT_DIRECTIVES_

#include <deque>
#include <optional>
#include <set>

#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {

class Directives {
 public:
  void Include(
      pbrt_proto::v3::PbrtProto directives,
      std::optional<std::filesystem::path> included_file = std::nullopt);
  const pbrt_proto::v3::Directive* Next();

 private:
  struct State {
    pbrt_proto::v3::PbrtProto directives;
    std::optional<std::filesystem::path> file;
    int current;
  };

  std::deque<State> state_;
  std::set<std::filesystem::path> files_;
};

}  // namespace pbrt_frontend
}  // namespace iris

#endif  // _FRONTENDS_PBRT_DIRECTIVES_
