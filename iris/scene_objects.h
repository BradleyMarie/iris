#ifndef _IRIS_SCENE_OBJECTS_
#define _IRIS_SCENE_OBJECTS_

#include <cassert>
#include <memory>
#include <set>
#include <utility>
#include <vector>

#include "iris/bounding_box.h"
#include "iris/environmental_light.h"
#include "iris/geometry.h"
#include "iris/light.h"
#include "iris/matrix.h"
#include "iris/reference_counted.h"

namespace iris {

class SceneObjects final {
 public:
  SceneObjects() = delete;
  SceneObjects(const SceneObjects&) = delete;
  SceneObjects(SceneObjects&&) = default;
  SceneObjects& operator=(const SceneObjects&) = delete;
  SceneObjects& operator=(SceneObjects&&) = default;

  class Builder final {
   public:
    Builder() = default;
    Builder(const Builder&) = delete;
    Builder(Builder&&) = default;

    Builder& operator=(const Builder&) = delete;
    Builder& operator=(Builder&&) = default;

    void Add(ReferenceCounted<Geometry> geometry,
             const Matrix& matrix = iris::Matrix::Identity());
    void Add(ReferenceCounted<Light> light);
    void Set(ReferenceCounted<EnvironmentalLight> environmental_light);
    SceneObjects Build();

   private:
    std::set<std::pair<ReferenceCounted<Geometry>, const Matrix*>> geometry_;
    std::set<ReferenceCounted<Light>> lights_;
    std::set<Matrix> matrices_;
    ReferenceCounted<EnvironmentalLight> environmental_light_;
    BoundingBox::Builder bounds_builder_;

    friend class SceneObjects;
  };

  BoundingBox GetBounds() const { return bounds_.Build(); }

  size_t NumGeometry() const noexcept { return geometry_.size(); }

  std::pair<const Geometry&, const Matrix*> GetGeometry(
      size_t index) const noexcept {
    assert(index < geometry_.size());
    return std::make_pair(std::cref(*geometry_[index].first),
                          geometry_[index].second);
  }

  size_t NumLights() const noexcept { return lights_.size(); }

  const Light& GetLight(size_t index) const noexcept {
    assert(index < lights_.size());
    return *lights_[index];
  }

  const EnvironmentalLight* GetEnvironmentalLight() const noexcept {
    return environmental_light_.Get();
  }

  void Reorder(std::vector<size_t> new_geometry_positions = {},
               std::vector<size_t> new_light_positions = {}) noexcept;

 private:
  SceneObjects(std::vector<std::pair<ReferenceCounted<Geometry>, const Matrix*>>
                   geometry,
               std::vector<ReferenceCounted<Light>> lights,
               std::set<Matrix> matrices,
               ReferenceCounted<EnvironmentalLight> environmental_light,
               BoundingBox::Builder bounds) noexcept
      : geometry_(std::move(geometry)),
        lights_(std::move(lights)),
        matrices_(std::move(matrices)),
        environmental_light_(std::move(environmental_light)),
        bounds_(bounds) {}

  std::vector<std::pair<ReferenceCounted<Geometry>, const Matrix*>> geometry_;
  std::vector<ReferenceCounted<Light>> lights_;
  std::set<Matrix> matrices_;
  ReferenceCounted<EnvironmentalLight> environmental_light_;
  BoundingBox::Builder bounds_;
};

}  // namespace iris

#endif  // _IRIS_SCENE_OBJECTS_