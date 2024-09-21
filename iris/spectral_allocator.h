#ifndef _IRIS_SPECTRAL_ALLOCATOR_
#define _IRIS_SPECTRAL_ALLOCATOR_

#include "iris/reflector.h"
#include "iris/spectrum.h"

namespace iris {
namespace internal {
class Arena;
}  // namespace internal

class SpectralAllocator {
 public:
  SpectralAllocator(internal::Arena& arena) noexcept : arena_(arena) {}

  const Spectrum* Add(const Spectrum* addend0, const Spectrum* addend1);
  const Spectrum* Scale(const Spectrum* spectrum, visual_t attenuation);
  const Spectrum* Scale(const Spectrum* spectrum0, const Spectrum* spectrum1);
  const Spectrum* Reflect(const Spectrum* spectrum, const Reflector* reflector);

  const Reflector* Add(const Reflector* addend0, const Reflector* addend1);
  const Reflector* Scale(const Reflector* reflector, visual_t attenuation);
  const Reflector* Scale(const Reflector* reflector,
                         const Reflector* attenuation);
  const Reflector* Invert(const Reflector* reflector);

  // Prefer the bounded version of these if possible.
  const Reflector* UnboundedAdd(const Reflector* addend0,
                                const Reflector* addend1);
  const Reflector* UnboundedScale(const Reflector* reflector,
                                  visual_t attenuation);

  // Special case for Fresnel conductors interfaces as an optimization.
  // Passing `nullptr` for `eta_incident` or `eta_transmitted` as well as
  // Intensity values returned less than 1.0 will be be clamped to 1.0.
  const Reflector* FresnelConductor(visual_t cos_theta_incident,
                                    const Spectrum* eta_incident,
                                    const Spectrum* eta_transmitted,
                                    const Spectrum* k);

 private:
  SpectralAllocator(const SpectralAllocator&) = delete;
  SpectralAllocator& operator=(const SpectralAllocator&) = delete;

  internal::Arena& arena_;
};

}  // namespace iris

#endif  // _IRIS_HIT_ALLOCATOR_