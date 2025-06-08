#include "iris/scenes/list_scene.h"

#include <memory>

#include "iris/intersector.h"
#include "iris/ray.h"
#include "iris/scene.h"
#include "iris/scene_objects.h"

namespace iris {
namespace scenes {
namespace {

class ListScene final : public Scene {
 public:
  class Builder final : public Scene::Builder {
   public:
    std::unique_ptr<Scene> Build(SceneObjects& scene_objects) const override;
  };

  ListScene(const SceneObjects& scene_objects) noexcept
      : scene_objects_(scene_objects) {}

  void Trace(const Ray& ray, Intersector& intersector) const override;

 private:
  const SceneObjects& scene_objects_;
};

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

}  // namespace

std::unique_ptr<Scene::Builder> MakeListSceneBuilder() {
  return std::make_unique<ListScene::Builder>();
}

}  // namespace scenes
}  // namespace iris
