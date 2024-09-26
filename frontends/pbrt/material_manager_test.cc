#include "frontends/pbrt/material_manager.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/materials/mock_material.h"

namespace iris {
namespace pbrt_frontend {
namespace {

class TestObjectBuilder : public materials::NestedMaterialBuilder {
 public:
  TestObjectBuilder() : ObjectBuilder({}) {}

  materials::MaterialBuilderResult Build(
      const std::unordered_map<std::string_view, Parameter>& parameters,
      const MaterialManager& material_manager, TextureManager& texture_manager,
      SpectrumManager& spectrum_manager) const override {
    ReferenceCounted<Material> material0;
    ReferenceCounted<Material> material1;
    ReferenceCounted<NormalMap> normal_map0;
    ReferenceCounted<NormalMap> normal_map1;
    return {material0, material1, normal_map0, normal_map1};
  }
};

TEST(MaterialManager, GetMissing) {
  MaterialManager material_manager;
  EXPECT_EXIT(material_manager.Get("test"),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: No material defined with name: test");
}

TEST(MaterialManager, Get) {
  auto material = std::make_shared<TestObjectBuilder>();
  MaterialManager material_manager;
  material_manager.Put("test", material);
  EXPECT_EQ(material.get(), material_manager.Get("test").get());
}

TEST(MaterialManager, Clear) {
  auto material = std::make_shared<TestObjectBuilder>();
  MaterialManager material_manager;
  material_manager.Clear();
  EXPECT_EXIT(material_manager.Get("test"),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: No material defined with name: test");
}

}  // namespace
}  // namespace pbrt_frontend
}  // namespace iris