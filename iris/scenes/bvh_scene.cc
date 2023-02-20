#include "iris/scenes/bvh_scene.h"

#include "iris/scenes/internal/bvh_builder.h"
#include "iris/scenes/internal/bvh_node.h"
#include "iris/scenes/internal/bvh_traversal.h"

namespace iris {
namespace scenes {

std::unique_ptr<Scene::Builder> BVHScene::Builder::Create() {
  return std::make_unique<Builder>();
}

std::unique_ptr<Scene> BVHScene::Builder::Build(
    SceneObjects& scene_objects) const {
  auto get_geometry = [&scene_objects](size_t index) {
    return scene_objects.GetGeometry(index);
  };

  auto result = internal::BuildBVH(get_geometry, scene_objects.NumGeometry());
  scene_objects.Reorder(result.geometry_sort_order);

  return std::make_unique<BVHScene>(scene_objects, std::move(result.bvh));
}

void BVHScene::Trace(const Ray& ray, Intersector& intersector) const {
  if (!bvh_nodes_.empty()) {
    internal::Intersect(bvh_nodes_.front(), scene_objects_, ray, intersector);
  }
}

}  // namespace scenes
}  // namespace iris