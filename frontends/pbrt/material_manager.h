#ifndef _FRONTENDS_PBRT_MATERIAL_MANAGER_
#define _FRONTENDS_PBRT_MATERIAL_MANAGER_

#include <stack>
#include <string>
#include <utility>

#include "absl/container/flat_hash_map.h"
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
           std::pair<pbrt_proto::v3::Material, MaterialResult> material);

 private:
  std::stack<absl::flat_hash_map<
      std::string, std::pair<pbrt_proto::v3::Material, MaterialResult>>>
      materials_;
  size_t deferred_pushes_ = 0;
};

}  // namespace pbrt_frontend
}  // namespace iris

#endif  // _FRONTENDS_PBRT_MATERIAL_MANAGER_
