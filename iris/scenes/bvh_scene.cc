#include "iris/scenes/bvh_scene.h"

#include <memory>
#include <utility>
#include <vector>

#include "iris/scene.h"
#include "iris/scene_objects.h"
#include "iris/scenes/internal/aligned_vector.h"
#include "iris/scenes/internal/bvh_builder.h"
#include "iris/scenes/internal/bvh_node.h"
#include "iris/scenes/internal/bvh_traversal.h"

namespace iris {
namespace scenes {
namespace internal {
class BVHNode;
}  // namespace internal

namespace {

using ::iris::scenes::internal::AlignedVector;
using ::iris::scenes::internal::BuildBVH;
using ::iris::scenes::internal::BuildBVHResult;
using ::iris::scenes::internal::BVHNode;
using ::iris::scenes::internal::Intersect;

class BVHScene final : public Scene {
 public:
  class Builder final : public Scene::Builder {
   public:
    std::unique_ptr<Scene> Build(SceneObjects& scene_objects) const override;
  };

  BVHScene(const SceneObjects& scene_objects,
           AlignedVector<BVHNode> bvh_nodes) noexcept
      : scene_objects_(scene_objects), bvh_nodes_(std::move(bvh_nodes)) {}

  void Trace(const Ray& ray, Intersector& intersector) const override;

 private:
  const SceneObjects& scene_objects_;
  AlignedVector<BVHNode> bvh_nodes_;
};

std::unique_ptr<Scene> BVHScene::Builder::Build(
    SceneObjects& scene_objects) const {
  BuildBVHResult result =
      BuildBVH([&scene_objects](
                   size_t index) { return scene_objects.GetGeometry(index); },
               scene_objects.NumGeometry(), /*for_scene=*/true);
  scene_objects.Reorder(std::move(result.geometry_sort_order));

  return std::make_unique<BVHScene>(scene_objects, std::move(result.bvh));
}

void BVHScene::Trace(const Ray& ray, Intersector& intersector) const {
  if (!bvh_nodes_.empty()) {
    Intersect(bvh_nodes_.front(), scene_objects_, ray, intersector);
  }
}

}  // namespace

std::unique_ptr<Scene::Builder> MakeBVHSceneBuilder() {
  return std::make_unique<BVHScene::Builder>();
}

}  // namespace scenes
}  // namespace iris
