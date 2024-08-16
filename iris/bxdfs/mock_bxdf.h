#ifndef _IRIS_BXDFS_MOCK_BXDF_
#define _IRIS_BXDFS_MOCK_BXDF_

#include <optional>

#include "googlemock/include/gmock/gmock.h"
#include "iris/bxdf.h"
#include "iris/float.h"
#include "iris/reflector.h"
#include "iris/sampler.h"
#include "iris/spectral_allocator.h"
#include "iris/vector.h"

namespace iris {
namespace bxdfs {

class MockBxdf final : public Bxdf {
 public:
  MOCK_METHOD(bool, IsDiffuse, (), (const override));
  MOCK_METHOD(std::optional<SampleResult>, Sample,
              (const Vector&, const std::optional<Differentials>& differentials,
               const Vector&, Sampler&),
              (const override));
  MOCK_METHOD(std::optional<visual_t>, Pdf,
              (const Vector&, const Vector&, const Vector&, const Bxdf*,
               Hemisphere),
              (const override));
  MOCK_METHOD(const Reflector*, Reflectance,
              (const Vector&, const Vector&, const Bxdf*, Hemisphere,
               SpectralAllocator&),
              (const override));
};

}  // namespace bxdfs
}  // namespace iris

#endif  // _IRIS_BXDFS_MOCK_BXDF_