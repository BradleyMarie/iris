#include "iris/bxdfs/fourier_bxdf.h"

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <optional>
#include <span>
#include <utility>

#include "iris/bxdf.h"
#include "iris/bxdf_allocator.h"
#include "iris/bxdfs/internal/diffuse_bxdf.h"
#include "iris/bxdfs/internal/math.h"
#include "iris/float.h"
#include "iris/reflectors/uniform_reflector.h"

namespace iris {
namespace bxdfs {
namespace {

using ::iris::bxdfs::internal::CosTheta;
using ::iris::reflectors::CreateUniformReflector;

class FourierBxdf : public internal::DiffuseBxdf {
 public:
  FourierBxdf(
      const Reflector* r, const Reflector* g, const Reflector* b,
      std::span<const geometric> elevational_samples,
      std::span<const visual> cdf,
      std::span<const std::pair<uint32_t, uint32_t>> coefficient_extents,
      std::span<const visual> y_coefficients,
      std::span<const visual> r_coefficients,
      std::span<const visual> b_coefficients, visual eta_transmitted)
      : r_(r),
        g_(g),
        b_(b),
        elevational_samples_(elevational_samples),
        cdf_(cdf),
        coefficient_extents_(coefficient_extents),
        y_coefficients_(y_coefficients),
        r_coefficients_(r_coefficients),
        b_coefficients_(b_coefficients),
        eta_transmitted_(eta_transmitted) {}

  std::optional<Vector> SampleDiffuse(const Vector& incoming,
                                      const Vector& surface_normal,
                                      Sampler& sampler) const override;

  visual_t PdfDiffuse(const Vector& incoming, const Vector& outgoing,
                      const Vector& surface_normal,
                      Hemisphere hemisphere) const override;

  const Reflector* ReflectanceDiffuse(
      const Vector& incoming, const Vector& outgoing, Hemisphere hemisphere,
      SpectralAllocator& allocator) const override;

