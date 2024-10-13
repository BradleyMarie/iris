#ifndef _IRIS_INTERNAL_ARENA_
#define _IRIS_INTERNAL_ARENA_

#include <cstddef>
#include <type_traits>
#include <vector>

namespace iris {
namespace internal {

class Arena final {
 public:
  Arena() = default;

  void* Allocate(size_t size);

  template <typename T, typename... Args>
  T& Allocate(Args&&... args) {
    T* result = new (Allocate(sizeof(T))) T(std::forward<Args>(args)...);
    return *result;
  }

  void Clear();

 private:
  Arena(const Arena&) = delete;
  Arena& operator=(const Arena&) = delete;

  std::vector<std::vector<char>> data_;
  size_t data_index_ = 0;
};

}  // namespace internal
}  // namespace iris

#endif  // _IRIS_INTERNAL_ARENA_