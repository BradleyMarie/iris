#ifndef _IRIS_SCENES_INTERNAL_ALIGNED_VECTOR_
#define _IRIS_SCENES_INTERNAL_ALIGNED_VECTOR_

#include <algorithm>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <limits>
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
    std::size_t allocation_size = n * sizeof(T);

    std::size_t max_alignment;
    if constexpr (sizeof(void*) > sizeof(uint32_t)) {
      if (for_scene_) {
        max_alignment = 32 * 1024 * 1024;  // 32MB
      } else {
        max_alignment = 128;  // 128B
      }
    } else {
      if (for_scene_) {
        max_alignment = 1024 * 1024 * 1024;  // 1GB
      } else {
        max_alignment = 4096;  // 4KB
      }
    }

    std::size_t alignment =
        std::min(max_alignment, std::bit_floor(allocation_size));
    if (std::size_t padding_needed = allocation_size % alignment;
        padding_needed != 0u) {
      if (std::numeric_limits<std::size_t>::max() - allocation_size >
          padding_needed) {
        allocation_size += padding_needed;
      } else {
        alignment = 1u;
        while (allocation_size % alignment == 0u) {
          alignment <<= 1u;
        }
      }
    }

#ifdef MSVC
    void* result = _aligned_malloc(alignment, allocation_size);
#else
    void* result = std::malloc(allocation_size);
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
