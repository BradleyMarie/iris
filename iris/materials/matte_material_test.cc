#define _USE_MATH_DEFINES
#include "iris/materials/matte_material.h"

#include <cmath>

#include "googletest/include/gtest/gtest.h"
#include "iris/bxdfs/lambertian_bxdf.h"
#include "iris/bxdfs/oren_nayar_bxdf.h"
#include "iris/reflectors/mock_reflector.h"
#include "iris/testing/bxdf_allocator.h"
#include "iris/testing/spectral_allocator.h"
#include "iris/textures/constant_texture.h"

namespace iris {
namespace materials {
namespace {

using ::iris::bxdfs::LambertianBrdf;
using ::iris::bxdfs::OrenNayarBrdf;
using ::iris::reflectors::MockReflector;
using ::iris::testing::GetBxdfAllocator;
using ::iris::testing::GetSpectralAllocator;
using ::iris::textures::ConstantPointerTexture2D;
using ::iris::textures::ConstantValueTexture2D;
using ::iris::textures::PointerTexture2D;
using ::iris::textures::ValueTexture2D;

TEST(MatteMaterialTest, EvaluateEmpty) {
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> reflectance =
      MakeReferenceCounted<
          ConstantPointerTexture2D<Reflector, SpectralAllocator>>(
          ReferenceCounted<Reflector>());
  ReferenceCounted<ValueTexture2D<visual>> sigma =
      MakeReferenceCounted<ConstantValueTexture2D<visual>>(1.0);
  MatteMaterial material(std::move(reflectance), std::move(sigma));

  ASSERT_FALSE(material.Evaluate(TextureCoordinates{{0.0, 0.0}, std::nullopt},
                                 GetSpectralAllocator(), GetBxdfAllocator()));
}

TEST(MatteMaterialTest, EvaluateLambertian) {
  ReferenceCounted<Reflector> reflector = MakeReferenceCounted<MockReflector>();

  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> reflectance =
      MakeReferenceCounted<
          ConstantPointerTexture2D<Reflector, SpectralAllocator>>(reflector);
  ReferenceCounted<ValueTexture2D<visual>> sigma =
      MakeReferenceCounted<ConstantValueTexture2D<visual>>(0.0);
  MatteMaterial material(std::move(reflectance), std::move(sigma));

  const Bxdf* result =
      material.Evaluate(TextureCoordinates{{0.0, 0.0}, std::nullopt},
                        GetSpectralAllocator(), GetBxdfAllocator());
  ASSERT_TRUE(result);
  EXPECT_TRUE(dynamic_cast<const LambertianBrdf*>(result));
}

TEST(MatteMaterialTest, EvaluateOrenNayar) {
  ReferenceCounted<Reflector> reflector = MakeReferenceCounted<MockReflector>();

  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> reflectance =
      MakeReferenceCounted<
          ConstantPointerTexture2D<Reflector, SpectralAllocator>>(reflector);
  ReferenceCounted<ValueTexture2D<visual>> sigma =
      MakeReferenceCounted<ConstantValueTexture2D<visual>>(1.0);
  MatteMaterial material(std::move(reflectance), std::move(sigma));

  const Bxdf* result =
      material.Evaluate(TextureCoordinates{{0.0, 0.0}, std::nullopt},
                        testing::GetSpectralAllocator(), GetBxdfAllocator());
  ASSERT_TRUE(result);
  EXPECT_TRUE(dynamic_cast<const OrenNayarBrdf*>(result));
}

}  // namespace
}  // namespace materials
}  // namespace iris