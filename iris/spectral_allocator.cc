#include "iris/spectral_allocator.h"

#include <algorithm>
#include <cassert>
#include <cmath>

#include "iris/internal/arena.h"

namespace iris {
namespace {

class ZeroSpectrum final : public Spectrum {
 public:
  visual_t Intensity(visual_t wavelength) const override {
    return static_cast<visual_t>(0.0);
  }
};

class SumSpectrum final : public Spectrum {
 public:
  SumSpectrum(const Spectrum& addend0, const Spectrum& addend1)
      : addend0_(addend0), addend1_(addend1) {}

  visual_t Intensity(visual_t wavelength) const override {
    return addend0_.Intensity(wavelength) + addend1_.Intensity(wavelength);
  }

 private:
  const Spectrum& addend0_;
  const Spectrum& addend1_;
};

class ScaledSpectrum final : public Spectrum {
 public:
  ScaledSpectrum(const Spectrum& spectrum, visual_t scalar)
      : spectrum_(spectrum), scalar_(scalar) {
    assert(scalar > 0.0);
  }

  visual_t Intensity(visual_t wavelength) const override {
    return spectrum_.Intensity(wavelength) * scalar_;
  }

 private:
  const Spectrum& spectrum_;
  const visual_t scalar_;
};

class ScaledSpectra final : public Spectrum {
 public:
  ScaledSpectra(const Spectrum& spectrum0, const Spectrum& spectrum1)
      : spectrum0_(spectrum0), spectrum1_(spectrum1) {}

  visual_t Intensity(visual_t wavelength) const override {
    return spectrum0_.Intensity(wavelength) * spectrum1_.Intensity(wavelength);
  }

 private:
  const Spectrum& spectrum0_;
  const Spectrum& spectrum1_;
};

class ReflectedSpectrum final : public Spectrum {
 public:
  ReflectedSpectrum(const Spectrum& spectrum, const Reflector& reflector)
      : spectrum_(spectrum), reflector_(reflector) {}

  visual_t Intensity(visual_t wavelength) const override {
    return spectrum_.Intensity(wavelength) * reflector_.Reflectance(wavelength);
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
    visual_t sum =
        addend0_.Reflectance(wavelength) + addend1_.Reflectance(wavelength);
    return std::min(static_cast<visual_t>(1.0), sum);
  }

 private:
  const Reflector& addend0_;
  const Reflector& addend1_;
};

class ScaledReflector final : public Reflector {
 public:
  ScaledReflector(const Reflector& reflector, visual_t scalar)
      : reflector_(reflector), scalar_(scalar) {
    assert(scalar > 0.0 && scalar < 1.0);
  }

  visual_t Reflectance(visual_t wavelength) const override {
    return reflector_.Reflectance(wavelength) * scalar_;
  }

 private:
  const Reflector& reflector_;
  const visual_t scalar_;
};

class ScaledReflectors final : public Reflector {
 public:
  ScaledReflectors(const Reflector& reflector, const Reflector& attenuation)
      : reflector_(reflector), attenuation_(attenuation) {}

  visual_t Reflectance(visual_t wavelength) const override {
    return reflector_.Reflectance(wavelength) *
           attenuation_.Reflectance(wavelength);
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
    visual_t inverted =
        static_cast<visual_t>(1.0) - reflector_.Reflectance(wavelength);
    return std::max(static_cast<visual_t>(0.0), inverted);
  }

 private:
  const Reflector& reflector_;
};

class UnboundedScaledReflector final : public Reflector {
 public:
  UnboundedScaledReflector(const Reflector& reflector, visual_t scalar)
      : reflector_(reflector), scalar_(scalar) {
    assert(scalar > 0.0);
  }

  visual_t Reflectance(visual_t wavelength) const override {
    return reflector_.Reflectance(wavelength) * scalar_;
  }

 private:
  const Reflector& reflector_;
  const visual_t scalar_;
};

class UnboundedSumReflector final : public Reflector {
 public:
  UnboundedSumReflector(const Reflector& addend0, const Reflector& addend1)
      : addend0_(addend0), addend1_(addend1) {}

  visual_t Reflectance(visual_t wavelength) const override {
    visual_t sum =
        addend0_.Reflectance(wavelength) + addend1_.Reflectance(wavelength);
    return sum;
  }

