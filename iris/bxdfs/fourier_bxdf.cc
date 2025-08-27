#include "iris/bxdfs/fourier_bxdf.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <numbers>
#include <optional>
#include <span>
#include <utility>
#include <vector>

#include "iris/bxdf.h"
#include "iris/bxdf_allocator.h"
#include "iris/bxdfs/internal/diffuse_bxdf.h"
#include "iris/bxdfs/internal/math.h"
#include "iris/float.h"

namespace iris {
namespace bxdfs {
namespace {

using ::iris::bxdfs::internal::CatmullRomWeights;
using ::iris::bxdfs::internal::ComputeCatmullRomWeights;
using ::iris::bxdfs::internal::CosDPhi;
using ::iris::bxdfs::internal::CosTheta;

thread_local std::vector<std::pair<visual_t, visual_t>> gSampleCoefficients;

class FourierBxdf : public internal::DiffuseBxdf {
 public:
  FourierBxdf(const Reflector* reflectance, const Reflector* r,
              const Reflector* g, const Reflector* b,
              std::span<const geometric> elevational_samples,
              std::span<const visual> cdf,
              std::span<const std::pair<size_t, size_t>> coefficient_extents,
              std::span<const visual> y_coefficients,
              std::span<const visual> r_coefficients,
              std::span<const visual> b_coefficients, visual eta_transmitted,
              bool front_face)
      : reflectance_(reflectance),
        r_(r),
        g_(g),
        b_(b),
        elevational_samples_(elevational_samples),
        cdf_(cdf),
        coefficient_extents_(coefficient_extents),
        y_coefficients_(y_coefficients),
        r_coefficients_(r_coefficients),
        b_coefficients_(b_coefficients),
        eta_transmitted_(eta_transmitted),
        front_face_(front_face) {}

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
  Vector Incoming(const Vector& incoming) const {
    return front_face_ ? incoming : -incoming;
  }

  Vector Outgoing(const Vector& outgoing) const {
    return front_face_ ? -outgoing : outgoing;
  }

