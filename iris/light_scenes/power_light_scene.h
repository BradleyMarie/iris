#ifndef _IRIS_LIGHT_SCENES_POWER_LIGHT_SCENE_
#define _IRIS_LIGHT_SCENES_POWER_LIGHT_SCENE_

#include <memory>

#include "iris/light_scene.h"

namespace iris {
namespace light_scenes {

std::unique_ptr<LightScene::Builder> MakePowerLightSceneBuilder();

}  // namespace light_scenes
}  // namespace iris

#endif  // _IRIS_LIGHT_SCENES_POWER_LIGHT_SCENE_
