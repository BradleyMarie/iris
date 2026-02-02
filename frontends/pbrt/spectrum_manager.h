#ifndef _FRONTENDS_PBRT_SPECTRUM_MANAGER_
#define _FRONTENDS_PBRT_SPECTRUM_MANAGER_

#include <map>

#include "iris/reference_counted.h"
#include "iris/reflector.h"
#include "iris/spectrum.h"
#include "pbrt_proto/pbrt.pb.h"

namespace iris::pbrt_frontend {

class SpectrumManager {
 public:
  virtual visual_t ComputeLuma(visual_t r, visual_t g, visual_t b) = 0;

  virtual ReferenceCounted<Spectrum> AllocateSpectrum(
      const pbrt_proto::Spectrum& spectrum, visual_t* luma = nullptr) = 0;

  virtual ReferenceCounted<Reflector> AllocateReflector(
      const pbrt_proto::Spectrum& spectrum) = 0;

  // Returns spectrum0 scaled by spectrum1. Both spectra are are guaranteed to
  // have been allocated by *this* allocator; however, there is no guarantee
  // that they will outlive the returned spectrum.
  virtual ReferenceCounted<Spectrum> AllocateSpectrum(
      const ReferenceCounted<Spectrum>& spectrum0,
      const ReferenceCounted<Spectrum>& spectrum1,
      visual_t* luma = nullptr) = 0;

  ReferenceCounted<Reflector> AllocateReflector(visual_t constant) {
    pbrt_proto::Spectrum spectrum;
    spectrum.set_constant_spectrum(constant);
    return AllocateReflector(spectrum);
  }

  ReferenceCounted<Reflector> AllocateReflector(visual_t r, visual_t g,
                                                visual_t b) {
    pbrt_proto::Spectrum spectrum;
    spectrum.mutable_rgb_spectrum()->set_r(r);
    spectrum.mutable_rgb_spectrum()->set_g(g);
    spectrum.mutable_rgb_spectrum()->set_b(b);
    return AllocateReflector(spectrum);
  }

  ReferenceCounted<Spectrum> AllocateSpectrum(visual_t r, visual_t g,
                                              visual_t b) {
    pbrt_proto::Spectrum spectrum;
    spectrum.mutable_rgb_spectrum()->set_r(r);
    spectrum.mutable_rgb_spectrum()->set_g(g);
    spectrum.mutable_rgb_spectrum()->set_b(b);
    return AllocateSpectrum(spectrum);
  }

  ReferenceCounted<Spectrum> AllocateSpectrum(
      const std::map<visual, visual>& wavelengths) {
    pbrt_proto::Spectrum spectrum;
    for (const auto& [wavelength, intensity] : wavelengths) {
      auto& sample = *spectrum.mutable_sampled_spectrum()->add_samples();
      sample.set_wavelength(wavelength);
      sample.set_intensity(wavelength);
    }
    return AllocateSpectrum(spectrum);
  }
};

}  // namespace iris::pbrt_frontend

#endif  // _FRONTENDS_PBRT_SPECTRUM_MANAGER_
