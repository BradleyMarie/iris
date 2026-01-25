#include "frontends/pbrt/directives.h"

#include <filesystem>
#include <optional>
#include <utility>

#include "google/protobuf/arena.h"
#include "pbrt_proto/v3/v3.pb.h"

namespace iris {
namespace pbrt_frontend {

using ::google::protobuf::Arena;
using ::pbrt_proto::v3::Directive;
using ::pbrt_proto::v3::PbrtProto;

void Directives::Include(pbrt_proto::v3::PbrtProto directives,
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
  state.directives_storage = std::make_unique<PbrtProto>(std::move(directives));
  state.directives = state.directives_storage.get();
  state.file = std::move(included_file);
  state.current = -1;
  state_.push_back(std::move(state));
}

void Directives::Include(std::unique_ptr<Arena> arena,
                         const PbrtProto* directives,
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
  state.arena = std::move(arena);
  state.directives = directives;
  state.file = std::move(included_file);
  state.current = -1;
  state_.push_back(std::move(state));
}

const Directive* Directives::Next() {
  for (;;) {
    if (state_.empty()) {
      return nullptr;
    }

    state_.back().current += 1;

    if (state_.back().current != state_.back().directives->directives_size()) {
      break;
    }

    if (state_.back().file.has_value()) {
      files_.erase(*state_.back().file);
    }

    state_.pop_back();
  }

  return &state_.back().directives->directives(state_.back().current);
}

bool Directives::HasNext() const {
  for (auto it = state_.rbegin(); it != state_.rend(); it++) {
    if (it->directives->directives_size() != 0 &&
        it->current < it->directives->directives_size() - 1) {
      return true;
    }
  }

  return false;
}

}  // namespace pbrt_frontend
}  // namespace iris
