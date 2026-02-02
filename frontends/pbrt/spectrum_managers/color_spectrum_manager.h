#ifndef _FRONTENDS_PBRT_SPECTRUM_MANAGERS_COLOR_SPECTRUM_MANAGER_
#define _FRONTENDS_PBRT_SPECTRUM_MANAGERS_COLOR_SPECTRUM_MANAGER_

#include <array>

#include "frontends/pbrt/spectrum_manager.h"
#include "iris/albedo_matcher.h"
#include "iris/color.h"
#include "iris/color_matcher.h"
#include "iris/power_matcher.h"
#include "iris/reference_counted.h"
#include "iris/reflector.h"
#include "iris/spectrum.h"
#include "pbrt_proto/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace spectrum_managers {

class ColorColorMatcher final : public ColorMatcher {
 public:
  std::array<visual_t, 3> Match(const Spectrum& spectrum) const override;
  Color::Space ColorSpace() const override;
};

class ColorAlbedoMatcher : public AlbedoMatcher {
 public:
  visual_t Match(const Reflector& reflector) const override;
};

class ColorPowerMatcher : public PowerMatcher {
 public:
  visual_t Match(const Spectrum& spectrum) const override;
};

class ColorSpectrumManager final : public SpectrumManager {
 public:
  ColorSpectrumManager(const std::filesystem::path& search_root,
                       bool all_spectra_are_reflective)
      : search_root_(search_root),
        all_sampled_spectra_are_reflective_(all_spectra_are_reflective) {}

  visual_t ComputeLuma(visual_t r, visual_t g, visual_t b) override;

  ReferenceCounted<Spectrum> AllocateSpectrum(
      const pbrt_proto::Spectrum& spectrum, visual_t* luma = nullptr) override;

  ReferenceCounted<Reflector> AllocateReflector(
      const pbrt_proto::Spectrum& spectrum) override;

  ReferenceCounted<Spectrum> AllocateSpectrum(
      const ReferenceCounted<Spectrum>& spectrum0,
      const ReferenceCounted<Spectrum>& spectrum1,
      visual_t* luma = nullptr) override;

 private:
  const std::filesystem::path search_root_;
  bool all_sampled_spectra_are_reflective_;
};

}  // namespace spectrum_managers
}  // namespace pbrt_frontend
}  // namespace iris

#endif  // _FRONTENDS_PBRT_SPECTRUM_MANAGERS_COLOR_SPECTRUM_MANAGER_
