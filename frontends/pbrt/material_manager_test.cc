#include "frontends/pbrt/material_manager.h"

#include <cstdlib>

#include "frontends/pbrt/materials/result.h"
#include "googletest/include/gtest/gtest.h"
#include "iris/materials/mock_material.h"
#include "pbrt_proto/v3/v3.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace {

using ::iris::materials::MockMaterial;
using ::testing::ExitedWithCode;

TEST(MaterialManager, MismatchedAttribute) {
  MaterialManager material_manager;

  EXPECT_EXIT(material_manager.AttributeEnd(), ExitedWithCode(EXIT_FAILURE),
              "ERROR: Mismatched AttributeBegin and AttributeEnd directives");
}

TEST(MaterialManager, GetMissing) {
  MaterialManager material_manager;

  material_manager.AttributeBegin();
  material_manager.Put("test",
                       std::pair<pbrt_proto::v3::Material, MaterialResult>());
  material_manager.AttributeEnd();

  EXPECT_FALSE(material_manager.Get("test"));
}

TEST(MaterialManager, Get) {
  MaterialManager material_manager;

  std::pair<pbrt_proto::v3::Material, MaterialResult> entry;
  entry.second.materials[0] = MakeReferenceCounted<MockMaterial>();
  entry.second.materials[1] = MakeReferenceCounted<MockMaterial>();

  material_manager.Put("test", entry);
  EXPECT_EQ(entry.second.materials[0].Get(),
            material_manager.Get("test")->second.materials[0].Get());
  EXPECT_EQ(entry.second.materials[1].Get(),
            material_manager.Get("test")->second.materials[1].Get());
}

}  // namespace
}  // namespace pbrt_frontend
}  // namespace iris
