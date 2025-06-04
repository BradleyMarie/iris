#include "iris/bxdfs/composite_bxdf.h"

#include <algorithm>
#include <initializer_list>
#include <optional>
#include <span>
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

class CompositeBxdf final : public Bxdf {
 public:
  CompositeBxdf(std::span<const Bxdf*> bxdfs)
      : bxdfs_(bxdfs),
        num_diffuse_bxdfs_(0),
        diffuse_pdf_(static_cast<visual_t>(0.0)) {
    for (size_t i = 0; i < bxdfs_.size(); i++) {
      visual_t pdf;
      if (bxdfs_[i]->IsDiffuse(&pdf)) {
        diffuse_pdf_ += std::clamp(pdf, static_cast<visual_t>(0.0),
                                   static_cast<visual_t>(1.0));
        std::swap(bxdfs_[num_diffuse_bxdfs_++], bxdfs_[i]);
      }
    }

    if (bxdfs_.size() != 0) {
      diffuse_pdf_ /= static_cast<visual_t>(bxdfs_.size());
    }
  }

  bool IsDiffuse(visual_t* diffuse_pdf) const override {
    if (diffuse_pdf != nullptr) {
      *diffuse_pdf = diffuse_pdf_;
    }

    return num_diffuse_bxdfs_ != 0;
  }

  std::variant<std::monostate, DiffuseSample, SpecularSample> Sample(
      const Vector& incoming, const std::optional<Differentials>& differentials,
      const Vector& surface_normal, Sampler& sampler,
      SpectralAllocator& allocator) const override {
    size_t index = sampler.NextIndex(bxdfs_.size());

    auto sample = bxdfs_[index]->Sample(incoming, differentials, surface_normal,
                                        sampler, allocator);
    if (SpecularSample* specular_sample = std::get_if<SpecularSample>(&sample);
        specular_sample != nullptr) {
      specular_sample->pdf /= static_cast<visual_t>(bxdfs_.size());
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
      visual_t pdf =
          bxdfs_[i]->PdfDiffuse(incoming, outgoing, surface_normal, hemisphere);
      if (pdf > static_cast<visual_t>(0.0)) {
        total_pdf += pdf;
      }
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
  std::span<const Bxdf*> bxdfs_;
  size_t num_diffuse_bxdfs_;
  visual_t diffuse_pdf_;
};

const Bxdf* MakeCompositeBxdf(BxdfAllocator& allocator,
                              std::initializer_list<const Bxdf*> bxdfs) {
  std::span<const Bxdf*> on_heap = allocator.AllocateList(bxdfs);
  if (on_heap.empty()) {
    return nullptr;
  }

  if (on_heap.size() == 1) {
    return on_heap[0];
  }

  return &allocator.Allocate<CompositeBxdf>(on_heap);
}

}  // namespace bxdfs
}  // namespace iris
