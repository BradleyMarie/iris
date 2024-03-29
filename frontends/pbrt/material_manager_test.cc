#include "frontends/pbrt/material_manager.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/materials/mock_material.h"

class TestObjectBuilder
    : public iris::pbrt_frontend::ObjectBuilder<
          std::tuple<iris::ReferenceCounted<iris::Material>,
                     iris::ReferenceCounted<iris::Material>,
                     iris::ReferenceCounted<iris::NormalMap>,
                     iris::ReferenceCounted<iris::NormalMap>>,
          iris::pbrt_frontend::TextureManager&> {
 public:
  TestObjectBuilder() : ObjectBuilder({}) {}

  std::tuple<iris::ReferenceCounted<iris::Material>,
             iris::ReferenceCounted<iris::Material>,
             iris::ReferenceCounted<iris::NormalMap>,
             iris::ReferenceCounted<iris::NormalMap>>
  Build(const std::unordered_map<std::string_view,
                                 iris::pbrt_frontend::Parameter>& parameters,
        iris::pbrt_frontend::TextureManager& texture_manager) const override {
    iris::ReferenceCounted<iris::Material> material0;
    iris::ReferenceCounted<iris::Material> material1;
    iris::ReferenceCounted<iris::NormalMap> normal_map0;
    iris::ReferenceCounted<iris::NormalMap> normal_map1;
    return {material0, material1, normal_map0, normal_map1};
  }
};

TEST(MaterialManager, GetMissing) {
  iris::pbrt_frontend::MaterialManager material_manager;
  EXPECT_EXIT(material_manager.Get("test"),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: No material defined with name: test");
}

TEST(MaterialManager, Get) {
  auto material = std::make_shared<TestObjectBuilder>();
  iris::pbrt_frontend::MaterialManager material_manager;
  material_manager.Put("test", material);
  EXPECT_EQ(material.get(), material_manager.Get("test").get());
}

TEST(MaterialManager, Clear) {
  auto material = std::make_shared<TestObjectBuilder>();
  iris::pbrt_frontend::MaterialManager material_manager;
  material_manager.Clear();
  EXPECT_EXIT(material_manager.Get("test"),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: No material defined with name: test");
}