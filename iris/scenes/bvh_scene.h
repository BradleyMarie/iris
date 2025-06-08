#ifndef _IRIS_SCENES_BVH_SCENE_
#define _IRIS_SCENES_BVH_SCENE_

#include <memory>

#include "iris/scene.h"

namespace iris {
namespace scenes {

std::unique_ptr<Scene::Builder> MakeBVHSceneBuilder();

}  // namespace scenes
}  // namespace iris

#endif  // _IRIS_SCENES_BVH_SCENE_
