#ifndef _IRIS_BXDFS_COMPOSITE_BXDF_
#define _IRIS_BXDFS_COMPOSITE_BXDF_

#include <array>
#include <cassert>
#include <concepts>
#include <optional>

#include "iris/bxdf.h"
#include "iris/bxdf_allocator.h"
#include "iris/float.h"
#include "iris/reflector.h"
#include "iris/sampler.h"
#include "iris/spectral_allocator.h"
#include "iris/vector.h"

namespace iris {
namespace bxdfs {
namespace internal {

template <size_t NumBxdfs>
class CompositeBxdf final : public Bxdf {
 public:
  CompositeBxdf(const Bxdf* bxdfs[]) {
    for (size_t i = 0; i < NumBxdfs; i++) {
      bxdfs_[i] = bxdfs[i];
      assert(bxdfs_[i]);
    }
  }

  bool IsDiffuse() const override {
    for (size_t i = 0; i < NumBxdfs; i++) {
      if (bxdfs_[i]->IsDiffuse()) {
        return true;
      }
    }
    return false;
  }

  std::optional<Vector> SampleDiffuse(const Vector& incoming,
                                      const Vector& surface_normal,
                                      Sampler& sampler) const override {
    std::array<const Bxdf*, NumBxdfs> diffuse_bxdfs;

    size_t num_diffuse = 0;
    for (size_t i = 0; i < NumBxdfs; i++) {
      if (bxdfs_[i]->IsDiffuse()) {
        diffuse_bxdfs[num_diffuse++] = bxdfs_[i];
      }
    }

    if (num_diffuse == 0) {
      return std::nullopt;
    }

    size_t index = sampler.NextIndex(num_diffuse);
    return diffuse_bxdfs[index]->SampleDiffuse(incoming, surface_normal,
                                               sampler);
  }

  std::optional<SampleResult> Sample(
      const Vector& incoming, const std::optional<Differentials>& differentials,
      const Vector& surface_normal, Sampler& sampler) const override {
    size_t index = sampler.NextIndex(NumBxdfs);
    return bxdfs_[index]->Sample(incoming, differentials, surface_normal,
                                 sampler);
  }

  std::optional<visual_t> Pdf(const Vector& incoming, const Vector& outgoing,
                              const Vector& surface_normal,
                              const Bxdf* sample_source,
                              Hemisphere hemisphere) const override {
    visual_t diffuse_pdf = 0.0;
    size_t num_diffuse = 0;
    for (const auto* bxdf : bxdfs_) {
      auto pdf = bxdf->Pdf(incoming, outgoing, surface_normal, sample_source,
                           hemisphere);
      diffuse_pdf += pdf.value_or(0.0);
      num_diffuse += pdf.has_value();
    }

    std::optional<visual_t> result;
    if (num_diffuse != 0) {
      visual_t specular_pdf = static_cast<visual_t>(NumBxdfs - num_diffuse);
      result = (diffuse_pdf + specular_pdf) / static_cast<visual_t>(NumBxdfs);
    }

    return result;
  }

  const Reflector* Reflectance(const Vector& incoming, const Vector& outgoing,
                               const Bxdf* sample_source, Hemisphere hemisphere,
                               SpectralAllocator& allocator) const override {
    const Reflector* result = nullptr;
    for (const auto* bxdf : bxdfs_) {
      auto reflectance = bxdf->Reflectance(incoming, outgoing, sample_source,
                                           hemisphere, allocator);
      result = allocator.UnboundedAdd(result, reflectance);
    }
    return result;
  }

 private:
  std::array<const Bxdf*, NumBxdfs> bxdfs_;
  static_assert(NumBxdfs != 0);
};

template <std::size_t I, std::size_t N>
const Bxdf* MakeComposite(BxdfAllocator& allocator,
                          std::array<const Bxdf*, N>& storage) {
  if constexpr (I == 0) {
    return nullptr;
  } else if constexpr (I == 1) {
    return storage[0];
  } else {
    return &allocator.Allocate<internal::CompositeBxdf<I>>(storage.data());
  }
}

template <std::size_t I, std::size_t N>
const Bxdf* MakeComposite(BxdfAllocator& allocator,
                          std::array<const Bxdf*, N>& storage,
                          const Bxdf* bxdf) {
  if (bxdf) {
    storage[I] = bxdf;
    return MakeComposite<I + 1, N>(allocator, storage);
  } else {
    return MakeComposite<I, N>(allocator, storage);
  }
}

template <std::size_t I, std::size_t N>
const Bxdf* MakeComposite(BxdfAllocator& allocator,
                          std::array<const Bxdf*, N>& storage, const Bxdf* bxdf,
                          std::convertible_to<const Bxdf*> auto... args) {
  if (bxdf) {
    storage[I] = bxdf;
    return MakeComposite<I + 1, N>(allocator, storage, args...);
  } else {
    return MakeComposite<I, N>(allocator, storage, args...);
  }
}

}  // namespace internal

const Bxdf* MakeComposite(BxdfAllocator& allocator,
                          std::convertible_to<const Bxdf*> auto... args) {
  std::array<const Bxdf*, sizeof...(args)> storage;
  return internal::MakeComposite<0, sizeof...(args)>(allocator, storage,
                                                     args...);
}

}  // namespace bxdfs
}  // namespace iris

#endif  // _IRIS_BXDFS_COMPOSITE_BXDF_