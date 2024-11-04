#ifndef _IRIS_BXDFS_MOCK_BXDF_
#define _IRIS_BXDFS_MOCK_BXDF_

#include <optional>
#include <variant>

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
  MOCK_METHOD(bool, IsDiffuse, (visual_t*), (const override));
  MOCK_METHOD((std::variant<std::monostate, DiffuseSample, SpecularSample>),
              Sample,
              (const Vector&, const std::optional<Differentials>& differentials,
               const Vector&, Sampler&, SpectralAllocator&),
              (const override));
  MOCK_METHOD(std::optional<Vector>, SampleDiffuse,
              (const Vector&, const Vector&, Sampler&), (const override));
  MOCK_METHOD(visual_t, PdfDiffuse,
              (const Vector&, const Vector&, const Vector&, Hemisphere),
              (const override));
  MOCK_METHOD(const Reflector*, ReflectanceDiffuse,
              (const Vector&, const Vector&, Hemisphere, SpectralAllocator&),
              (const override));
};

}  // namespace bxdfs
}  // namespace iris

#endif  // _IRIS_BXDFS_MOCK_BXDF_