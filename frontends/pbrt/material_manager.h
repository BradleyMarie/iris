#ifndef _FRONTENDS_PBRT_MATERIAL_MANAGER_
#define _FRONTENDS_PBRT_MATERIAL_MANAGER_

#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

#include "frontends/pbrt/materials/material_builder.h"

namespace iris {
namespace pbrt_frontend {

class MaterialManager {
 public:
  std::shared_ptr<materials::NestedMaterialBuilder> Get(
      std::string_view name) const;
  void Put(std::string_view name,
           std::shared_ptr<materials::NestedMaterialBuilder> texture);
  void Clear();

 private:
  std::unordered_map<std::string,
                     std::shared_ptr<materials::NestedMaterialBuilder>>
      materials_;
  mutable std::string temp_key_;
};

}  // namespace pbrt_frontend
}  // namespace iris

#endif  // _FRONTENDS_PBRT_MATERIAL_MANAGER_