 private:
  const Reflector& addend0_;
  const Reflector& addend1_;
};

class FresnelConductorReflector final : public Reflector {
 public:
  FresnelConductorReflector(visual_t cos_theta_incident,
                            const Spectrum& eta_incident,
                            const Spectrum& eta_transmitted,
                            const Spectrum& k_transmitted)
      : cos_theta_incident_(cos_theta_incident),
        eta_incident_(eta_incident),
        eta_transmitted_(eta_transmitted),
        k_transmitted_(k_transmitted) {}

  visual_t Reflectance(visual_t wavelength) const override {
    visual_t eta_incident = std::max(static_cast<visual_t>(1.0),
                                     eta_incident_.Intensity(wavelength));
    visual_t eta_transmitted = eta_transmitted_.Intensity(wavelength);
    visual_t k_transmitted = k_transmitted_.Intensity(wavelength);

    visual_t cos_theta_incident_squared =
        cos_theta_incident_ * cos_theta_incident_;
    visual_t sin_theta_incident_squared =
        static_cast<visual_t>(1.0) - cos_theta_incident_squared;

    visual_t eta = eta_transmitted / eta_incident;
    visual_t eta_squared = eta * eta;

    visual_t eta_k = k_transmitted / eta_incident;
    visual_t eta_k_squared = eta_k * eta_k;

    visual_t t0 = eta_squared - eta_k_squared - sin_theta_incident_squared;
    visual_t a_squared_plus_b_squared = std::sqrt(
        t0 * t0 + static_cast<visual_t>(4.0) * eta_squared * eta_k_squared);
    visual_t t1 = a_squared_plus_b_squared + cos_theta_incident_squared;
    visual_t a =
        std::sqrt(static_cast<visual_t>(0.5) * (a_squared_plus_b_squared + t0));
    visual_t t2 = static_cast<visual_t>(2.0) * cos_theta_incident_ * a;
    visual_t reflectance_s = (t1 - t2) / (t1 + t2);

    visual_t t3 = cos_theta_incident_squared * a_squared_plus_b_squared +
                  sin_theta_incident_squared * sin_theta_incident_squared;
    visual_t t4 = t2 * sin_theta_incident_squared;
    visual_t reflectance_p = reflectance_s * (t3 - t4) / (t3 + t4);

    return static_cast<visual_t>(0.5) * (reflectance_p + reflectance_s);
  }

 private:
  const visual_t cos_theta_incident_;
  const Spectrum& eta_incident_;
  const Spectrum& eta_transmitted_;
  const Spectrum& k_transmitted_;
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

const Spectrum* SpectralAllocator::Scale(const Spectrum* spectrum0,
                                         const Spectrum* spectrum1) {
  if (!spectrum0 || !spectrum1) {
    return nullptr;
  }

  return &arena_.Allocate<ScaledSpectra>(*spectrum0, *spectrum1);
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

const Reflector* SpectralAllocator::Scale(const Reflector* reflector,
                                          visual_t attenuation) {
  assert(attenuation >= 0.0 && attenuation <= 1.0);

  if (!reflector || attenuation <= 0.0) {
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

const Reflector* SpectralAllocator::UnboundedScale(const Reflector* reflector,
                                                   visual_t attenuation) {
  assert(attenuation >= 0.0);

  if (!reflector || attenuation <= 0.0) {
    return nullptr;
  }

  if (attenuation != 1.0) {
    return &arena_.Allocate<UnboundedScaledReflector>(*reflector, attenuation);
  }

  return reflector;
}

const Reflector* SpectralAllocator::FresnelConductor(
    visual_t cos_theta_incident, const Spectrum* eta_incident,
    const Spectrum* eta_transmitted, const Spectrum* k_transmitted) {
  static const ZeroSpectrum zero_spectrum;

  if (cos_theta_incident == static_cast<visual_t>(0.0)) {
    return nullptr;
  }

  cos_theta_incident =
      std::min(static_cast<visual_t>(1.0), std::abs(cos_theta_incident));

  if (!eta_incident) {
    eta_incident = &zero_spectrum;
  }

  if (!eta_transmitted) {
    eta_transmitted = &zero_spectrum;
  }

  if (!k_transmitted) {
    k_transmitted = &zero_spectrum;
  }

  return &arena_.Allocate<FresnelConductorReflector>(
      cos_theta_incident, *eta_incident, *eta_transmitted, *k_transmitted);
}

}  // namespace iris