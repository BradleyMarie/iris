#include "iris/spectral_allocator.h"

#include <algorithm>
#include <cassert>
#include <cmath>

#include "iris/float.h"
#include "iris/internal/arena.h"
#include "iris/reflector.h"
#include "iris/spectrum.h"

namespace iris {
namespace {

class SumSpectrum final : public Spectrum {
 public:
  SumSpectrum(const Spectrum& addend0, const Spectrum& addend1)
      : addend0_(addend0), addend1_(addend1) {}

  visual_t Intensity(visual_t wavelength) const override {
    visual_t addend0 = addend0_.Intensity(wavelength);
    assert(std::isfinite(addend0) && addend0 >= static_cast<visual_t>(0.0));

    visual_t addend1 = addend1_.Intensity(wavelength);
    assert(std::isfinite(addend1) && addend1 >= static_cast<visual_t>(0.0));

    return addend0 + addend1;
  }

 private:
  const Spectrum& addend0_;
  const Spectrum& addend1_;
};

class ScaledSpectrum final : public Spectrum {
 public:
  ScaledSpectrum(const Spectrum& spectrum, visual_t scalar)
      : spectrum_(spectrum), scalar_(scalar) {
    assert(std::isfinite(scalar) && scalar >= 0.0);
  }

  visual_t Intensity(visual_t wavelength) const override {
    visual_t intensity = spectrum_.Intensity(wavelength);
    assert(std::isfinite(intensity) && intensity >= static_cast<visual_t>(0.0));

    return intensity * scalar_;
  }

 private:
  const Spectrum& spectrum_;
  visual_t scalar_;
};

class ReflectedSpectrum final : public Spectrum {
 public:
  ReflectedSpectrum(const Spectrum& spectrum, const Reflector& reflector)
      : spectrum_(spectrum), reflector_(reflector) {}

  visual_t Intensity(visual_t wavelength) const override {
    visual_t intensity = spectrum_.Intensity(wavelength);
    assert(std::isfinite(intensity) && intensity >= static_cast<visual_t>(0.0));

    visual_t reflectance = reflector_.Reflectance(wavelength);
    assert(std::isfinite(reflectance) &&
           reflectance >= static_cast<visual_t>(0.0));

    return intensity * reflectance;
  }

 private:
  const Spectrum& spectrum_;
  const Reflector& reflector_;
};

class SumReflector final : public Reflector {
 public:
  SumReflector(const Reflector& addend0, const Reflector& addend1)
      : addend0_(addend0), addend1_(addend1) {}

  visual_t Reflectance(visual_t wavelength) const override {
    visual_t reflectance0 = addend0_.Reflectance(wavelength);
    assert(std::isfinite(reflectance0) &&
           reflectance0 >= static_cast<visual_t>(0.0));

    visual_t reflectance1 = addend1_.Reflectance(wavelength);
    assert(std::isfinite(reflectance1) &&
           reflectance1 >= static_cast<visual_t>(0.0));

    return std::min(static_cast<visual_t>(1.0), reflectance0 + reflectance1);
  }

 private:
  const Reflector& addend0_;
  const Reflector& addend1_;
};

class ScaledReflector final : public Reflector {
 public:
  ScaledReflector(const Reflector& reflector, visual_t scalar)
      : reflector_(reflector), scalar_(scalar) {
    assert(std::isfinite(scalar) && scalar >= 0.0);
  }

  visual_t Reflectance(visual_t wavelength) const override {
    visual_t reflectance = reflector_.Reflectance(wavelength);
    assert(std::isfinite(reflectance) &&
           reflectance >= static_cast<visual_t>(0.0));

    return reflectance * scalar_;
  }

 private:
  const Reflector& reflector_;
  visual_t scalar_;
};

class ScaledReflectors final : public Reflector {
 public:
  ScaledReflectors(const Reflector& reflector, const Reflector& attenuation)
      : reflector_(reflector), attenuation_(attenuation) {}

