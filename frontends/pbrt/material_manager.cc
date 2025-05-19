#include "frontends/pbrt/material_manager.h"

#include <cstdlib>
#include <iostream>

namespace iris {
namespace pbrt_frontend {

void MaterialManager::AttributeBegin() { materials_.push(materials_.top()); }

void MaterialManager::AttributeEnd() {
  if (materials_.size() == 1) {
    std::cerr << "ERROR: Mismatched AttributeBegin and AttributeEnd directives"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  materials_.pop();
}

const std::pair<pbrt_proto::v3::Material, MaterialResult>& MaterialManager::Get(
    const std::string& name) const {
  auto iter = materials_.top().find(name);
  if (iter == materials_.top().end()) {
    std::cerr << "ERROR: No material defined with name: " << name << std::endl;
    exit(EXIT_FAILURE);
  }

  return iter->second;
}

}  // namespace pbrt_frontend
}  // namespace iris
