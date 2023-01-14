#ifndef _IRIS_LIGHT_SCENE_MOCK_LIGHT_SCENE_
#define _IRIS_LIGHT_SCENE_MOCK_LIGHT_SCENE_

#include "googlemock/include/gmock/gmock.h"
#include "iris/light_scene.h"

namespace iris {
namespace light_scenes {

class MockLightScene final : public LightScene {
 public:
  MOCK_METHOD(LightSample*, Sample,
              (const Point&, Random&, LightSampleAllocator&), (const override));
};

}  // namespace light_scenes
}  // namespace iris

#endif  // _IRIS_LIGHT_SCENE_MOCK_LIGHT_SCENE_