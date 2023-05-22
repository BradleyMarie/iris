#include "iris/normal_maps/bump_normal_map.h"

namespace iris {
namespace normals {

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

  auto displacement = bumps_->Evaluate(texture_coordinates);

  TextureCoordinates texture_coordinates_dx{
      {texture_coordinates.uv[0] + texture_coordinates.differentials->du_dx,
       texture_coordinates.uv[1] + texture_coordinates.differentials->dv_dx},
      texture_coordinates.differentials};
  auto displacement_dx =
      bumps_->Evaluate(texture_coordinates_dx) - displacement;

  TextureCoordinates texture_coordinates_dy{
      {texture_coordinates.uv[0] + texture_coordinates.differentials->du_dy,
       texture_coordinates.uv[1] + texture_coordinates.differentials->dv_dy},
      texture_coordinates.differentials};
  auto displacement_dy =
      bumps_->Evaluate(texture_coordinates_dy) - displacement;

  Vector dn_dx =
      Normalize(differentials->dp_dx + surface_normal * displacement_dx);
  Vector dn_dy =
      Normalize(differentials->dp_dy + surface_normal * displacement_dy);

  Vector shading_normal = Normalize(CrossProduct(dn_dx, dn_dy));

  if (DotProduct(shading_normal, surface_normal) < 0.0) {
    return -shading_normal;
  }

  return shading_normal;
}

}  // namespace normals
}  // namespace iris