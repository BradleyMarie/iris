#ifndef _IRIS_SCENES_LIST_SCENE_
#define _IRIS_SCENES_LIST_SCENE_

#include <memory>

#include "iris/scene.h"

namespace iris {
namespace scenes {

std::unique_ptr<Scene::Builder> MakeListSceneBuilder();

}  // namespace scenes
}  // namespace iris

#endif  // _IRIS_SCENES_LIST_SCENE_
