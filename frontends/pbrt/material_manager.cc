#include "frontends/pbrt/material_manager.h"

#include <cstdlib>
#include <iostream>
#include <limits>

namespace iris {
namespace pbrt_frontend {

void MaterialManager::AttributeBegin() {
  if (deferred_pushes_ == std::numeric_limits<size_t>::max()) {
    std::cerr << "ERROR: Attribute stack is too deep" << std::endl;
    exit(EXIT_FAILURE);
  }

  deferred_pushes_ += 1;
}

void MaterialManager::AttributeEnd() {
  if (deferred_pushes_ != 0) {
    deferred_pushes_ -= 1;
    return;
  }

  if (materials_.size() == 1) {
    std::cerr << "ERROR: Mismatched AttributeBegin and AttributeEnd directives"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  materials_.pop();
}

const std::pair<pbrt_proto::v3::Material, MaterialResult>* MaterialManager::Get(
    const std::string& name) const {
  auto iter = materials_.top().find(name);
  if (iter == materials_.top().end()) {
    std::cerr << "WARNING: No material defined with name: " << name
              << std::endl;
    return nullptr;
  }

  return &iter->second;
}

void MaterialManager::Put(
    const std::string& name,
    std::pair<pbrt_proto::v3::Material, MaterialResult> material) {
  while (deferred_pushes_ != 0) {
    materials_.push(materials_.top());
    deferred_pushes_ -= 1;
  }

  materials_.top()[name] = std::move(material);
}

}  // namespace pbrt_frontend
}  // namespace iris
