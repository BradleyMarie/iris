#include "iris/scenes/list_scene.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/testing/scene.h"

TEST(ListSceneTest, SceneTestSuite) {
  auto builder = iris::scenes::ListScene::Builder::Create();
  iris::testing::SceneTestSuite(*builder);
}