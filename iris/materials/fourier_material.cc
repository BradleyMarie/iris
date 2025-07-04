#include "iris/materials/fourier_material.h"

#include <cstddef>
#include <span>
#include <utility>
#include <vector>

#include "iris/bxdf.h"
#include "iris/bxdf_allocator.h"
#include "iris/bxdfs/fourier_bxdf.h"
#include "iris/float.h"
#include "iris/material.h"
#include "iris/reference_counted.h"
#include "iris/reflector.h"
#include "iris/reflectors/uniform_reflector.h"
#include "iris/spectral_allocator.h"
#include "iris/texture_coordinates.h"

namespace iris {
namespace materials {
namespace {

using ::iris::bxdfs::MakeFourierBxdf;
using ::iris::reflectors::CreateUniformReflector;

static const ReferenceCounted<Reflector> kReflectance =
    CreateUniformReflector(static_cast<visual_t>(1.0));

class FourierMaterial : public Material {
 public:
  FourierMaterial(ReferenceCounted<Reflector> r, ReferenceCounted<Reflector> g,
                  ReferenceCounted<Reflector> b,
                  std::vector<geometric> elevational_samples,
                  std::vector<visual> cdf,
                  std::vector<std::pair<size_t, size_t>> coefficient_extents,
                  std::vector<visual> y_coefficients,
                  std::vector<visual> r_coefficients,
                  std::vector<visual> b_coefficients, visual eta)
      : r_(std::move(r)),
        g_(std::move(g)),
        b_(std::move(b)),
        elevational_samples_(std::move(elevational_samples)),
        cdf_(std::move(cdf)),
        coefficient_extents_(std::move(coefficient_extents)),
        y_coefficients_(std::move(y_coefficients)),
        r_coefficients_(std::move(r_coefficients)),
        b_coefficients_(std::move(b_coefficients)),
        eta_(eta) {}

  const Bxdf* Evaluate(const TextureCoordinates& texture_coordinates,
                       SpectralAllocator& spectral_allocator,
                       BxdfAllocator& bxdf_allocator) const override;

 private:
  ReferenceCounted<Reflector> r_;
  ReferenceCounted<Reflector> g_;
  ReferenceCounted<Reflector> b_;
  std::vector<geometric> elevational_samples_;
  std::vector<visual> cdf_;
  std::vector<std::pair<size_t, size_t>> coefficient_extents_;
  std::vector<visual> y_coefficients_;
  std::vector<visual> r_coefficients_;
  std::vector<visual> b_coefficients_;
  visual eta_;
};

const Bxdf* FourierMaterial::Evaluate(
    const TextureCoordinates& texture_coordinates,
    SpectralAllocator& spectral_allocator,
    BxdfAllocator& bxdf_allocator) const {
  if (!r_coefficients_.empty() && !b_coefficients_.empty()) {
    return MakeFourierBxdf(bxdf_allocator, r_.Get(), g_.Get(), b_.Get(),
                           elevational_samples_, cdf_, coefficient_extents_,
                           y_coefficients_, r_coefficients_, b_coefficients_,
                           eta_);
  }

  return MakeFourierBxdf(bxdf_allocator, kReflectance.Get(),
                         elevational_samples_, cdf_, coefficient_extents_,
                         y_coefficients_, eta_);
}

}  // namespace

ReferenceCounted<Material> MakeFourierMaterial(
    std::vector<geometric> elevational_samples, std::vector<visual> cdf,
    std::vector<std::pair<size_t, size_t>> coefficient_extents,
    std::vector<visual> y_coefficients, visual eta) {
  return MakeReferenceCounted<FourierMaterial>(
      ReferenceCounted<Reflector>(), ReferenceCounted<Reflector>(),
      ReferenceCounted<Reflector>(), std::move(elevational_samples),
      std::move(cdf), coefficient_extents, std::move(y_coefficients),
      std::vector<visual>(), std::vector<visual>(), eta);
}

ReferenceCounted<Material> MakeFourierMaterial(
    ReferenceCounted<Reflector> r, ReferenceCounted<Reflector> g,
    ReferenceCounted<Reflector> b, std::vector<geometric> elevational_samples,
    std::vector<visual> cdf,
    std::vector<std::pair<size_t, size_t>> coefficient_extents,
    std::vector<visual> y_coefficients, std::vector<visual> r_coefficients,
    std::vector<visual> b_coefficients, visual eta) {
  return MakeReferenceCounted<FourierMaterial>(
      std::move(r), std::move(g), std::move(b), std::move(elevational_samples),
      std::move(cdf), coefficient_extents, std::move(y_coefficients),
      std::move(r_coefficients), std::move(b_coefficients), eta);
}

}  // namespace materials
}  // namespace iris
