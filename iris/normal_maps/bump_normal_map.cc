#include "iris/normal_maps/bump_normal_map.h"

namespace iris {
namespace normals {
namespace {

std::pair<geometric_t, geometric_t> ComputeNormal(
    const iris::ReferenceCounted<textures::ValueTexture2D<visual>>& bumps,
    const TextureCoordinates& texture_coordinates,
    const NormalMap::Differentials& differentials) {
  visual_t displacement = bumps->Evaluate(texture_coordinates);

  if (differentials.type == NormalMap::Differentials::DU_DV) {
    geometric_t du = static_cast<geometric_t>(0.5) *
                     (std::abs(texture_coordinates.differentials->du_dx) +
                      std::abs(texture_coordinates.differentials->du_dy));
    geometric_t dv = static_cast<geometric_t>(0.5) *
                     (std::abs(texture_coordinates.differentials->dv_dx) +
                      std::abs(texture_coordinates.differentials->dv_dy));

    TextureCoordinates texture_coordinates_du{
        {texture_coordinates.uv[0] + du, texture_coordinates.uv[1]},
        texture_coordinates.differentials};
    visual_t displacement_u = bumps->Evaluate(texture_coordinates_du);

    TextureCoordinates texture_coordinates_dv{
        {texture_coordinates.uv[0], texture_coordinates.uv[1] + dv},
        texture_coordinates.differentials};
    visual_t displacement_v = bumps->Evaluate(texture_coordinates_dv);

    return {(displacement_u - displacement) / du,
            (displacement_v - displacement) / dv};
  }

  TextureCoordinates texture_coordinates_dx{
      {texture_coordinates.uv[0] + texture_coordinates.differentials->du_dx,
       texture_coordinates.uv[1] + texture_coordinates.differentials->dv_dx},
      texture_coordinates.differentials};
  visual_t displacement_x = bumps->Evaluate(texture_coordinates_dx);

  TextureCoordinates texture_coordinates_dy{
      {texture_coordinates.uv[0] + texture_coordinates.differentials->du_dy,
       texture_coordinates.uv[1] + texture_coordinates.differentials->dv_dy},
      texture_coordinates.differentials};
  visual_t displacement_y = bumps->Evaluate(texture_coordinates_dy);

  geometric_t duv_dx = std::sqrt(texture_coordinates.differentials->du_dx *
                                     texture_coordinates.differentials->du_dx +
                                 texture_coordinates.differentials->dv_dx *
                                     texture_coordinates.differentials->dv_dx);
  geometric_t duv_dy = std::sqrt(texture_coordinates.differentials->du_dy *
                                     texture_coordinates.differentials->du_dy +
                                 texture_coordinates.differentials->dv_dy *
                                     texture_coordinates.differentials->dv_dy);

  return {(displacement_x - displacement) / duv_dx,
          (displacement_y - displacement) / duv_dy};
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
  if (DotProduct(shading_normal, surface_normal) < 0.0) {
    return -shading_normal;
  }

  return shading_normal;
}

}  // namespace normals
}  // namespace iris