 private:
  const Reflector* r_;
  const Reflector* g_;
  const Reflector* b_;
  std::span<const geometric> elevational_samples_;
  std::span<const visual> cdf_;
  std::span<const std::pair<uint32_t, uint32_t>> coefficient_extents_;
  std::span<const visual> y_coefficients_;
  std::span<const visual> r_coefficients_;
  std::span<const visual> b_coefficients_;
  visual eta_transmitted_;
};

const static ReferenceCounted<Reflector> kUniform =
    CreateUniformReflector(static_cast<visual>(1.0));

double CosDPhi(const Vector& wa, const Vector& wb) {
  double waxy = wa.x * wa.x + wa.y * wa.y;
  double wbxy = wb.x * wb.x + wb.y * wb.y;
  if (waxy == 0.0 || wbxy == 0.0) {
    return 1.0;
  }

  return std::clamp((wa.x * wb.x + wa.y * wb.y) / std::sqrt(waxy * wbxy), -1.0,
                    1.0);
}

struct CatmullRomWeights {
  size_t offsets[4];
  visual_t weights[4];
  size_t num_weights;
};

std::optional<CatmullRomWeights> ComputeCatmullRomWeights(
    std::span<const geometric> elevational_samples, geometric value) {
  if (elevational_samples.empty() || value < elevational_samples.front()) {
    return std::nullopt;
  }

  auto iterator = std::upper_bound(elevational_samples.begin(),
                                   elevational_samples.end(), value);
  if (iterator == elevational_samples.end()) {
    return std::nullopt;
  }

  if (*iterator != value) {
    iterator -= 1;
  }

  size_t offsets[4] = {
      static_cast<size_t>((iterator - 1) - elevational_samples.begin()),
      static_cast<size_t>((iterator + 0) - elevational_samples.begin()),
      static_cast<size_t>((iterator + 1) - elevational_samples.begin()),
      static_cast<size_t>((iterator + 2) - elevational_samples.begin())};
  visual_t weights[4] = {static_cast<visual_t>(0.0), static_cast<visual_t>(0.0),
                         static_cast<visual_t>(0.0),
                         static_cast<visual_t>(0.0)};

  visual_t x0 = iterator[0];
  visual_t x1 = iterator[1];

  visual_t interval_width = x1 - x0;
  visual_t t = (value - x0) / interval_width;
  visual_t t2 = t * t;
  visual_t t3 = t2 * t;

  weights[1] = static_cast<visual_t>(2.0) * t3 -
               static_cast<visual_t>(3.0) * t2 + static_cast<visual_t>(1.0);
  weights[2] =
      static_cast<visual_t>(-2.0) * t3 + static_cast<visual_t>(3.0) * t2;

  if (iterator > elevational_samples.begin()) {
    visual_t w0 = (t3 - static_cast<visual_t>(2.0) * t2 + t) * (x1 - x0) /
                  (x1 - iterator[-1]);
    weights[0] = -w0;
    weights[2] += w0;
  } else {
    visual_t w0 = t3 - static_cast<visual_t>(2.0) * t2 + t;
    weights[0] = static_cast<visual_t>(0.0);
    weights[1] -= w0;
    weights[2] += w0;
  }

  if (iterator + 2 < elevational_samples.end()) {
    visual_t w3 = (t3 - t2) * (x1 - x0) / (iterator[2] - x0);
    weights[1] -= w3;
    weights[3] = w3;
  } else {
    visual_t w3 = t3 - t2;
    weights[1] -= w3;
    weights[2] += w3;
    weights[3] = static_cast<visual_t>(0.0);
  }

  CatmullRomWeights result;
  result.num_weights = 0;

  for (size_t i = 0; i < 4; i++) {
    if (weights[i] == static_cast<visual_t>(0.0)) {
      continue;
    }

    result.offsets[result.num_weights] = offsets[i];
    result.weights[result.num_weights] = weights[i];
    result.num_weights += 1;
  }

  return result;
}

visual_t ComputeChannel(
    const CatmullRomWeights& incoming_weights,
    const CatmullRomWeights& outgoing_weights, double cos_phi,
    std::span<const std::pair<uint32_t, uint32_t>> coefficient_extents,
    std::span<const visual> all_coefficients, size_t num_elevational_samples) {
  std::span<const visual_t> coefficients[4][4];
  double weights[4][4];
  size_t num_coefficients = 0;
  for (size_t i_index = 0; i_index < incoming_weights.num_weights; i_index++) {
    for (size_t o_index = 0; o_index < outgoing_weights.num_weights;
         o_index++) {
      auto [start_index, length] =
          coefficient_extents[incoming_weights.offsets[i_index] *
                                  num_elevational_samples +
                              outgoing_weights.offsets[o_index]];
      assert(start_index + length <= all_coefficients.size());
      coefficients[i_index][o_index] =
          std::span(all_coefficients.begin() + start_index, length);
      num_coefficients =
          std::max(num_coefficients, static_cast<size_t>(length));

      weights[i_index][o_index] =
          incoming_weights.weights[i_index] * outgoing_weights.weights[o_index];
    }
  }

  double result = 0.0;
  double cosKMinusOnePhi = cos_phi;
  double cosKPhi = 1.0;
  for (size_t coefficient_index = 0; coefficient_index < num_coefficients;
       coefficient_index++) {
    double value = 0.0;
    for (size_t i_index = 0; i_index < incoming_weights.num_weights;
         i_index++) {
      for (size_t o_index = 0; o_index < outgoing_weights.num_weights;
           o_index++) {
        if (coefficient_index < coefficients[i_index][o_index].size()) {
          value += coefficients[i_index][o_index][coefficient_index] *
                   weights[i_index][o_index];
        }
      }
    }

    result += value * cosKPhi;

    double cosKPlusOnePhi = 2.0 * cos_phi * cosKPhi - cosKMinusOnePhi;
    cosKMinusOnePhi = cosKPhi;
    cosKPhi = cosKPlusOnePhi;
  }

  return static_cast<visual_t>(result);
}

std::optional<Vector> FourierBxdf::SampleDiffuse(const Vector& incoming,
                                                 const Vector& surface_normal,
                                                 Sampler& sampler) const {
  Vector outgoing = internal::CosineSampleHemisphere(incoming.z, sampler);
  return outgoing.AlignWith(surface_normal);
}

visual_t FourierBxdf::PdfDiffuse(const Vector& incoming, const Vector& outgoing,
                                 const Vector& surface_normal,
                                 Hemisphere hemisphere) const {
  if (hemisphere != Hemisphere::BRDF) {
    return static_cast<visual_t>(0.0);
  }

  return std::abs(static_cast<visual_t>(outgoing.z) *
                  std::numbers::inv_pi_v<visual_t>);
}

const Reflector* FourierBxdf::ReflectanceDiffuse(
    const Vector& incoming, const Vector& outgoing, Hemisphere hemisphere,
    SpectralAllocator& allocator) const {
  // TODO: Remove this
  if (hemisphere != Hemisphere::BRDF) {
    return nullptr;
  }

  Vector reversed_outgoing = -outgoing;
  geometric mu_incoming = CosTheta(incoming);
  geometric mu_outgoing = CosTheta(reversed_outgoing);
  if (mu_incoming == static_cast<visual_t>(0.0) ||
      mu_outgoing == static_cast<visual_t>(0.0)) {
    return nullptr;
  }

  std::optional<CatmullRomWeights> incoming_weights =
      ComputeCatmullRomWeights(elevational_samples_, mu_incoming);
  if (!incoming_weights) {
    return nullptr;
  }

  std::optional<CatmullRomWeights> outgoing_weights =
      ComputeCatmullRomWeights(elevational_samples_, mu_outgoing);
  if (!outgoing_weights) {
    return nullptr;
  }

  double cos_phi = CosDPhi(reversed_outgoing, incoming);
  visual_t y = ComputeChannel(*incoming_weights, *outgoing_weights, cos_phi,
                              coefficient_extents_, y_coefficients_,
                              elevational_samples_.size());

  const Reflector* result;
  if (r_coefficients_.empty() || b_coefficients_.empty()) {
    result = allocator.Scale(
        kUniform.Get(),
        std::clamp(y, static_cast<visual_t>(0.0), static_cast<visual_t>(1.0)));
  } else {
    visual_t r = ComputeChannel(*incoming_weights, *outgoing_weights, cos_phi,
                                coefficient_extents_, r_coefficients_,
                                elevational_samples_.size());
    visual_t b = ComputeChannel(*incoming_weights, *outgoing_weights, cos_phi,
                                coefficient_extents_, b_coefficients_,
                                elevational_samples_.size());
    visual_t g = static_cast<visual_t>(1.398290) * y -
                 static_cast<visual_t>(0.297375) * r -
                 static_cast<visual_t>(0.100913) * b;

    result = allocator.Add(
        allocator.Scale(r_, std::clamp(r, static_cast<visual_t>(0.0),
                                       static_cast<visual_t>(1.0))),
        allocator.Scale(g_, std::clamp(g, static_cast<visual_t>(0.0),
                                       static_cast<visual_t>(1.0))),
        allocator.Scale(b_, std::clamp(b, static_cast<visual_t>(0.0),
                                       static_cast<visual_t>(1.0))));
  }

  return allocator.UnboundedScale(
      result, static_cast<visual_t>(1.0) / std::abs(mu_outgoing));
}

}  // namespace

const Bxdf* MakeFourierBxdf(
    BxdfAllocator& bxdf_allocator,
    std::span<const geometric> elevational_samples, std::span<const visual> cdf,
    std::span<const std::pair<uint32_t, uint32_t>> coefficient_extents,
    std::span<const visual> y_coefficients, visual eta_transmitted) {
  size_t num_samples = elevational_samples.size();
  if (num_samples < 3 || cdf.size() / num_samples != num_samples ||
      coefficient_extents.size() / num_samples != num_samples ||
      !std::isfinite(eta_transmitted) ||
      eta_transmitted < static_cast<visual_t>(1.0)) {
    return nullptr;
  }

  return &bxdf_allocator.Allocate<FourierBxdf>(
      nullptr, nullptr, nullptr, elevational_samples, cdf, coefficient_extents,
      y_coefficients, std::span<const visual>(), std::span<const visual>(),
      eta_transmitted);
}

const Bxdf* MakeFourierBxdf(
    BxdfAllocator& bxdf_allocator, const Reflector* r, const Reflector* g,
    const Reflector* b, std::span<const geometric> elevational_samples,
    std::span<const visual> cdf,
    std::span<const std::pair<uint32_t, uint32_t>> coefficient_extents,
    std::span<const visual> y_coefficients,
    std::span<const visual> r_coefficients,
    std::span<const visual> b_coefficients, visual eta_transmitted) {
  size_t num_samples = elevational_samples.size();
  if (num_samples < 3 || cdf.size() / num_samples != num_samples ||
      coefficient_extents.size() / num_samples != num_samples ||
      !std::isfinite(eta_transmitted) ||
      eta_transmitted < static_cast<visual_t>(1.0)) {
    return nullptr;
  }

  return &bxdf_allocator.Allocate<FourierBxdf>(
      r, g, b, elevational_samples, cdf, coefficient_extents, y_coefficients,
      r_coefficients, b_coefficients, eta_transmitted);
}

}  // namespace bxdfs
}  // namespace iris
