#include "frontends/pbrt/material_manager.h"

#include <cstdlib>
#include <iostream>

namespace iris {
namespace pbrt_frontend {

const std::pair<pbrt_proto::v3::Material, MaterialResult>& MaterialManager::Get(
    const std::string& name) const {
  auto iter = materials_.find(name);
  if (iter == materials_.end()) {
    std::cerr << "ERROR: No material defined with name: " << name << std::endl;
    exit(EXIT_FAILURE);
  }

  return iter->second;
}

}  // namespace pbrt_frontend
}  // namespace iris
