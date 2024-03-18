#ifndef _FRONTENDS_PBRT_MATERIAL_MANAGER_
#define _FRONTENDS_PBRT_MATERIAL_MANAGER_

#include <memory>
#include <string>
#include <string_view>
#include <tuple>
#include <unordered_map>

#include "frontends/pbrt/object_builder.h"
#include "frontends/pbrt/texture_manager.h"
#include "iris/material.h"
#include "iris/normal_map.h"
#include "iris/reference_counted.h"

namespace iris::pbrt_frontend {

class MaterialManager {
 public:
  std::shared_ptr<ObjectBuilder<
      std::tuple<ReferenceCounted<Material>, ReferenceCounted<Material>,
                 ReferenceCounted<NormalMap>, ReferenceCounted<NormalMap>>,
      TextureManager&>>
  Get(std::string_view name) const;
  void Put(
      std::string_view name,
      std::shared_ptr<ObjectBuilder<
          std::tuple<ReferenceCounted<Material>, ReferenceCounted<Material>,
                     ReferenceCounted<NormalMap>, ReferenceCounted<NormalMap>>,
          TextureManager&>>
          texture);
  void Clear();

 private:
  std::unordered_map<
      std::string,
      std::shared_ptr<ObjectBuilder<
          std::tuple<ReferenceCounted<Material>, ReferenceCounted<Material>,
                     ReferenceCounted<NormalMap>, ReferenceCounted<NormalMap>>,
          TextureManager&>>>
      materials_;
  mutable std::string temp_key_;
};

}  // namespace iris::pbrt_frontend

#endif  // _FRONTENDS_PBRT_MATERIAL_MANAGER_