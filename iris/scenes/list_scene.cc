#include "iris/scenes/list_scene.h"

#include <memory>

namespace iris {
namespace scenes {

std::unique_ptr<Scene::Builder> ListScene::Builder::Create() {
  return std::make_unique<Builder>();
}

std::unique_ptr<Scene> ListScene::Builder::Build(
    SceneObjects& scene_objects) const {
  return std::make_unique<ListScene>(scene_objects);
}

void ListScene::Trace(const Ray& ray, Intersector& intersector) const {
  for (size_t i = 0; i < scene_objects_.NumGeometry(); i++) {
    auto [geometry, model_to_world] = scene_objects_.GetGeometry(i);
    if (intersector.Intersect(geometry, model_to_world)) {
      break;
    }
  }
}

}  // namespace scenes
}  // namespace iris