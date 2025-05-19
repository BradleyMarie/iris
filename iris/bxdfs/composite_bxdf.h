#ifndef _IRIS_BXDFS_COMPOSITE_BXDF_
#define _IRIS_BXDFS_COMPOSITE_BXDF_

#include <algorithm>
#include <array>
#include <cassert>
#include <concepts>
#include <optional>
#include <variant>

#include "iris/bxdf.h"
#include "iris/bxdf_allocator.h"
#include "iris/float.h"
#include "iris/reflector.h"
#include "iris/sampler.h"
#include "iris/spectral_allocator.h"
#include "iris/vector.h"

namespace iris {
namespace bxdfs {

const Bxdf* MakeCompositeBxdf(BxdfAllocator& allocator,
                              std::convertible_to<const Bxdf*> auto... args) {
  auto storage = std::to_array<const Bxdf*>({args...});

  size_t num_bxdfs = 0;
  for (size_t i = 0; i < storage.size(); i++) {
    if (storage[i] != nullptr) {
      std::swap(storage[i], storage[num_bxdfs++]);
    }
  }

  if (num_bxdfs == 0) {
    return nullptr;
  }

  if (num_bxdfs == 1) {
    return storage.front();
  }

  class CompositeBxdf final : public Bxdf {
   public:
    CompositeBxdf(const Bxdf* bxdfs[], size_t num_bxdfs)
        : num_bxdfs_(num_bxdfs),
          num_diffuse_bxdfs_(0),
          diffuse_pdf_(static_cast<visual_t>(0.0)) {
      size_t specular_location = num_bxdfs - 1;
      for (size_t i = 0; i < num_bxdfs; i++) {
        visual_t pdf;
        if (bxdfs[i]->IsDiffuse(&pdf)) {
          diffuse_pdf_ += std::clamp(pdf, static_cast<visual_t>(0.0),
                                     static_cast<visual_t>(1.0));
          bxdfs_[num_diffuse_bxdfs_++] = bxdfs[i];
        } else {
          bxdfs_[specular_location--] = bxdfs[i];
        }
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

    std::variant<std::monostate, DiffuseSample, SpecularSample> Sample(
        const Vector& incoming,
        const std::optional<Differentials>& differentials,
        const Vector& surface_normal, Sampler& sampler,
        SpectralAllocator& allocator) const override {
      size_t index = sampler.NextIndex(num_bxdfs_);

      auto sample = bxdfs_[index]->Sample(incoming, differentials,
                                          surface_normal, sampler, allocator);
      if (SpecularSample* specular_sample =
              std::get_if<SpecularSample>(&sample);
          specular_sample != nullptr) {
        specular_sample->pdf /= static_cast<visual_t>(num_bxdfs_);
      }

      return sample;
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

    visual_t PdfDiffuse(const Vector& incoming, const Vector& outgoing,
                        const Vector& surface_normal,
                        Hemisphere hemisphere) const override {
      visual_t total_pdf = static_cast<visual_t>(0.0);
      if (num_diffuse_bxdfs_ == 0) {
        return total_pdf;
      }

      for (size_t i = 0; i < num_diffuse_bxdfs_; i++) {
        visual_t pdf = bxdfs_[i]->PdfDiffuse(incoming, outgoing, surface_normal,
                                             hemisphere);
        total_pdf += std::max(static_cast<visual_t>(0.0), pdf);
      }

      return total_pdf / static_cast<visual_t>(num_diffuse_bxdfs_);
    }

    const Reflector* ReflectanceDiffuse(
        const Vector& incoming, const Vector& outgoing, Hemisphere hemisphere,
        SpectralAllocator& allocator) const override {
      const Reflector* result = nullptr;
      for (size_t i = 0; i < num_diffuse_bxdfs_; i++) {
        const Reflector* reflectance = bxdfs_[i]->ReflectanceDiffuse(
            incoming, outgoing, hemisphere, allocator);
        result = allocator.UnboundedAdd(result, reflectance);
      }
      return result;
    }

   private:
    std::array<const Bxdf*, storage.size()> bxdfs_;
    size_t num_bxdfs_;
    size_t num_diffuse_bxdfs_;
    visual_t diffuse_pdf_;
  };

  return &allocator.Allocate<CompositeBxdf>(storage.data(), num_bxdfs);
}

}  // namespace bxdfs
}  // namespace iris

#endif  // _IRIS_BXDFS_COMPOSITE_BXDF_
