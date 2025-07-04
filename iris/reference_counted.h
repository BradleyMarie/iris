#ifndef _IRIS_REFERENCE_COUNTED_
#define _IRIS_REFERENCE_COUNTED_

#include <concepts>
#include <type_traits>

#include "iris/reference_countable.h"

namespace iris {

template <typename T>
  requires std::derived_from<T, ReferenceCountable>
class ReferenceCounted final {
 public:
  ReferenceCounted() noexcept : ptr_(nullptr) {}

  ReferenceCounted(const ReferenceCounted& value) noexcept : ptr_(value.ptr_) {
    MaybeIncrement();
  }

  ReferenceCounted(ReferenceCounted&& value) noexcept : ptr_(value.ptr_) {
    value.ptr_ = nullptr;
  }

  template <class U>
    requires std::derived_from<U, T>
  ReferenceCounted(const ReferenceCounted<U>& value) noexcept
      : ptr_(value.ptr_) {
    MaybeIncrement();
  }

  template <class U>
    requires std::derived_from<U, T>
  ReferenceCounted(ReferenceCounted<U>&& value) noexcept : ptr_(value.ptr_) {
    value.ptr_ = nullptr;
  }

  ReferenceCounted& operator=(const ReferenceCounted& value) noexcept {
    MaybeDecrement();
    ptr_ = value.ptr_;
    MaybeIncrement();
    return *this;
  }

  ReferenceCounted& operator=(ReferenceCounted&& value) noexcept {
    MaybeDecrement();
    ptr_ = value.ptr_;
    value.ptr_ = nullptr;
    return *this;
  }

  ~ReferenceCounted() noexcept { MaybeDecrement(); }

  auto operator<=>(const ReferenceCounted& rhs) const = default;
  explicit operator bool() const noexcept { return ptr_; }
  const T* operator->() const noexcept { return ptr_; }
  const T& operator*() const noexcept { return *ptr_; }

  void Swap(ReferenceCounted& other) noexcept {
    T* temp = ptr_;
    ptr_ = other.ptr_;
    other.ptr_ = temp;
  }

  const T* Get() const noexcept { return ptr_; }

  void Reset() {
    MaybeDecrement();
    ptr_ = nullptr;
  }

 private:
  ReferenceCounted(T* value) noexcept : ptr_(value) {}

  void MaybeIncrement() {
    if (ptr_) {
      ptr_->Increment();
    }
  }

  void MaybeDecrement() {
    if (ptr_ && ptr_->Decrement()) {
      delete ptr_;
    }
  }

  T* ptr_;

  template <typename U>
    requires std::derived_from<U, ReferenceCountable>
  friend class ReferenceCounted;

  template <typename U, typename... Args>
    requires std::derived_from<U, ReferenceCountable>
  friend ReferenceCounted<U> MakeReferenceCounted(Args&&... args);
};

template <typename T, typename... Args>
  requires std::derived_from<T, ReferenceCountable>
ReferenceCounted<T> MakeReferenceCounted(Args&&... args) {
  return ReferenceCounted<T>(new T(std::forward<Args>(args)...));
}

template <typename T>
  requires std::derived_from<T, ReferenceCountable>
void swap(ReferenceCounted<T>& lhs, ReferenceCounted<T>& rhs) noexcept {
  lhs.Swap(rhs);
}

}  // namespace iris

namespace std {

template <typename T>
  requires std::derived_from<T, iris::ReferenceCountable>
void swap(iris::ReferenceCounted<T>& lhs,
          iris::ReferenceCounted<T>& rhs) noexcept {
  lhs.Swap(rhs);
}

template <typename T>
  requires std::derived_from<T, iris::ReferenceCountable>
struct hash<iris::ReferenceCounted<T>> {
  std::size_t operator()(
      const iris::ReferenceCounted<T>& value) const noexcept {
    std::hash<const T*> hash;
    return hash(value.Get());
  }
};

}  // namespace std

#endif  // _IRIS_REFERENCE_COUNTED_