  const Reflector* reflectance_;
  const Reflector* r_;
  const Reflector* g_;
  const Reflector* b_;
  std::span<const geometric> elevational_samples_;
  std::span<const visual> cdf_;
  std::span<const std::pair<size_t, size_t>> coefficient_extents_;
  std::span<const visual> y_coefficients_;
  std::span<const visual> r_coefficients_;
  std::span<const visual> b_coefficients_;
  [[maybe_unused]] visual eta_transmitted_;
  bool front_face_;
};

std::optional<geometric_t> SampleCatmullRom2D(
    std::span<const geometric> elevational_samples,
    std::span<const std::pair<size_t, size_t>> coefficient_extents,
    std::span<const visual> all_coefficients, std::span<const visual> cdf,
    geometric mu_incoming, geometric_t sample) {
  std::optional<CatmullRomWeights> incoming_weights =
      ComputeCatmullRomWeights(elevational_samples, mu_incoming);
  if (!incoming_weights) {
    return std::nullopt;
  }

  auto interpolate_cdf = [&](size_t o_offset) {
    visual_t result = 0;
    for (size_t i_index = 0; i_index < incoming_weights->num_weights;
         i_index++) {
      result += cdf[incoming_weights->control_point_indices[i_index] *
                        elevational_samples.size() +
                    o_offset] *
                incoming_weights->control_point_weights[i_index];
    }
    return result;
  };

  sample *= interpolate_cdf(elevational_samples.size() - 1);

  auto find_interval = [&]() -> size_t {
    size_t first = 0;
    size_t len = elevational_samples.size();

    while (len > 0) {
      size_t half = len >> 1;
      size_t middle = first + half;

      if (interpolate_cdf(middle) < sample) {
        first = middle + 1;
        len -= half + 1;
      } else {
        len = half;
      }
    }

    if (first == 0) {
      return 0;
    }

    first -= 1;

    if (first > elevational_samples.size() - 2) {
      return elevational_samples.size() - 2;
    }

    return first;
  };

  size_t index = find_interval();

  auto interpolate_a0 = [&](size_t o_offset) {
    visual_t result = 0;
    for (size_t i_index = 0; i_index < incoming_weights->num_weights;
         i_index++) {
      auto [start, length] =
          coefficient_extents[incoming_weights->control_point_indices[i_index] *
                                  elevational_samples.size() +
                              o_offset];
      if (length != 0) {
        result += all_coefficients[start] *
                  incoming_weights->control_point_weights[i_index];
      }
    }
    return result;
  };

  visual_t f0 = interpolate_a0(index);
  visual_t f1 = interpolate_a0(index + 1);
  geometric x0 = elevational_samples[index];
  geometric x1 = elevational_samples[index + 1];
  geometric width = x1 - x0;

  sample = (sample - interpolate_cdf(index)) / width;

  visual_t d0;
  if (index > 0) {
    d0 = width * (f1 - interpolate_a0(index - 1)) /
         (x1 - elevational_samples[index - 1]);
  } else {
    d0 = f1 - f0;
  }

  visual_t d1;
  if (index + 2 < elevational_samples.size()) {
    d1 = width * (interpolate_a0(index + 2) - f0) /
         (elevational_samples[index + 2] - x0);
  } else {
    d1 = f1 - f0;
  }

  geometric_t t;
  if (f0 != f1) {
    t = (f0 - std::sqrt(std::max(static_cast<geometric_t>(0.0),
                                 f0 * f0 + static_cast<geometric_t>(2.0) *
                                               sample * (f1 - f0)))) /
        (f0 - f1);
  } else {
    t = sample / f0;
  }

  geometric_t a = static_cast<geometric_t>(0.0);
  geometric_t b = static_cast<geometric_t>(1.0);
  geometric_t Fhat;
  geometric_t fhat;
  for (;;) {
    if (!(t >= a && t <= b)) {
      t = static_cast<geometric_t>(0.5) * (a + b);
    }

    Fhat =
        t * (f0 + t * (static_cast<geometric_t>(0.5) * d0 +
                       t * (static_cast<geometric_t>(1.0 / 3.0) *
                                (static_cast<geometric_t>(-2.0) * d0 - d1) +
                            f1 - f0 +
                            t * (static_cast<geometric_t>(0.25) * (d0 + d1) +
                                 static_cast<geometric_t>(0.5) * (f0 - f1)))));
    fhat =
        f0 + t * (d0 + t * (static_cast<geometric_t>(-2.0) * d0 - d1 +
                            static_cast<geometric_t>(3.0) * (f1 - f0) +
                            t * (d0 + d1 +
                                 static_cast<geometric_t>(2.0) * (f0 - f1))));

    if (std::abs(Fhat - sample) < static_cast<geometric_t>(1e-6) ||
        b - a < static_cast<geometric_t>(1e-6)) {
      break;
    }

    if (Fhat - sample < static_cast<geometric_t>(0.0)) {
      a = t;
    } else {
      b = t;
    }

    t -= (Fhat - sample) / fhat;
  }

  return x0 + width * t;
}

geometric_t SamplePhi(
    std::span<const std::pair<visual_t, visual_t>> coefficients,
    geometric_t sample) {
  bool flip;
  if (sample >= static_cast<geometric_t>(0.5)) {
    sample = static_cast<geometric_t>(1.0) -
             static_cast<geometric_t>(2.0) *
                 (sample - static_cast<geometric_t>(0.5));
    flip = true;
  } else {
    sample *= static_cast<geometric_t>(2.0);
    flip = false;
  }

  double a = 0.0;
  double b = std::numbers::pi_v<double>;
  double phi = 0.5 * std::numbers::pi_v<double>;
  double target = sample * coefficients[0].first * std::numbers::pi_v<double>;
  for (;;) {
    double cosPhi = std::cos(phi);
    double sinPhi = std::sin(phi);
    double cosPhiPrev = cosPhi;
    double cosPhiCur = 1.0;
    double sinPhiPrev = -sinPhi;
    double sinPhiCur = 0.0;

    double F = coefficients[0].first * phi;
    double f = coefficients[0].first;
    for (size_t k = 1; k < coefficients.size(); k++) {
      double sinPhiNext = 2.0 * cosPhi * sinPhiCur - sinPhiPrev;
      double cosPhiNext = 2.0 * cosPhi * cosPhiCur - cosPhiPrev;
      sinPhiPrev = sinPhiCur;
      sinPhiCur = sinPhiNext;
      cosPhiPrev = cosPhiCur;
      cosPhiCur = cosPhiNext;

      F += coefficients[k].second * sinPhiNext;
      f += coefficients[k].first * cosPhiNext;
    }

    F -= target;

    if (F > 1e-6) {
      b = phi;
    } else if (F < -1e-6) {
      a = phi;
    } else {
      break;
    }

    if (b - a < 1e-6) {
      break;
    }

    phi -= F / f;
    if (a < phi && phi < b) {
      continue;
    }

    phi = 0.5 * (a + b);
  }

  if (flip) {
    phi = 2.0 * std::numbers::pi_v<double> - phi;
  }

  return static_cast<geometric_t>(phi);
}

visual_t ComputeChannel(
    const CatmullRomWeights& incoming_weights,
    const CatmullRomWeights& outgoing_weights, double cos_phi,
    std::span<const std::pair<size_t, size_t>> coefficient_extents,
    std::span<const visual> all_coefficients, size_t num_elevational_samples) {
  std::span<const visual_t> coefficients[4][4];
  double control_point_weights[4][4];
  size_t num_coefficients = 0;
  for (size_t i_index = 0; i_index < incoming_weights.num_weights; i_index++) {
    for (size_t o_index = 0; o_index < outgoing_weights.num_weights;
         o_index++) {
      auto [start_index, length] =
          coefficient_extents[incoming_weights.control_point_indices[i_index] *
                                  num_elevational_samples +
                              outgoing_weights.control_point_indices[o_index]];

      coefficients[i_index][o_index] =
          std::span(all_coefficients.begin() + start_index, length);
      num_coefficients =
          std::max(num_coefficients, static_cast<size_t>(length));

      control_point_weights[i_index][o_index] =
          incoming_weights.control_point_weights[i_index] *
          outgoing_weights.control_point_weights[o_index];
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
                   control_point_weights[i_index][o_index];
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
  Vector actual_incoming = Incoming(incoming);
  geometric mu_incoming = CosTheta(actual_incoming);

  std::optional<geometric_t> mu_outgoing =
      SampleCatmullRom2D(elevational_samples_, coefficient_extents_,
                         y_coefficients_, cdf_, mu_incoming, sampler.Next());
  if (!mu_outgoing) {
    return std::nullopt;
  }

  std::optional<CatmullRomWeights> incoming_weights =
      ComputeCatmullRomWeights(elevational_samples_, mu_incoming);
  if (!incoming_weights) {
    return std::nullopt;
  }

  std::optional<CatmullRomWeights> outgoing_weights =
      ComputeCatmullRomWeights(elevational_samples_, *mu_outgoing);
  if (!outgoing_weights) {
    return std::nullopt;
  }

  gSampleCoefficients.assign(1, {0.0, 0.0});
  for (size_t i_index = 0; i_index < incoming_weights->num_weights; i_index++) {
    for (size_t o_index = 0; o_index < outgoing_weights->num_weights;
         o_index++) {
      auto [start_index, length] = coefficient_extents_
          [incoming_weights->control_point_indices[i_index] *
               elevational_samples_.size() +
           outgoing_weights->control_point_indices[o_index]];
      if (gSampleCoefficients.size() < length) {
        gSampleCoefficients.resize(length);
      }

      visual_t weight = incoming_weights->control_point_weights[i_index] *
                        outgoing_weights->control_point_weights[o_index];

      for (size_t coeff = 0; coeff < length; coeff++) {
        gSampleCoefficients[coeff].first +=
            weight * y_coefficients_[start_index + coeff];
      }
    }
  }

  for (size_t coeff = 1; coeff < gSampleCoefficients.size(); coeff++) {
    gSampleCoefficients[coeff].second =
        gSampleCoefficients[coeff].first / static_cast<visual_t>(coeff);
  }

  geometric_t phi_outgoing = SamplePhi(gSampleCoefficients, sampler.Next());
  geometric_t sin_squared_theta_outgoing =
      std::max(static_cast<geometric_t>(0.0),
               static_cast<geometric_t>(1.0) - *mu_outgoing * *mu_outgoing);

  geometric_t norm = std::sqrt(sin_squared_theta_outgoing /
                               internal::SinSquaredTheta(actual_incoming));
  if (!std::isfinite(norm)) {
    return Vector(static_cast<geometric>(0.0), static_cast<geometric>(0.0),
                  static_cast<geometric>(1.0));
  }

  geometric_t sin_phi_outgoing = std::sin(phi_outgoing);
  geometric_t cos_phi_outgoing = std::cos(phi_outgoing);

  Vector outgoing =
      Normalize(Vector(norm * (cos_phi_outgoing * actual_incoming.x -
                               sin_phi_outgoing * actual_incoming.y),
                       norm * (sin_phi_outgoing * actual_incoming.x +
                               cos_phi_outgoing * actual_incoming.y),
                       *mu_outgoing));

  return front_face_ ? -outgoing : outgoing;
}

visual_t FourierBxdf::PdfDiffuse(const Vector& incoming, const Vector& outgoing,
                                 const Vector& surface_normal,
                                 Hemisphere hemisphere) const {
  Vector actual_incoming = Incoming(incoming);
  Vector actual_outgoing = Outgoing(outgoing);

  geometric mu_incoming = CosTheta(actual_incoming);
  geometric mu_outgoing = CosTheta(actual_outgoing);
  if (mu_incoming == static_cast<visual_t>(0.0) ||
      mu_outgoing == static_cast<visual_t>(0.0)) {
    return static_cast<visual_t>(0.0);
  }

  std::optional<CatmullRomWeights> incoming_weights =
      ComputeCatmullRomWeights(elevational_samples_, mu_incoming);
  if (!incoming_weights) {
    return static_cast<visual_t>(0.0);
  }

  std::optional<CatmullRomWeights> outgoing_weights =
      ComputeCatmullRomWeights(elevational_samples_, mu_outgoing);
  if (!outgoing_weights) {
    return static_cast<visual_t>(0.0);
  }

  geometric_t cos_phi = CosDPhi(actual_outgoing, actual_incoming);
  visual_t y = ComputeChannel(*incoming_weights, *outgoing_weights, cos_phi,
                              coefficient_extents_, y_coefficients_,
                              elevational_samples_.size());
  if (y <= static_cast<visual_t>(0.0)) {
    return static_cast<visual_t>(0.0);
  }

  visual_t rho = static_cast<visual_t>(0.0);
  for (size_t i_index = 0; i_index < incoming_weights->num_weights; i_index++) {
    size_t cdf_index = incoming_weights->control_point_indices[i_index] *
                           elevational_samples_.size() +
                       (elevational_samples_.size() - 1);

    rho += incoming_weights->control_point_weights[i_index] * cdf_[cdf_index] *
           static_cast<visual_t>(2.0 * std::numbers::pi_v<visual_t>);
  }

  return (rho > static_cast<visual_t>(0.0)) ? (y / rho)
                                            : static_cast<visual_t>(0.0);
}

const Reflector* FourierBxdf::ReflectanceDiffuse(
    const Vector& incoming, const Vector& outgoing, Hemisphere hemisphere,
    SpectralAllocator& allocator) const {
  Vector actual_incoming = Incoming(incoming);
  Vector actual_outgoing = Outgoing(outgoing);

  geometric mu_incoming = CosTheta(actual_incoming);
  geometric mu_outgoing = CosTheta(actual_outgoing);
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

  geometric_t cos_phi = CosDPhi(actual_outgoing, actual_incoming);
  visual_t y = ComputeChannel(*incoming_weights, *outgoing_weights, cos_phi,
                              coefficient_extents_, y_coefficients_,
                              elevational_samples_.size());

  const Reflector* result;
  if (r_coefficients_.empty() || b_coefficients_.empty()) {
    result = allocator.UnboundedScale(reflectance_,
                                      std::max(static_cast<visual_t>(0.0), y));
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

    result = allocator.UnboundedAdd(
        allocator.UnboundedScale(r_, std::max(static_cast<visual_t>(0.0), r)),
        allocator.UnboundedScale(g_, std::max(static_cast<visual_t>(0.0), g)),
        allocator.UnboundedScale(b_, std::max(static_cast<visual_t>(0.0), b)));
  }

  return allocator.UnboundedScale(
      result, static_cast<visual_t>(1.0) / std::abs(mu_outgoing));
}

}  // namespace

const Bxdf* MakeFourierBxdf(
    BxdfAllocator& bxdf_allocator, const Reflector* reflectance,
    std::span<const geometric> elevational_samples, std::span<const visual> cdf,
    std::span<const std::pair<size_t, size_t>> coefficient_extents,
    std::span<const visual> y_coefficients, visual eta_transmitted,
    bool front_face) {
  if (reflectance == nullptr) {
    return nullptr;
  }

  size_t num_samples = elevational_samples.size();
  if (num_samples < 3 || cdf.size() / num_samples != num_samples ||
      coefficient_extents.size() / num_samples != num_samples ||
      !std::isfinite(eta_transmitted) ||
      eta_transmitted < static_cast<visual_t>(1.0)) {
    return nullptr;
  }

  return &bxdf_allocator.Allocate<FourierBxdf>(
      reflectance, nullptr, nullptr, nullptr, elevational_samples, cdf,
      coefficient_extents, y_coefficients, std::span<const visual>(),
      std::span<const visual>(), eta_transmitted, front_face);
}

const Bxdf* MakeFourierBxdf(
    BxdfAllocator& bxdf_allocator, const Reflector* r, const Reflector* g,
    const Reflector* b, std::span<const geometric> elevational_samples,
    std::span<const visual> cdf,
    std::span<const std::pair<size_t, size_t>> coefficient_extents,
    std::span<const visual> y_coefficients,
    std::span<const visual> r_coefficients,
    std::span<const visual> b_coefficients, visual eta_transmitted,
    bool front_face) {
  if (r == nullptr && g == nullptr && b == nullptr) {
    return nullptr;
  }

  size_t num_samples = elevational_samples.size();
  if (num_samples < 3 || cdf.size() / num_samples != num_samples ||
      coefficient_extents.size() / num_samples != num_samples ||
      !std::isfinite(eta_transmitted) ||
      eta_transmitted < static_cast<visual_t>(1.0)) {
    return nullptr;
  }

  return &bxdf_allocator.Allocate<FourierBxdf>(
      nullptr, r, g, b, elevational_samples, cdf, coefficient_extents,
      y_coefficients, r_coefficients, b_coefficients, eta_transmitted,
      front_face);
}

}  // namespace bxdfs
}  // namespace iris
