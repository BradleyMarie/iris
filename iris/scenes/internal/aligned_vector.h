#ifndef _IRIS_SCENES_INTERNAL_ALIGNED_VECTOR_
#define _IRIS_SCENES_INTERNAL_ALIGNED_VECTOR_

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <vector>

namespace iris {
namespace scenes {
namespace internal {
namespace internal {

template <typename T>
class AlignedAllocator {
 public:
  using value_type = T;

  AlignedAllocator(bool for_scene) noexcept : for_scene_(for_scene) {}

  template <typename U>
  AlignedAllocator(const AlignedAllocator<U>& other) noexcept
      : for_scene_(other.for_scene_) {}

  T* allocate(std::size_t n) {
    std::size_t alignment;
    if constexpr (sizeof(void*) > sizeof(uint32_t)) {
      if (for_scene_) {
        alignment = 32 * 1024 * 1024;  // 32MB
      } else {
        alignment = 128;  // 128B
      }
    } else {
      if (for_scene_) {
        alignment = 1024 * 1024 * 1024;  // 1GB
      } else {
        alignment = 4096;  // 4KB
      }
    }

#ifdef MSVC
    void* result = _aligned_malloc(alignment, n * sizeof(T));
#else
    void* result = std::aligned_alloc(alignment, n * sizeof(T));
#endif  // MSVC

    if (!result) {
      throw std::bad_alloc();
    }

    return static_cast<T*>(result);
  }

  void deallocate(T* p, std::size_t n) noexcept {
#ifdef MSVC
    _aligned_free(alignment, n);
#else
    std::free(p);
#endif  // MSVC
  }

  template <typename U>
  bool operator==(const AlignedAllocator<U>& other) const noexcept {
    return for_scene_ == other.for_scene_;
  }

  template <typename U>
  bool operator!=(const AlignedAllocator<U>& other) const noexcept {
    return for_scene_ != other.for_scene_;
  }

 private:
  bool for_scene_;
};

}  // namespace internal

template <typename T>
using AlignedVector = std::vector<T, internal::AlignedAllocator<T>>;

template <typename T>
AlignedVector<T> MakeAlignedVector(bool for_scene) {
  return AlignedVector<T>(internal::AlignedAllocator<T>(for_scene));
}

}  // namespace internal
}  // namespace scenes
}  // namespace iris

#endif  // _IRIS_SCENES_INTERNAL_ALIGNED_VECTOR_
