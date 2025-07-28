#include "frontends/pbrt/shapes/trianglemesh.h"

#include <cmath>
#include <cstdio>
#include <iostream>
#include <utility>
#include <vector>

#include "frontends/pbrt/defaults.h"
#include "frontends/pbrt/texture_manager.h"
#include "iris/emissive_material.h"
#include "iris/geometry.h"
#include "iris/geometry/triangle_mesh.h"
#include "iris/material.h"
#include "iris/matrix.h"
#include "iris/normal_map.h"
#include "iris/reference_counted.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace shapes {

using ::iris::geometry::AllocateTriangleMesh;
using ::pbrt_proto::v3::FloatTextureParameter;
using ::pbrt_proto::v3::Shape;

std::pair<std::vector<ReferenceCounted<Geometry>>, Matrix> MakeTriangleMesh(
    const Shape::TriangleMesh& trianglemesh, const Matrix& model_to_world,
    const ReferenceCounted<Material>& front_material,
    const ReferenceCounted<Material>& back_material,
    const ReferenceCounted<EmissiveMaterial>& front_emissive_material,
    const ReferenceCounted<EmissiveMaterial>& back_emissive_material,
    const ReferenceCounted<NormalMap>& front_normal_map,
    const ReferenceCounted<NormalMap>& back_normal_map,
    TextureManager& texture_manager) {
  Shape::TriangleMesh with_defaults = Defaults().shapes().trianglemesh();
  with_defaults.MergeFrom(trianglemesh);

  if (with_defaults.n_size() != 0 &&
      with_defaults.n_size() != with_defaults.p_size()) {
    std::cerr << "ERROR: Invalid number of parameters in parameter list: N"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  if (with_defaults.uv_size() != 0 &&
      with_defaults.uv_size() != with_defaults.p_size()) {
    std::cerr << "ERROR: Invalid number of parameters in parameter list: uv"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  if (with_defaults.indices().empty()) {
    return std::pair<std::vector<ReferenceCounted<Geometry>>, Matrix>(
        {}, model_to_world);
  }

  std::vector<std::tuple<uint32_t, uint32_t, uint32_t>> indices;
  for (const auto& entry : with_defaults.indices()) {
    if (entry.v0() < 0 || entry.v0() >= with_defaults.p_size() ||
        entry.v1() < 0 || entry.v1() >= with_defaults.p_size() ||
        entry.v2() < 0 || entry.v2() >= with_defaults.p_size()) {
      std::cerr << "ERROR: Out of range value for parameter: indices"
                << std::endl;
      exit(EXIT_FAILURE);
    }

    indices.emplace_back(static_cast<uint32_t>(entry.v0()),
                         static_cast<uint32_t>(entry.v1()),
                         static_cast<uint32_t>(entry.v2()));
  }

  std::vector<face_t> face_indices;
  for (const auto& face_index : with_defaults.faceindices()) {
    if (face_index < 0) {
      std::cerr << "ERROR: Out of range value for parameter: faceIndices"
                << std::endl;
      exit(EXIT_FAILURE);
    }

    face_indices.push_back(static_cast<face_t>(face_index));
  }

  std::vector<Point> points;
  for (const auto& p : with_defaults.p()) {
    Point point(static_cast<geometric>(p.x()), static_cast<geometric>(p.y()),
                static_cast<geometric>(p.z()));
    points.push_back(model_to_world.Multiply(point));
  }

  std::vector<std::tuple<geometric, geometric, geometric>> normals;
  for (const auto& n : with_defaults.n()) {
    Vector normal(static_cast<geometric>(n.x()), static_cast<geometric>(n.y()),
                  static_cast<geometric>(n.z()));
    Vector transformed = model_to_world.InverseTransposeMultiply(normal);
    normals.emplace_back(transformed.x, transformed.y, transformed.z);
  }

  std::vector<std::pair<geometric, geometric>> uvs;
  for (const auto& uv : with_defaults.uv()) {
    uvs.emplace_back(static_cast<geometric>(uv.u()),
                     static_cast<geometric>(uv.v()));
  }

  ReferenceCounted<textures::MaskTexture> alpha_mask =
      texture_manager.AllocateFloatTexture(with_defaults.alpha());
  if (!alpha_mask) {
    return std::pair<std::vector<ReferenceCounted<Geometry>>, Matrix>(
        {}, model_to_world);
  }

  std::vector<ReferenceCounted<Geometry>> triangles;
  if (model_to_world.SwapsHandedness() && normals.empty()) {
    triangles = AllocateTriangleMesh(
        points, indices, face_indices, normals, uvs, std::move(alpha_mask),
        back_material, front_material, back_emissive_material,
        front_emissive_material, back_normal_map, front_normal_map);
  } else {
    triangles = AllocateTriangleMesh(
        points, indices, face_indices, normals, uvs, std::move(alpha_mask),
        front_material, back_material, front_emissive_material,
        back_emissive_material, front_normal_map, back_normal_map);
  }

  return std::make_pair(std::move(triangles), Matrix::Identity());
}

}  // namespace shapes
}  // namespace pbrt_frontend
}  // namespace iris
