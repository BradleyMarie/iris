#include "iris/materials/mix_material.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/bxdfs/mock_bxdf.h"
#include "iris/materials/mock_material.h"
#include "iris/point.h"
#include "iris/reflectors/mock_reflector.h"
#include "iris/testing/bxdf_allocator.h"
#include "iris/testing/spectral_allocator.h"
#include "iris/textures/constant_texture.h"
#include "iris/textures/float_texture.h"
#include "iris/textures/test_util.h"

namespace iris {
namespace materials {
namespace {

using ::iris::bxdfs::MockBxdf;
using ::iris::reflectors::MockReflector;
using ::iris::testing::GetBxdfAllocator;
using ::iris::testing::GetSpectralAllocator;
using ::iris::textures::FloatTexture;
using ::iris::textures::MakeConstantTexture;
using ::testing::_;
using ::testing::DoAll;
using ::testing::IsNull;
using ::testing::NotNull;
using ::testing::Return;
using ::testing::SetArgPointee;

TEST(MixMaterialTest, Null) {
  ReferenceCounted<FloatTexture> attenuation = MakeConstantTexture(0.25);
  EXPECT_FALSE(MakeMixMaterial(ReferenceCounted<Material>(),
                               ReferenceCounted<Material>(),
                               std::move(attenuation)));
}

TEST(MixMaterialTest, Evaluate) {
  ReferenceCounted<FloatTexture> attenuation = MakeConstantTexture(0.25);

  MockReflector reflector0;
  EXPECT_CALL(reflector0, Reflectance(1.0)).WillOnce(Return(0.75));
  MockReflector reflector1;
  EXPECT_CALL(reflector1, Reflectance(1.0)).WillOnce(Return(0.125));

  MockBxdf mock_bxdf0;
  EXPECT_CALL(mock_bxdf0, IsDiffuse(NotNull()))
      .WillRepeatedly(DoAll(SetArgPointee<0>(1.0), Return(true)));
  EXPECT_CALL(mock_bxdf0,
              ReflectanceDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0),
                                 Bxdf::Hemisphere::BTDF, _))
      .WillOnce(Return(&reflector0));
  MockBxdf mock_bxdf1;
  EXPECT_CALL(mock_bxdf1, IsDiffuse(NotNull()))
      .WillRepeatedly(DoAll(SetArgPointee<0>(1.0), Return(true)));
  EXPECT_CALL(mock_bxdf1,
              ReflectanceDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0),
                                 Bxdf::Hemisphere::BTDF, _))
      .WillOnce(Return(&reflector1));

  ReferenceCounted<MockMaterial> mock_material0 =
      MakeReferenceCounted<MockMaterial>();
  EXPECT_CALL(*mock_material0, Evaluate(_, _, _)).WillOnce(Return(&mock_bxdf0));

  ReferenceCounted<MockMaterial> mock_material1 =
      MakeReferenceCounted<MockMaterial>();
  EXPECT_CALL(*mock_material1, Evaluate(_, _, _)).WillOnce(Return(&mock_bxdf1));

  ReferenceCounted<Material> mix_material =
      MakeMixMaterial(std::move(mock_material0), std::move(mock_material1),
                      std::move(attenuation));

  const Bxdf* bxdf =
      mix_material->Evaluate(TextureCoordinates{Point(0.0, 0.0, 0.0),
                                                Vector(0.0, 0.0, 0.0),
                                                Vector(0.0, 0.0, 0.0),
                                                {0.0, 0.0},
                                                0.0,
                                                0.0,
                                                0.0,
                                                0.0},
                             GetSpectralAllocator(), GetBxdfAllocator());
  ASSERT_TRUE(bxdf);

  const Reflector* reflector =
      bxdf->ReflectanceDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0),
                               Bxdf::Hemisphere::BTDF, GetSpectralAllocator());
  ASSERT_TRUE(reflector);

  EXPECT_EQ(0.28125, reflector->Reflectance(1.0));
}

}  // namespace
}  // namespace materials
}  // namespace iris
