#include "frontends/pbrt/directives.h"

#include <filesystem>
#include <memory>
#include <optional>
#include <utility>

#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {

using ::pbrt_proto::v3::Directive;
using ::pbrt_proto::v3::PbrtProto;

void Directives::Include(PbrtProto directives,
                         std::optional<std::filesystem::path> included_file) {
  if (included_file.has_value()) {
    *included_file = std::filesystem::weakly_canonical(*included_file);
    if (auto [_, inserted] = files_.insert(*included_file); !inserted) {
      std::cerr << "ERROR: Detected cyclic Include of file: "
                << included_file->native() << std::endl;
      exit(EXIT_FAILURE);
    }
  }

  State state;
  state.directives = std::make_unique<PbrtProto>(std::move(directives));
  state.file = std::move(included_file);
  state.cur = state.directives->mutable_directives()->begin();
  state.end = state.directives->mutable_directives()->end();
  state_.push(std::move(state));
}

const Directive* Directives::Next() {
  for (;;) {
    if (state_.empty()) {
      return nullptr;
    }

    if (state_.top().cur != state_.top().end) {
      break;
    }

    if (state_.top().file.has_value()) {
      files_.erase(*state_.top().file);
    }

    state_.pop();
  }

  next_ = *state_.top().cur++;

  return &next_;
}

}  // namespace pbrt_frontend
}  // namespace iris
