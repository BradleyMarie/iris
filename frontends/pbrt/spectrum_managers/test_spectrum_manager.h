#ifndef _FRONTENDS_PBRT_SPECTRUM_MANAGERS_TEST_SPECTRUM_MANAGER_
#define _FRONTENDS_PBRT_SPECTRUM_MANAGERS_TEST_SPECTRUM_MANAGER_

#include <map>

#include "frontends/pbrt/spectrum_manager.h"
#include "iris/reference_counted.h"
#include "iris/reflector.h"
#include "iris/spectrum.h"
#include "pbrt_proto/v3/v3.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace spectrum_managers {

class TestSpectrumManager final : public SpectrumManager {
 public:
  visual_t ComputeLuma(visual_t r, visual_t g, visual_t b) override;

  ReferenceCounted<Spectrum> AllocateSpectrum(
      const pbrt_proto::v3::Spectrum& spectrum, visual_t* luma) override;

  ReferenceCounted<Reflector> AllocateReflector(
      const pbrt_proto::v3::Spectrum& spectrum) override;

  ReferenceCounted<Spectrum> AllocateSpectrum(
      const ReferenceCounted<Spectrum>& spectrum0,
      const ReferenceCounted<Spectrum>& spectrum1, visual_t* luma) override;
};

}  // namespace spectrum_managers
}  // namespace pbrt_frontend
}  // namespace iris

#endif  // _FRONTENDS_PBRT_SPECTRUM_MANAGERS_TEST_SPECTRUM_MANAGER_
