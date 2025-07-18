#include "iris/bxdfs/transparent_btdf.h"

#include <variant>

#include "googletest/include/gtest/gtest.h"
#include "iris/random/mock_random.h"
#include "iris/reflectors/mock_reflector.h"
#include "iris/testing/bxdf_allocator.h"
#include "iris/testing/spectral_allocator.h"

namespace iris {
namespace bxdfs {
namespace {

using ::iris::random::MockRandom;
using ::iris::reflectors::MockReflector;
using ::iris::testing::GetBxdfAllocator;
using ::iris::testing::GetSpectralAllocator;
using ::testing::_;
using ::testing::Return;

TEST(TransparentBtdfTest, Null) {
  MockReflector reflector;
  EXPECT_TRUE(MakeTransparentBtdf(GetBxdfAllocator(), &reflector));
  EXPECT_FALSE(MakeTransparentBtdf(GetBxdfAllocator(), nullptr));
}

TEST(TransparentBtdfTest, SampleWithDerivatives) {
  MockReflector reflector;
  MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));
  Sampler sampler(rng);

  const Bxdf* bxdf = MakeTransparentBtdf(GetBxdfAllocator(), &reflector);
  auto result = std::get<Bxdf::SpecularSample>(bxdf->Sample(
      Vector(1.0, 1.0, 1.0), {{Vector(1.0, 0.5, 1.0), Vector(0.5, 1.0, 1.0)}},
      Vector(0.0, 0.0, 1.0), sampler, GetSpectralAllocator()));
  EXPECT_EQ(result.hemisphere, Bxdf::Hemisphere::BTDF);
  EXPECT_EQ(Vector(-1.0, -1.0, -1.0), result.direction);
  EXPECT_EQ(result.reflectance, &reflector);
  ASSERT_TRUE(result.differentials);
  EXPECT_EQ(Vector(-1.0, -0.5, -1.0), result.differentials->dx);
  EXPECT_EQ(Vector(-0.5, -1.0, -1.0), result.differentials->dy);
  EXPECT_EQ(result.pdf, 1.0);
  EXPECT_EQ(result.etendue_conservation_scalar, 1.0);
}

}  // namespace
}  // namespace bxdfs
}  // namespace iris
