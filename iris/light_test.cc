#include "iris/light.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/float.h"
#include "iris/hit_point.h"
#include "iris/internal/light_parameters.h"
#include "iris/power_matcher.h"
#include "iris/ray.h"
#include "iris/reference_countable.h"
#include "iris/sampler.h"
#include "iris/spectral_allocator.h"
#include "iris/spectrum.h"
#include "iris/vector.h"
#include "iris/visibility_tester.h"

namespace iris {
namespace {

class PublicLight : public Light {
  std::optional<SampleResult> Sample(
      const HitPoint& hit_point, Sampler sampler, VisibilityTester& tester,
      SpectralAllocator& allocator) const override {
    return std::nullopt;
  }

  const Spectrum* Emission(const Ray& to_light, VisibilityTester& tester,
                           SpectralAllocator& allocator,
                           visual_t* pdf = nullptr) const override {
    return nullptr;
  }

  visual_t Power(const PowerMatcher& power_matcher,
                 visual_t world_radius_squared) const override {
    return 0.0;
  }
};

class InternalLight : public Light {
 public:
  InternalLight(const internal::LightParameters& parameters)
      : Light(parameters) {}

  std::optional<SampleResult> Sample(
      const HitPoint& hit_point, Sampler sampler, VisibilityTester& tester,
      SpectralAllocator& allocator) const override {
    return std::nullopt;
  }

  const Spectrum* Emission(const Ray& to_light, VisibilityTester& tester,
                           SpectralAllocator& allocator,
                           visual_t* pdf = nullptr) const override {
    return nullptr;
  }

  visual_t Power(const PowerMatcher& power_matcher,
                 visual_t world_radius_squared) const override {
    return 0.0;
  }
};

TEST(Light, PublicLight) { EXPECT_TRUE(PublicLight().IsInvisible()); }

TEST(Light, InternalLight) {
  EXPECT_TRUE(InternalLight({true}).IsInvisible());
  EXPECT_FALSE(InternalLight({false}).IsInvisible());
}

}  // namespace
}  // namespace iris
