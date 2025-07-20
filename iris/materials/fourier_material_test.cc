#include "iris/materials/fourier_material.h"

#include <cstddef>
#include <utility>
#include <vector>

#include "googletest/include/gtest/gtest.h"
#include "iris/reflectors/mock_reflector.h"
#include "iris/testing/bxdf_allocator.h"
#include "iris/testing/spectral_allocator.h"

namespace iris {
namespace materials {
namespace {

using ::iris::reflectors::MockReflector;
using ::iris::testing::GetBxdfAllocator;
using ::iris::testing::GetSpectralAllocator;

TEST(FourierMaterialTest, EvaluateY) {
  std::vector<geometric> elevational_samples = {0.0, 0.25, 0.75, 1.0};
  std::vector<visual> cdf = {0.0,  0.0,  0.0,  0.0,  0.33, 0.33, 0.33, 0.33,
                             0.66, 0.66, 0.66, 0.66, 1.0,  1.0,  1.0,  1.0};
  std::vector<visual> coefficients = {1.0};
  std::vector<std::pair<size_t, size_t>> coefficient_extents = {
      {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1},
      {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}};

  ReferenceCounted<Material> material = MakeFourierMaterial(
      elevational_samples, cdf, coefficient_extents, coefficients, 1.5, true);

  const Bxdf* result =
      material->Evaluate(TextureCoordinates{{0.0, 0.0}, std::nullopt},
                         GetSpectralAllocator(), GetBxdfAllocator());
  ASSERT_TRUE(result);
}

TEST(FourierMaterialTest, EvaluateYRB) {
  ReferenceCounted<Reflector> r = MakeReferenceCounted<MockReflector>();
  ReferenceCounted<Reflector> g = MakeReferenceCounted<MockReflector>();
  ReferenceCounted<Reflector> b = MakeReferenceCounted<MockReflector>();

  std::vector<geometric> elevational_samples = {0.0, 0.25, 0.75, 1.0};
  std::vector<visual> cdf = {0.0,  0.0,  0.0,  0.0,  0.33, 0.33, 0.33, 0.33,
                             0.66, 0.66, 0.66, 0.66, 1.0,  1.0,  1.0,  1.0};
  std::vector<visual> coefficients = {1.0};
  std::vector<std::pair<size_t, size_t>> coefficient_extents = {
      {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1},
      {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}};

  ReferenceCounted<Material> material = MakeFourierMaterial(
      r, g, b, elevational_samples, cdf, coefficient_extents, coefficients,
      coefficients, coefficients, 1.5, true);

  const Bxdf* result =
      material->Evaluate(TextureCoordinates{{0.0, 0.0}, std::nullopt},
                         GetSpectralAllocator(), GetBxdfAllocator());
  ASSERT_TRUE(result);
}

}  // namespace
}  // namespace materials
}  // namespace iris