  visual_t Reflectance(visual_t wavelength) const override {
    visual_t reflectance = reflector_.Reflectance(wavelength);
    assert(std::isfinite(reflectance) &&
           reflectance >= static_cast<visual_t>(0.0));

    visual_t attenuation = attenuation_.Reflectance(wavelength);
    assert(std::isfinite(attenuation) &&
           attenuation >= static_cast<visual_t>(0.0));

    return reflectance * attenuation;
  }

 private:
  const Reflector& reflector_;
  const Reflector& attenuation_;
};

class PerfectReflector final : public Reflector {
 public:
  visual_t Reflectance(visual_t wavelength) const override {
    return static_cast<visual_t>(1.0);
  }
};

class InvertedReflector final : public Reflector {
 public:
  InvertedReflector(const Reflector& reflector) : reflector_(reflector) {}

  visual_t Reflectance(visual_t wavelength) const override {
    visual_t reflectance = reflector_.Reflectance(wavelength);
    assert(std::isfinite(reflectance) &&
           reflectance >= static_cast<visual_t>(0.0));

    return std::max(static_cast<visual_t>(0.0),
                    static_cast<visual_t>(1.0) - reflectance);
  }

 private:
  const Reflector& reflector_;
};

class UnboundedScaledReflector final : public Reflector {
 public:
  UnboundedScaledReflector(const Reflector& reflector, visual_t scalar)
      : reflector_(reflector), scalar_(scalar) {
    assert(std::isfinite(scalar) && scalar >= 0.0);
  }

  visual_t Reflectance(visual_t wavelength) const override {
    visual_t reflectance = reflector_.Reflectance(wavelength);
    assert(std::isfinite(reflectance) &&
           reflectance >= static_cast<visual_t>(0.0));

    return reflectance * scalar_;
  }

 private:
  const Reflector& reflector_;
  visual_t scalar_;
};

class UnboundedScaledReflectors final : public Reflector {
 public:
  UnboundedScaledReflectors(const Reflector& reflector,
                            const Reflector& attenuation)
      : reflector_(reflector), attenuation_(attenuation) {}

  visual_t Reflectance(visual_t wavelength) const override {
    visual_t reflectance = reflector_.Reflectance(wavelength);
    assert(std::isfinite(reflectance) &&
           reflectance >= static_cast<visual_t>(0.0));

    visual_t attenuation = attenuation_.Reflectance(wavelength);
    assert(std::isfinite(attenuation) &&
           attenuation >= static_cast<visual_t>(0.0));

    return reflectance * attenuation;
  }

 private:
  const Reflector& reflector_;
  const Reflector& attenuation_;
};

class UnboundedSumReflector final : public Reflector {
 public:
  UnboundedSumReflector(const Reflector& addend0, const Reflector& addend1)
      : addend0_(addend0), addend1_(addend1) {}

  visual_t Reflectance(visual_t wavelength) const override {
    visual_t addend0 = addend0_.Reflectance(wavelength);
    assert(std::isfinite(addend0) && addend0 >= static_cast<visual_t>(0.0));

    visual_t addend1 = addend1_.Reflectance(wavelength);
    assert(std::isfinite(addend1) && addend1 >= static_cast<visual_t>(0.0));

    return addend0 + addend1;
  }

 private:
  const Reflector& addend0_;
  const Reflector& addend1_;
};

class FresnelConductorReflector final : public Reflector {
 public:
  FresnelConductorReflector(visual_t eta_dielectric,
                            const Spectrum& eta_conductor,
                            const Spectrum* k_conductor,
                            visual_t cos_theta_dielectric)
      : eta_conductor_(eta_conductor),
        k_conductor_(k_conductor),
        eta_dielectric_(eta_dielectric),
        cos_theta_dielectric_(std::min(static_cast<visual_t>(1.0),
                                       std::abs(cos_theta_dielectric))) {
    assert(std::isfinite(eta_dielectric) &&
           eta_dielectric >= static_cast<visual_t>(1.0));
    assert(cos_theta_dielectric >= static_cast<visual_t>(-1.0) &&
           cos_theta_dielectric <= static_cast<visual_t>(1.0));
  }

