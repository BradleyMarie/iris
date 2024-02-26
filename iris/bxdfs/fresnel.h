#ifndef _IRIS_BXDFS_FRESNEL_
#define _IRIS_BXDFS_FRESNEL_

#include "iris/float.h"
#include "iris/spectral_allocator.h"

namespace iris {
namespace bxdfs {

class Fresnel {
 public:
  virtual const Reflector* Evaluate(visual_t cos_theta_incident,
                                    SpectralAllocator& allocator) const = 0;
  virtual const Reflector* Complement(visual_t cos_theta_incident,
                                      SpectralAllocator& allocator) const = 0;
};

class FresnelDielectric : public Fresnel {
 public:
  FresnelDielectric(visual_t eta_incident, visual_t eta_transmitted)
      : eta_incident_(eta_incident), eta_transmitted_(eta_transmitted) {}

  const Reflector* Evaluate(visual_t cos_theta_incident,
                            SpectralAllocator& allocator) const override;
  const Reflector* Complement(visual_t cos_theta_incident,
                              SpectralAllocator& allocator) const override;

 private:
  visual_t eta_incident_, eta_transmitted_;
};

}  // namespace bxdfs
}  // namespace iris

#endif  // _IRIS_BXDFS_MICROFACET_BXDF_