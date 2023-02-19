#include <memory>

#include "iris/scene.h"
#include "iris/scene_objects.h"

namespace iris {
namespace scenes {
namespace internal {
class BVHNode;
}  // namespace internal

class BVHScene final : public Scene {
 public:
  class Builder final : public Scene::Builder {
   public:
    static std::unique_ptr<Scene::Builder> Create();
    std::unique_ptr<Scene> Build(SceneObjects& scene_objects) const override;
  };

  BVHScene(const SceneObjects& scene_objects,
           std::unique_ptr<internal::BVHNode[]> bvh_nodes) noexcept
      : scene_objects_(scene_objects), bvh_nodes_(std::move(bvh_nodes)) {}

  void Trace(const Ray& ray, Intersector& intersector) const override;

 private:
  const SceneObjects& scene_objects_;
  const std::unique_ptr<internal::BVHNode[]> bvh_nodes_;
};

}  // namespace scenes
}  // namespace iris