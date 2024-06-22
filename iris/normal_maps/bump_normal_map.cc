#include "iris/normal_maps/bump_normal_map.h"

namespace iris {
namespace normals {
namespace {

std::pair<geometric_t, geometric_t> ComputeNormal(
    const iris::ReferenceCounted<textures::ValueTexture2D<visual>>& bumps,
    const TextureCoordinates& texture_coordinates,
    const NormalMap::Differentials& differentials) {
  visual_t displacement = bumps->Evaluate(texture_coordinates);

  geometric_t displacement0 = 0.0;
  geometric_t displacement1 = 0.0;
  if (differentials.type == NormalMap::Differentials::DU_DV) {
    geometric_t du = static_cast<geometric_t>(0.5) *
                     (std::abs(texture_coordinates.differentials->du_dx) +
                      std::abs(texture_coordinates.differentials->du_dy));

    if (du != static_cast<geometric_t>(0.0)) {
      TextureCoordinates texture_coordinates_du{
          {texture_coordinates.uv[0] + du, texture_coordinates.uv[1]},
          texture_coordinates.differentials};
      displacement0 =
          (bumps->Evaluate(texture_coordinates_du) - displacement) / du;
    }

    geometric_t dv = static_cast<geometric_t>(0.5) *
                     (std::abs(texture_coordinates.differentials->dv_dx) +
                      std::abs(texture_coordinates.differentials->dv_dy));

    if (du != static_cast<geometric_t>(0.0)) {
      TextureCoordinates texture_coordinates_dv{
          {texture_coordinates.uv[0], texture_coordinates.uv[1] + dv},
          texture_coordinates.differentials};
      displacement1 =
          (bumps->Evaluate(texture_coordinates_dv) - displacement) / dv;
    }
  } else {
    geometric_t duv_dx =
        std::sqrt(texture_coordinates.differentials->du_dx *
                      texture_coordinates.differentials->du_dx +
                  texture_coordinates.differentials->dv_dx *
                      texture_coordinates.differentials->dv_dx);

    if (duv_dx != static_cast<geometric_t>(0.0)) {
      TextureCoordinates texture_coordinates_dx{
          {texture_coordinates.uv[0] + texture_coordinates.differentials->du_dx,
           texture_coordinates.uv[1] +
               texture_coordinates.differentials->dv_dx},
          texture_coordinates.differentials};
      displacement0 =
          (bumps->Evaluate(texture_coordinates_dx) - displacement) / duv_dx;
    }

    geometric_t duv_dy =
        std::sqrt(texture_coordinates.differentials->du_dy *
                      texture_coordinates.differentials->du_dy +
                  texture_coordinates.differentials->dv_dy *
                      texture_coordinates.differentials->dv_dy);

    if (duv_dy != static_cast<geometric_t>(0.0)) {
      TextureCoordinates texture_coordinates_dy{
          {texture_coordinates.uv[0] + texture_coordinates.differentials->du_dy,
           texture_coordinates.uv[1] +
               texture_coordinates.differentials->dv_dy},
          texture_coordinates.differentials};
      displacement1 =
          (bumps->Evaluate(texture_coordinates_dy) - displacement) / duv_dy;
    }
  }

  return {displacement0, displacement1};
}

}  // namespace

Vector BumpNormalMap::Evaluate(
    const TextureCoordinates& texture_coordinates,
    const std::optional<Differentials>& differentials,
    const Vector& surface_normal) const {
  // If we instead had guaranteed values for dp_du and dp_dv we could instead
  // convert the bump map into a normal map during construction so that we could
  // always return a value from this function without falling back on the
  // surface normal.
  if (!texture_coordinates.differentials || !differentials) {
    return surface_normal;
  }

  auto result = ComputeNormal(bumps_, texture_coordinates, *differentials);
  Vector dn_dx = differentials->dp.first + surface_normal * result.first;
  Vector dn_dy = differentials->dp.second + surface_normal * result.second;
  Vector shading_normal = CrossProduct(dn_dy, dn_dx);

  return shading_normal.AlignWith(surface_normal);
}

}  // namespace normals
}  // namespace iris