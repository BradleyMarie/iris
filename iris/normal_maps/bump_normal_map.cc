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

  visual_t displacement = bumps_->Evaluate(texture_coordinates);

  TextureCoordinates texture_coordinates_dx{
      {texture_coordinates.uv[0] + texture_coordinates.differentials->du_dx,
       texture_coordinates.uv[1] + texture_coordinates.differentials->dv_dx},
      texture_coordinates.differentials};
  visual_t displacement_x = bumps_->Evaluate(texture_coordinates_dx);

  TextureCoordinates texture_coordinates_dy{
      {texture_coordinates.uv[0] + texture_coordinates.differentials->du_dy,
       texture_coordinates.uv[1] + texture_coordinates.differentials->dv_dy},
      texture_coordinates.differentials};
  visual_t displacement_y = bumps_->Evaluate(texture_coordinates_dy);

  geometric_t duv_dx = std::sqrt(texture_coordinates.differentials->du_dx *
                                     texture_coordinates.differentials->du_dx +
                                 texture_coordinates.differentials->dv_dx *
                                     texture_coordinates.differentials->dv_dx);
  geometric_t duv_dy = std::sqrt(texture_coordinates.differentials->du_dy *
                                     texture_coordinates.differentials->du_dy +
                                 texture_coordinates.differentials->dv_dy *
                                     texture_coordinates.differentials->dv_dy);

  Vector dn_dx = differentials->dp_dx +
                 surface_normal * (displacement_x - displacement) / duv_dx;
  Vector dn_dy = differentials->dp_dy +
                 surface_normal * (displacement_y - displacement) / duv_dy;

  Vector shading_normal = Normalize(CrossProduct(dn_dy, dn_dx));

  if (DotProduct(shading_normal, surface_normal) < 0.0) {
    return -shading_normal;
  }

  return shading_normal;
}

}  // namespace normals
}  // namespace iris