#include "iris/bxdfs/fourier_bxdf.h"

#include <cmath>
#include <cstddef>
#include <limits>
#include <utility>
#include <vector>

#include "googletest/include/gtest/gtest.h"
#include "iris/float.h"
#include "iris/reflectors/mock_reflector.h"
#include "iris/testing/bxdf_allocator.h"

namespace iris {
namespace bxdfs {
namespace {

using ::iris::reflectors::MockReflector;
using ::iris::testing::GetBxdfAllocator;
using ::testing::_;
using ::testing::DoAll;
using ::testing::Eq;
using ::testing::FieldsAre;
using ::testing::IsNull;
using ::testing::NotNull;
using ::testing::Ref;
using ::testing::Return;
using ::testing::SetArgPointee;

TEST(FourierBrdfTest, MakeFourierBxdfYNone) {
  std::vector<geometric> empty_geometric;
  std::vector<visual> empty_visual;
  std::vector<std::pair<size_t, size_t>> empty_coefficient_extents;
  std::vector<visual> elevational_samples = {-1.0, 0.0, 1.0};
  std::vector<visual> cdf = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0};
  std::vector<visual> coefficients = {1.0};
  std::vector<std::pair<size_t, size_t>> coefficient_extents = {
      {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}};
  visual eta = 1.0;

  EXPECT_EQ(nullptr, MakeFourierBxdf(GetBxdfAllocator(), empty_geometric, cdf,
                                     coefficient_extents, coefficients, eta));
  EXPECT_EQ(nullptr, MakeFourierBxdf(GetBxdfAllocator(), elevational_samples,
                                     empty_visual, coefficient_extents,
                                     coefficients, eta));
  EXPECT_EQ(nullptr,
            MakeFourierBxdf(GetBxdfAllocator(), elevational_samples, cdf,
                            empty_coefficient_extents, coefficients, eta));
  EXPECT_EQ(nullptr,
            MakeFourierBxdf(GetBxdfAllocator(), elevational_samples, cdf,
                            coefficient_extents, coefficients, 0.0));
  EXPECT_EQ(nullptr,
            MakeFourierBxdf(GetBxdfAllocator(), elevational_samples, cdf,
                            coefficient_extents, coefficients,
                            std::numeric_limits<visual_t>::infinity()));
}

TEST(FourierBrdfTest, MakeFourierBxdfYCreates) {
  std::vector<visual> elevational_samples = {-1.0, 0.0, 1.0};
  std::vector<visual> cdf = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0};
  std::vector<visual> coefficients = {1.0};
  std::vector<std::pair<size_t, size_t>> coefficient_extents = {
      {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}};
  visual eta = 1.0;

  EXPECT_NE(nullptr,
            MakeFourierBxdf(GetBxdfAllocator(), elevational_samples, cdf,
                            coefficient_extents, coefficients, eta));
}

TEST(FourierBrdfTest, MakeFourierBxdfYRBNone) {
  MockReflector reflector;
  std::vector<geometric> empty_geometric;
  std::vector<visual> empty_visual;
  std::vector<std::pair<size_t, size_t>> empty_coefficient_extents;
  std::vector<visual> elevational_samples = {-1.0, 0.0, 1.0};
  std::vector<visual> cdf = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0};
  std::vector<visual> coefficients = {1.0};
  std::vector<std::pair<size_t, size_t>> coefficient_extents = {
      {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}};
  visual eta = 1.0;

  EXPECT_EQ(nullptr,
            MakeFourierBxdf(GetBxdfAllocator(), nullptr, nullptr, nullptr,
                            elevational_samples, cdf, coefficient_extents,
                            coefficients, coefficients, coefficients, eta));
  EXPECT_EQ(nullptr, MakeFourierBxdf(GetBxdfAllocator(), &reflector, &reflector,
                                     &reflector, empty_geometric, cdf,
                                     coefficient_extents, coefficients,
                                     coefficients, coefficients, eta));
  EXPECT_EQ(nullptr, MakeFourierBxdf(
                         GetBxdfAllocator(), &reflector, &reflector, &reflector,
                         elevational_samples, empty_visual, coefficient_extents,
                         coefficients, coefficients, coefficients, eta));
  EXPECT_EQ(nullptr, MakeFourierBxdf(GetBxdfAllocator(), &reflector, &reflector,
                                     &reflector, elevational_samples, cdf,
                                     empty_coefficient_extents, coefficients,
                                     coefficients, coefficients, eta));
  EXPECT_EQ(nullptr, MakeFourierBxdf(GetBxdfAllocator(), &reflector, &reflector,
                                     &reflector, elevational_samples, cdf,
                                     coefficient_extents, coefficients,
                                     coefficients, coefficients, 0.0));
  EXPECT_EQ(nullptr, MakeFourierBxdf(
                         GetBxdfAllocator(), &reflector, &reflector, &reflector,
                         elevational_samples, cdf, coefficient_extents,
                         coefficients, coefficients, coefficients,
                         std::numeric_limits<visual_t>::infinity()));
}

TEST(FourierBrdfTest, MakeFourierBxdfYRBCreates) {
  MockReflector reflector;
  std::vector<visual> elevational_samples = {-1.0, 0.0, 1.0};
  std::vector<visual> cdf = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0};
  std::vector<visual> coefficients = {1.0};
  std::vector<std::pair<size_t, size_t>> coefficient_extents = {
      {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}};
  visual eta = 1.0;

  EXPECT_NE(nullptr, MakeFourierBxdf(GetBxdfAllocator(), &reflector, &reflector,
                                     &reflector, elevational_samples, cdf,
                                     coefficient_extents, coefficients,
                                     coefficients, coefficients, eta));
}

}  // namespace
}  // namespace bxdfs
}  // namespace iris
