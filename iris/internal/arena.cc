#include "iris/internal/arena.h"

namespace iris {
namespace internal {

void* Arena::Allocate(size_t size) {
  if (size == 0) {
    return nullptr;
  }

  size_t chunks_required = (size % sizeof(Chunk) == 0) ? 0 : 1;
  chunks_required += size / sizeof(Chunk);

  if (data_index_ == data_.size()) {
    data_.emplace_back();
  }

  data_[data_index_].resize(chunks_required);

  return static_cast<void*>(data_[data_index_++].data());
}

void Arena::Clear() { data_index_ = 0; }

}  // namespace internal
}  // namespace iris
