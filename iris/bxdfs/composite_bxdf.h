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
        bxdfs_.data(), bxdfs_.data() + num_bxdfs_,
        [](const Bxdf* bxdf) { return bxdf->IsDiffuse(); });

    num_diffuse_bxdfs_ = std::distance(bxdfs_.begin(), partition_iter);

    diffuse_pdf_ = static_cast<visual_t>(0.0);
    for (size_t i = 0; i < num_diffuse_bxdfs_; i++) {
      visual_t pdf;
      bxdfs_[i]->IsDiffuse(&pdf);
      diffuse_pdf_ += std::clamp(pdf, static_cast<visual_t>(0.0),
                                 static_cast<visual_t>(1.0));
    }

    if (num_bxdfs_ != 0) {
      diffuse_pdf_ /= static_cast<visual_t>(num_bxdfs_);
    }
  }

  bool IsDiffuse(visual_t* diffuse_pdf) const override {
    if (diffuse_pdf != nullptr) {
      *diffuse_pdf = diffuse_pdf_;
    }

    return num_diffuse_bxdfs_ != 0;
  }

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

    std::optional<SampleResult> sample =
        bxdfs_[index]->Sample(incoming, differentials, surface_normal, sampler);
    if (!sample.has_value()) {
      return std::nullopt;
    }

    visual_t pdf_weight;
    if (sample->bxdf_override != nullptr) {
      pdf_weight = sample->pdf_weight / static_cast<visual_t>(num_bxdfs_);
    } else {
      pdf_weight = diffuse_pdf_;
    }

    return SampleResult{sample->direction, sample->differentials,
                        sample->bxdf_override, pdf_weight};
  }

  visual_t Pdf(const Vector& incoming, const Vector& outgoing,
               const Vector& surface_normal,
               Hemisphere hemisphere) const override {
    visual_t total_pdf = static_cast<visual_t>(0.0);
    if (num_diffuse_bxdfs_ == 0) {
      return total_pdf;
    }

    for (size_t i = 0; i < num_diffuse_bxdfs_; i++) {
      visual_t pdf =
          bxdfs_[i]->Pdf(incoming, outgoing, surface_normal, hemisphere);
      total_pdf += std::max(static_cast<visual_t>(0.0), pdf);
    }

    return total_pdf / static_cast<visual_t>(num_diffuse_bxdfs_);
  }

  const Reflector* Reflectance(const Vector& incoming, const Vector& outgoing,
                               Hemisphere hemisphere,
                               SpectralAllocator& allocator) const override {
    const Reflector* result = nullptr;
    for (size_t i = 0; i < num_diffuse_bxdfs_; i++) {
      const Reflector* reflectance =
          bxdfs_[i]->Reflectance(incoming, outgoing, hemisphere, allocator);
      result = allocator.UnboundedAdd(result, reflectance);
    }
    return result;
  }

 private:
  std::array<const Bxdf*, N> bxdfs_;
  size_t num_bxdfs_;
  size_t num_diffuse_bxdfs_;
  visual_t diffuse_pdf_;
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