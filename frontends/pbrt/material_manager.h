#ifndef _FRONTENDS_PBRT_MATERIAL_MANAGER_
#define _FRONTENDS_PBRT_MATERIAL_MANAGER_

#include <stack>
#include <string>
#include <unordered_map>
#include <utility>

#include "frontends/pbrt/materials/result.h"
#include "pbrt_proto/v3/v3.pb.h"

namespace iris {
namespace pbrt_frontend {

class MaterialManager {
 public:
  MaterialManager() { materials_.emplace(); }

  void AttributeBegin();
  void AttributeEnd();

  const std::pair<pbrt_proto::v3::Material, MaterialResult>* Get(
      const std::string& name) const;

  void Put(const std::string& name,
           std::pair<pbrt_proto::v3::Material, MaterialResult> material) {
    materials_.top()[name] = std::move(material);
  }

 private:
  std::stack<std::unordered_map<
      std::string, std::pair<pbrt_proto::v3::Material, MaterialResult>>>
      materials_;
};

}  // namespace pbrt_frontend
}  // namespace iris

#endif  // _FRONTENDS_PBRT_MATERIAL_MANAGER_