  visual_t Reflectance(visual_t wavelength) const override {
    visual_t eta_conductor = eta_conductor_.Intensity(wavelength);
    assert(std::isfinite(eta_conductor) &&
           eta_conductor > static_cast<visual_t>(0.0));

    if (!std::isfinite(eta_conductor) ||
        eta_conductor <= static_cast<visual_t>(0.0)) {
      return static_cast<visual_t>(0.0);
    }

    visual_t k_conductor = static_cast<visual_t>(0.0);
    if (k_conductor_) {
      visual_t raw_k = k_conductor_->Intensity(wavelength);
      assert(std::isfinite(raw_k) && raw_k > static_cast<visual_t>(0.0));

      if (std::isfinite(raw_k) && raw_k > static_cast<visual_t>(0.0)) {
        k_conductor = raw_k;
      }
    }

    visual_t cos_theta_dielectric_squared =
        cos_theta_dielectric_ * cos_theta_dielectric_;
    visual_t sin_theta_dielectric_squared =
        static_cast<visual_t>(1.0) - cos_theta_dielectric_squared;

    visual_t eta = eta_conductor / eta_dielectric_;
    visual_t eta_squared = eta * eta;

    visual_t eta_k = k_conductor / eta_dielectric_;
    visual_t eta_k_squared = eta_k * eta_k;

    visual_t t0 = eta_squared - eta_k_squared - sin_theta_dielectric_squared;
    visual_t a_squared_plus_b_squared = std::sqrt(
        t0 * t0 + static_cast<visual_t>(4.0) * eta_squared * eta_k_squared);
    visual_t t1 = a_squared_plus_b_squared + cos_theta_dielectric_squared;
    visual_t a =
        std::sqrt(static_cast<visual_t>(0.5) * (a_squared_plus_b_squared + t0));
    visual_t t2 = static_cast<visual_t>(2.0) * cos_theta_dielectric_ * a;
    visual_t reflectance_s = (t1 - t2) / (t1 + t2);

    visual_t t3 = cos_theta_dielectric_squared * a_squared_plus_b_squared +
                  sin_theta_dielectric_squared * sin_theta_dielectric_squared;
    visual_t t4 = t2 * sin_theta_dielectric_squared;
    visual_t reflectance_p = reflectance_s * (t3 - t4) / (t3 + t4);

    return std::clamp(
        static_cast<visual_t>(0.5) * (reflectance_p + reflectance_s),
        static_cast<visual_t>(0.0), static_cast<visual_t>(1.0));
  }

