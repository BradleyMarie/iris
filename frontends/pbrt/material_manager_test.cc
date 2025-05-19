#include "frontends/pbrt/material_manager.h"

#include <cstdlib>

#include "frontends/pbrt/materials/result.h"
#include "googletest/include/gtest/gtest.h"
#include "iris/materials/mock_material.h"
#include "pbrt_proto/v3/pbrt.pb.h"

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

  EXPECT_EXIT(material_manager.Get("test"), ExitedWithCode(EXIT_FAILURE),
              "ERROR: No material defined with name: test");
}

TEST(MaterialManager, Get) {
  MaterialManager material_manager;

  std::pair<pbrt_proto::v3::Material, MaterialResult> entry;
  entry.second.material = MakeReferenceCounted<MockMaterial>();

  material_manager.Put("test", entry);
  EXPECT_EQ(entry.second.material.Get(),
            material_manager.Get("test").second.material.Get());
}

}  // namespace
}  // namespace pbrt_frontend
}  // namespace iris
