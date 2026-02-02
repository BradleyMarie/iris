#include "frontends/pbrt/shapes/trianglemesh.h"

#include <cmath>
#include <cstdint>
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
#include "pbrt_proto/pbrt.pb.h"
#include "pbrt_proto/v3/v3.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace shapes {

using ::iris::geometry::AllocateTriangleMesh;
using ::pbrt_proto::v3::Shape;

std::pair<std::vector<ReferenceCounted<Geometry>>, Matrix> MakeTriangleMesh(
    const Shape::TriangleMesh& trianglemesh, const Matrix& model_to_world,
    const ReferenceCounted<Material>& front_material,
    const ReferenceCounted<Material>& back_material,
    const ReferenceCounted<EmissiveMaterial>& front_emissive_material,
    const ReferenceCounted<EmissiveMaterial>& back_emissive_material,
    const ReferenceCounted<NormalMap>& front_normal_map,
    const ReferenceCounted<NormalMap>& back_normal_map,
    TextureManager& texture_manager, bool reversed_orientation) {
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

  uint32_t largest_index = 0u;
  std::vector<std::tuple<uint32_t, uint32_t, uint32_t>> indices;
  for (const auto& entry : with_defaults.indices()) {
    if (entry.v0() >= static_cast<uint32_t>(with_defaults.p_size()) ||
        entry.v1() >= static_cast<uint32_t>(with_defaults.p_size()) ||
        entry.v2() >= static_cast<uint32_t>(with_defaults.p_size())) {
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

  if (!with_defaults.n().empty() &&
      static_cast<uint32_t>(with_defaults.n().size()) < largest_index) {
    std::cerr << "ERROR: Too few values for parameter: n" << std::endl;
    exit(EXIT_FAILURE);
  }

  if (!with_defaults.uv().empty() &&
      static_cast<uint32_t>(with_defaults.uv().size()) < largest_index) {
    std::cerr << "ERROR: Too few values for parameter: uv" << std::endl;
    exit(EXIT_FAILURE);
  }

  if (!with_defaults.faceindices().empty() &&
      with_defaults.faceindices().size() < with_defaults.indices().size()) {
    std::cerr << "ERROR: Too few values for parameter: faceindices"
              << std::endl;
    exit(EXIT_FAILURE);
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

  std::vector<Point> model_points;
  std::vector<Point> world_points;
  for (const auto& p : with_defaults.p()) {
    model_points.emplace_back(static_cast<geometric>(p.x()),
                              static_cast<geometric>(p.y()),
                              static_cast<geometric>(p.z()));
    world_points.push_back(model_to_world.Multiply(model_points.back()));
  }

  std::vector<Vector> model_normals;
  std::vector<Vector> world_normals;
  for (const auto& n : with_defaults.n()) {
    model_normals.emplace_back(static_cast<geometric>(n.x()),
                               static_cast<geometric>(n.y()),
                               static_cast<geometric>(n.z()));
    world_normals.emplace_back(
        model_to_world.InverseTransposeMultiply(model_normals.back()));
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

  if (!model_normals.empty()) {
    for (std::tuple<uint32_t, uint32_t, uint32_t>& indices : indices) {
      Vector surface_normal =
          CrossProduct(model_points[std::get<1>(indices)] -
                           model_points[std::get<0>(indices)],
                       model_points[std::get<2>(indices)] -
                           model_points[std::get<0>(indices)]);

      geometric_t cumulative_dp =
          DotProduct(surface_normal, model_normals[std::get<0>(indices)] +
                                         model_normals[std::get<1>(indices)] +
                                         model_normals[std::get<2>(indices)]);

      if (cumulative_dp < static_cast<geometric_t>(0.0)) {
        std::swap(std::get<1>(indices), std::get<2>(indices));
      }

      if (model_to_world.SwapsHandedness() ^ reversed_orientation) {
        std::swap(std::get<1>(indices), std::get<2>(indices));
      }
    }
  } else if (model_to_world.SwapsHandedness()) {
    for (std::tuple<uint32_t, uint32_t, uint32_t>& indices : indices) {
      std::swap(std::get<1>(indices), std::get<2>(indices));
    }
  }

  std::vector<ReferenceCounted<Geometry>> triangles = AllocateTriangleMesh(
      world_points, indices, face_indices, world_normals, uvs,
      std::move(alpha_mask), front_material, back_material,
      front_emissive_material, back_emissive_material, front_normal_map,
      back_normal_map);

  return std::make_pair(std::move(triangles), Matrix::Identity());
}

}  // namespace shapes
}  // namespace pbrt_frontend
}  // namespace iris
