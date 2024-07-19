#ifndef _IRIS_SCENE_OBJECTS_
#define _IRIS_SCENE_OBJECTS_

#include <cassert>
#include <memory>
#include <set>
#include <span>
#include <utility>
#include <vector>

#include "iris/environmental_light.h"
#include "iris/geometry.h"
#include "iris/light.h"
#include "iris/matrix.h"
#include "iris/reference_counted.h"

namespace iris {

class SceneObjects {
 public:
  SceneObjects() = delete;
  SceneObjects(const SceneObjects&) = delete;
  SceneObjects(SceneObjects&&) = default;
  SceneObjects& operator=(const SceneObjects&) = delete;
  SceneObjects& operator=(SceneObjects&&) = default;

  class Builder {
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

    friend class SceneObjects;
  };

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

  void Reorder(std::span<const size_t> new_geometry_positions = {},
               std::span<const size_t> new_light_positions = {}) noexcept;

 private:
  SceneObjects(Builder&& builder);

  std::vector<std::pair<ReferenceCounted<Geometry>, const Matrix*>> geometry_;
  std::vector<ReferenceCounted<Light>> lights_;
  std::set<Matrix> matrices_;
  ReferenceCounted<EnvironmentalLight> environmental_light_;
};

}  // namespace iris

#endif  // _IRIS_SCENE_OBJECTS_