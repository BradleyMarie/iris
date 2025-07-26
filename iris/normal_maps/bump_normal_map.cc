#include "iris/normal_maps/bump_normal_map.h"

#include <cmath>
#include <optional>
#include <utility>

#include "iris/float.h"
#include "iris/normal_map.h"
#include "iris/reference_counted.h"
#include "iris/texture_coordinates.h"
#include "iris/textures/float_texture.h"
#include "iris/vector.h"

namespace iris {
namespace normal_maps {
namespace {

using ::iris::textures::FloatTexture;

Vector ComputeNormal(const ReferenceCounted<FloatTexture>& bump_map,
                     const TextureCoordinates& texture_coordinates,
                     const Vector& dp_du, const Vector& dp_dv, geometric_t du,
                     geometric_t dv, const Vector& surface_normal) {
  visual_t displacement = bump_map->Evaluate(texture_coordinates);

  geometric_t displacement0 = 0.0;
  if (du != static_cast<geometric_t>(0.0)) {
    TextureCoordinates texture_coordinates_du{
        texture_coordinates.p + dp_du * du,
        texture_coordinates.dp_dx,
        texture_coordinates.dp_dy,
        {texture_coordinates.uv[0] + du, texture_coordinates.uv[1]},
        texture_coordinates.du_dx,
        texture_coordinates.du_dy,
        texture_coordinates.dv_dx,
        texture_coordinates.du_dy};

    displacement0 = bump_map->Evaluate(texture_coordinates_du) - displacement;
    displacement0 /= du;
  }

  geometric_t displacement1 = 0.0;
  if (dv != static_cast<geometric_t>(0.0)) {
    TextureCoordinates texture_coordinates_dv{
        texture_coordinates.p + dp_dv * dv,
        texture_coordinates.dp_dx,
        texture_coordinates.dp_dy,
        {texture_coordinates.uv[0], texture_coordinates.uv[1] + dv},
        texture_coordinates.du_dx,
        texture_coordinates.du_dy,
        texture_coordinates.dv_dx,
        texture_coordinates.du_dy};

    displacement1 = bump_map->Evaluate(texture_coordinates_dv) - displacement;
    displacement1 /= dv;
  }

  Vector offset_dp_du = dp_du + surface_normal * displacement0;
  Vector offset_dp_dv = dp_dv + surface_normal * displacement1;
  Vector shading_normal = CrossProduct(offset_dp_du, offset_dp_dv);

  return shading_normal.AlignWith(surface_normal);
}

Vector ComputeNormal(const ReferenceCounted<FloatTexture>& bump_map,
                     const TextureCoordinates& texture_coordinates,
                     const NormalMap::Differentials& differentials,
                     const Vector& surface_normal) {
  if (differentials.type == NormalMap::Differentials::DX_DY) {
    auto [dp_du, dp_dv] = CoordinateSystem(surface_normal);
    return ComputeNormal(bump_map, texture_coordinates, dp_du, dp_dv,
                         static_cast<geometric_t>(0.0005),
                         static_cast<geometric_t>(0.0005), surface_normal);
  }

  geometric_t du =
      static_cast<geometric_t>(0.5) * (std::abs(texture_coordinates.du_dx) +
                                       std::abs(texture_coordinates.du_dy));
  geometric_t dv =
      static_cast<geometric_t>(0.5) * (std::abs(texture_coordinates.dv_dx) +
                                       std::abs(texture_coordinates.dv_dy));

  bool first_is_zero = differentials.dp.first.IsZero();
  bool second_is_zero = differentials.dp.second.IsZero();
  if (first_is_zero && second_is_zero) {
    return surface_normal;
  }

  if (first_is_zero) {
    du = static_cast<geometric_t>(0.0);
  }

  if (second_is_zero) {
    dv = static_cast<geometric_t>(0.0);
  }

  Vector dp_du = first_is_zero
                     ? CrossProduct(surface_normal, differentials.dp.second)
                     : differentials.dp.first;
  Vector dp_dv = second_is_zero
                     ? CrossProduct(surface_normal, differentials.dp.first)
                     : differentials.dp.second;

  return ComputeNormal(bump_map, texture_coordinates, dp_du, dp_dv, du, dv,
                       surface_normal);
}

class BumpNormalMap final : public NormalMap {
 public:
  BumpNormalMap(ReferenceCounted<FloatTexture> bump_map)
      : bump_map_(std::move(bump_map)) {}

  Vector Evaluate(const TextureCoordinates& texture_coordinates,
                  const std::optional<Differentials>& Differentials,
                  const Vector& surface_normal) const override;

 private:
  ReferenceCounted<FloatTexture> bump_map_;
};

Vector BumpNormalMap::Evaluate(
    const TextureCoordinates& texture_coordinates,
    const std::optional<Differentials>& differentials,
    const Vector& surface_normal) const {
  if (!bump_map_ || !differentials) {
    return surface_normal;
  }

  return ComputeNormal(bump_map_, texture_coordinates, *differentials,
                       surface_normal);
}

}  // namespace

ReferenceCounted<NormalMap> MakeBumpNormalMap(
    ReferenceCounted<FloatTexture> bump_map) {
  if (!bump_map) {
    return ReferenceCounted<NormalMap>();
  }

  return MakeReferenceCounted<BumpNormalMap>(std::move(bump_map));
}

}  // namespace normal_maps
}  // namespace iris
