#ifndef _IRIS_BXDFS_COMPOSITE_BXDF_
#define _IRIS_BXDFS_COMPOSITE_BXDF_

#include <algorithm>
#include <array>
#include <cassert>
#include <concepts>
#include <iterator>
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

template <size_t N>
class CompositeBxdf final : public Bxdf {
 public:
  CompositeBxdf(const Bxdf* bxdfs[], size_t num_bxdfs) : num_bxdfs_(num_bxdfs) {
    std::copy(bxdfs, bxdfs + num_bxdfs, bxdfs_.data());
    auto partition_iter = std::stable_partition(
        bxdfs_.begin(), bxdfs_.end(),
        [](const Bxdf* bxdf) { return bxdf->IsDiffuse(); });
    num_diffuse_bxdfs_ = std::distance(bxdfs_.begin(), partition_iter);
  }

  bool IsDiffuse() const override { return num_diffuse_bxdfs_ != 0; }

  std::optional<Vector> SampleDiffuse(const Vector& incoming,
                                      const Vector& surface_normal,
                                      Sampler& sampler) const override {
    if (num_diffuse_bxdfs_ == 0) {
      return std::nullopt;
    }

    size_t index = sampler.NextIndex(num_diffuse_bxdfs_);
    return bxdfs_[index]->SampleDiffuse(incoming, surface_normal, sampler);
  }

  std::optional<SampleResult> Sample(
      const Vector& incoming, const std::optional<Differentials>& differentials,
      const Vector& surface_normal, Sampler& sampler) const override {
    size_t index = sampler.NextIndex(num_bxdfs_);
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
      visual_t specular_pdf = static_cast<visual_t>(num_bxdfs_ - num_diffuse);
      result = (diffuse_pdf + specular_pdf) / static_cast<visual_t>(num_bxdfs_);
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
  std::array<const Bxdf*, N> bxdfs_;
  size_t num_bxdfs_;
  size_t num_diffuse_bxdfs_;
};

}  // namespace internal

const Bxdf* MakeComposite(BxdfAllocator& allocator,
                          std::convertible_to<const Bxdf*> auto... args) {
  auto storage = std::to_array<const Bxdf*>({args...});

  auto partition_iter =
      std::stable_partition(storage.begin(), storage.end(),
                            [](const Bxdf* bxdf) { return bxdf != nullptr; });
  auto num_elements = std::distance(storage.begin(), partition_iter);

  if (num_elements == 0) {
    return nullptr;
  }

  if (num_elements == 1) {
    return storage.front();
  }

  return &allocator.Allocate<internal::CompositeBxdf<storage.size()>>(
      storage.data(), num_elements);
}

}  // namespace bxdfs
}  // namespace iris

#endif  // _IRIS_BXDFS_COMPOSITE_BXDF_