 private:
  const Spectrum& eta_conductor_;
  const Spectrum* k_conductor_;
  visual_t eta_dielectric_;
  visual_t cos_theta_dielectric_;
};

}  // namespace

const Spectrum* SpectralAllocator::Add(const Spectrum* addend0,
                                       const Spectrum* addend1) {
  if (!addend0) {
    return addend1;
  }

  if (!addend1) {
    return addend0;
  }

  return &arena_.Allocate<SumSpectrum>(*addend0, *addend1);
}

const Spectrum* SpectralAllocator::Scale(const Spectrum* spectrum,
                                         visual_t attenuation) {
  assert(std::isfinite(attenuation) && attenuation >= 0.0);

  if (!spectrum) {
    return nullptr;
  }

  if (attenuation == 1.0) {
    return spectrum;
  }

  if (attenuation > 0.0) {
    return &arena_.Allocate<ScaledSpectrum>(*spectrum, attenuation);
  }

  return nullptr;
}

const Spectrum* SpectralAllocator::Reflect(const Spectrum* spectrum,
                                           const Reflector* reflector) {
  if (!spectrum || !reflector) {
    return nullptr;
  }

  return &arena_.Allocate<ReflectedSpectrum>(*spectrum, *reflector);
}

const Reflector* SpectralAllocator::Add(const Reflector* addend0,
                                        const Reflector* addend1) {
  if (!addend0) {
    return addend1;
  }

  if (!addend1) {
    return addend0;
  }

  return &arena_.Allocate<SumReflector>(*addend0, *addend1);
}

const Reflector* SpectralAllocator::Add(const Reflector* addend0,
                                        const Reflector* addend1,
                                        const Reflector* addend2) {
  return Add(Add(addend0, addend1), addend2);
}

const Reflector* SpectralAllocator::Scale(const Reflector* reflector,
                                          visual_t attenuation) {
  assert(std::isfinite(attenuation) && attenuation >= 0.0 &&
         attenuation <= 1.0);

  if (!reflector || !std::isfinite(attenuation) || attenuation <= 0.0) {
    return nullptr;
  }

  if (attenuation < 1.0) {
    return &arena_.Allocate<ScaledReflector>(*reflector, attenuation);
  }

  return reflector;
}

const Reflector* SpectralAllocator::Scale(const Reflector* reflector,
                                          const Reflector* attenuation) {
  if (!reflector || !attenuation) {
    return nullptr;
  }

  return &arena_.Allocate<ScaledReflectors>(*reflector, *attenuation);
}

const Reflector* SpectralAllocator::Invert(const Reflector* reflector) {
  static const PerfectReflector perfect_reflector;

  if (!reflector) {
    return &perfect_reflector;
  }

  if (reflector == &perfect_reflector) {
    return nullptr;
  }

  return &arena_.Allocate<InvertedReflector>(*reflector);
}

const Reflector* SpectralAllocator::UnboundedAdd(const Reflector* addend0,
                                                 const Reflector* addend1) {
  if (!addend0) {
    return addend1;
  }

  if (!addend1) {
    return addend0;
  }

  return &arena_.Allocate<UnboundedSumReflector>(*addend0, *addend1);
}

const Reflector* SpectralAllocator::UnboundedAdd(const Reflector* addend0,
                                                 const Reflector* addend1,
                                                 const Reflector* addend2) {
  return UnboundedAdd(UnboundedAdd(addend0, addend1), addend2);
}

const Reflector* SpectralAllocator::UnboundedScale(const Reflector* reflector,
                                                   visual_t attenuation) {
  assert(std::isfinite(attenuation) && attenuation >= 0.0);

  if (!reflector || !std::isfinite(attenuation) || attenuation <= 0.0) {
    return nullptr;
  }

  if (attenuation != 1.0) {
    return &arena_.Allocate<UnboundedScaledReflector>(*reflector, attenuation);
  }

  return reflector;
}

const Reflector* SpectralAllocator::UnboundedScale(
    const Reflector* reflector, const Reflector* attenuation) {
  if (!reflector || !attenuation) {
    return nullptr;
  }

  return &arena_.Allocate<UnboundedScaledReflectors>(*reflector, *attenuation);
}

const Reflector* SpectralAllocator::FresnelConductor(
    visual_t eta_dielectric, const Spectrum* eta_conductor,
    const Spectrum* k_conductor, visual_t cos_theta_dielectric) {
  assert(std::isfinite(eta_dielectric) &&
         eta_dielectric >= static_cast<visual_t>(1.0));
  assert(cos_theta_dielectric >= static_cast<visual_t>(-1.0) &&
         cos_theta_dielectric <= static_cast<visual_t>(1.0));

  if (!std::isfinite(eta_dielectric) ||
      eta_dielectric < static_cast<visual_t>(1.0)) {
    return nullptr;
  }

  if (!eta_conductor) {
    return nullptr;
  }

  return &arena_.Allocate<FresnelConductorReflector>(
      eta_dielectric, *eta_conductor, k_conductor, cos_theta_dielectric);
}

}  // namespace iris
