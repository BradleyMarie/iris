#include "iris/spectral_allocator.h"

#include <algorithm>
#include <cassert>

#include "iris/internal/arena.h"

namespace iris {
namespace {

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

}  // namespace iris