#ifndef _IRIS_SCENE_
#define _IRIS_SCENE_

#include <memory>

#include "iris/intersector.h"
#include "iris/ray.h"

namespace iris {

// Forward Declaration
class SceneObjects;

class Scene {
 public:
  class Builder {
   public:
    virtual std::unique_ptr<Scene> Build(SceneObjects& scene_objects) const = 0;
    virtual ~Builder() = default;
  };

  virtual void Trace(const Ray& ray, Intersector& intersector) const = 0;
  virtual ~Scene() {}
};

}  // namespace iris

#endif  // _IRIS_SCENE_
