#include "frontends/pbrt/shapes/trianglemesh.h"

#include <cmath>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <tuple>
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
#include "pbrt_proto/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace shapes {

using ::iris::geometry::AllocateTriangleMesh;
using ::pbrt_proto::TriangleMeshShape;

std::tuple<std::vector<ReferenceCounted<Geometry>>, Matrix, bool>
MakeTriangleMesh(
    const TriangleMeshShape& trianglemesh, const Matrix& model_to_world,
    const ReferenceCounted<Material>& front_material,
    const ReferenceCounted<Material>& back_material,
    const ReferenceCounted<EmissiveMaterial>& front_emissive_material,
    const ReferenceCounted<EmissiveMaterial>& back_emissive_material,
    const ReferenceCounted<NormalMap>& front_normal_map,
    const ReferenceCounted<NormalMap>& back_normal_map,
    TextureManager& texture_manager, bool reversed_orientation) {
  if (trianglemesh.n_size() != 0 &&
      trianglemesh.n_size() != trianglemesh.p_size()) {
    std::cerr << "ERROR: Invalid number of parameters in parameter list: N"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  if (trianglemesh.uv_size() != 0 &&
      trianglemesh.uv_size() != trianglemesh.p_size()) {
    std::cerr << "ERROR: Invalid number of parameters in parameter list: uv"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  if (trianglemesh.indices().empty()) {
    return std::tuple<std::vector<ReferenceCounted<Geometry>>, Matrix, bool>(
        {}, model_to_world, false);
  }

  uint32_t largest_index = 0u;
  std::vector<std::tuple<uint32_t, uint32_t, uint32_t>> indices;
  indices.reserve(trianglemesh.indices_size());
  for (const auto& entry : trianglemesh.indices()) {
    if (entry.v0() >= static_cast<uint32_t>(trianglemesh.p_size()) ||
        entry.v1() >= static_cast<uint32_t>(trianglemesh.p_size()) ||
        entry.v2() >= static_cast<uint32_t>(trianglemesh.p_size())) {
      std::cerr << "ERROR: Out of range value for parameter: indices"
                << std::endl;
      exit(EXIT_FAILURE);
    }

    largest_index = std::max(largest_index, entry.v0());
    largest_index = std::max(largest_index, entry.v1());
    largest_index = std::max(largest_index, entry.v2());

    indices.emplace_back(static_cast<uint32_t>(entry.v0()),
                         static_cast<uint32_t>(entry.v1()),
                         static_cast<uint32_t>(entry.v2()));
  }

  if (!trianglemesh.n().empty() &&
      static_cast<uint32_t>(trianglemesh.n().size()) < largest_index) {
    std::cerr << "ERROR: Too few values for parameter: n" << std::endl;
    exit(EXIT_FAILURE);
  }

  if (!trianglemesh.uv().empty() &&
      static_cast<uint32_t>(trianglemesh.uv().size()) < largest_index) {
    std::cerr << "ERROR: Too few values for parameter: uv" << std::endl;
    exit(EXIT_FAILURE);
  }

  if (!trianglemesh.faceindices().empty() &&
      trianglemesh.faceindices().size() < trianglemesh.indices().size()) {
    std::cerr << "ERROR: Too few values for parameter: faceindices"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  std::vector<face_t> face_indices;
  face_indices.reserve(trianglemesh.faceindices_size());
  for (const auto& face_index : trianglemesh.faceindices()) {
    if (face_index < 0) {
      std::cerr << "ERROR: Out of range value for parameter: faceIndices"
                << std::endl;
      exit(EXIT_FAILURE);
    }

    face_indices.emplace_back(static_cast<face_t>(face_index));
  }

  std::vector<Point> model_points;
  model_points.reserve(trianglemesh.p_size());
  std::vector<Point> world_points;
  world_points.reserve(trianglemesh.p_size());
  for (const pbrt_proto::Point& p : trianglemesh.p()) {
    model_points.emplace_back(p.x(), p.y(), p.z());
    world_points.emplace_back(model_to_world.Multiply(model_points.back()));
  }

  std::vector<Vector> model_normals;
  model_normals.reserve(trianglemesh.n_size());
  std::vector<Vector> world_normals;
  world_normals.reserve(trianglemesh.n_size());
  for (const pbrt_proto::Vector& n : trianglemesh.n()) {
    model_normals.emplace_back(n.x(), n.y(), n.z());
    world_normals.emplace_back(
        model_to_world.InverseTransposeMultiply(model_normals.back()));
  }

  std::vector<std::pair<geometric, geometric>> uvs;
  uvs.reserve(trianglemesh.uv_size());
  for (const auto& uv : trianglemesh.uv()) {
    uvs.emplace_back(static_cast<geometric>(uv.u()),
                     static_cast<geometric>(uv.v()));
  }

  ReferenceCounted<textures::MaskTexture> alpha_mask =
      texture_manager.AllocateFloatTexture(trianglemesh.alpha());
  if (trianglemesh.has_alpha() && !alpha_mask && !front_emissive_material &&
      !back_emissive_material) {
    return std::tuple<std::vector<ReferenceCounted<Geometry>>, Matrix, bool>(
        {}, model_to_world, false);
  }

  if (!world_normals.empty()) {
    for (auto [i0, i1, i2] : indices) {
      Vector surface_normal = CrossProduct(model_points[i1] - model_points[i0],
                                           model_points[i2] - model_points[i0]);
      Vector aggregate_normal =
          model_normals[i0] + model_normals[i1] + model_normals[i2];

      geometric_t cumulative_dp = DotProduct(surface_normal, aggregate_normal);
      if (cumulative_dp < static_cast<geometric_t>(0.0)) {
        std::swap(i1, i2);
      }

      if (model_to_world.SwapsHandedness() ^ reversed_orientation) {
        std::swap(i1, i2);
      }
    }
  } else if (model_to_world.SwapsHandedness()) {
    for (auto [i0, i1, i2] : indices) {
      std::swap(i1, i2);
    }
  }

  std::vector<ReferenceCounted<Geometry>> triangles = AllocateTriangleMesh(
      std::move(world_points), std::move(indices), std::move(face_indices),
      std::move(world_normals), std::move(uvs), std::move(alpha_mask),
      front_material, back_material, front_emissive_material,
      back_emissive_material, front_normal_map, back_normal_map);

  return std::make_tuple(
      std::move(triangles), Matrix::Identity(),
      !alpha_mask && (front_emissive_material || back_emissive_material));
}

}  // namespace shapes
}  // namespace pbrt_frontend
}  // namespace iris
