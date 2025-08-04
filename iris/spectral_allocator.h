#ifndef _IRIS_SPECTRAL_ALLOCATOR_
#define _IRIS_SPECTRAL_ALLOCATOR_

#include "iris/float.h"
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
  const Spectrum* Reflect(const Spectrum* spectrum, const Reflector* reflector);

  const Reflector* Add(const Reflector* addend0, const Reflector* addend1);
  const Reflector* Add(const Reflector* addend0, const Reflector* addend1,
                       const Reflector* addend2);
  const Reflector* Scale(const Reflector* reflector, visual_t attenuation);
  const Reflector* Scale(const Reflector* reflector,
                         const Reflector* attenuation);
  const Reflector* Invert(const Reflector* reflector);

  // Prefer the bounded version of these if possible.
  const Reflector* UnboundedAdd(const Reflector* addend0,
                                const Reflector* addend1);
  const Reflector* UnboundedAdd(const Reflector* addend0,
                                const Reflector* addend1,
                                const Reflector* addend2);
  const Reflector* UnboundedScale(const Reflector* reflector,
                                  visual_t attenuation);
  const Reflector* UnboundedScale(const Reflector* reflector,
                                  const Reflector* attenuation);

  // Optmization for Fresnel dielectric-conductor interfaces.
  const Reflector* FresnelConductor(visual_t eta_dielectric,
                                    const Spectrum* eta_conductor,
                                    const Spectrum* k_conductor,
                                    visual_t cos_theta_incident);

 private:
  SpectralAllocator(const SpectralAllocator&) = delete;
  SpectralAllocator& operator=(const SpectralAllocator&) = delete;

  internal::Arena& arena_;
};

}  // namespace iris

#endif  // _IRIS_HIT_ALLOCATOR_
