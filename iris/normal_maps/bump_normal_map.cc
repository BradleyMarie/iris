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

std::pair<geometric_t, geometric_t> ComputeNormal(
    const ReferenceCounted<FloatTexture>& bumps,
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

    if (dv != static_cast<geometric_t>(0.0)) {
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

class BumpNormalMap final : public NormalMap {
 public:
  BumpNormalMap(ReferenceCounted<FloatTexture> bumps)
      : bumps_(std::move(bumps)) {}

  Vector Evaluate(const TextureCoordinates& texture_coordinates,
                  const std::optional<Differentials>& Differentials,
                  const Vector& surface_normal) const override;

 private:
  ReferenceCounted<FloatTexture> bumps_;
};

Vector BumpNormalMap::Evaluate(
    const TextureCoordinates& texture_coordinates,
    const std::optional<Differentials>& differentials,
    const Vector& surface_normal) const {
  if (!bumps_) {
    return surface_normal;
  }

  // If we instead had guaranteed values for dp_du and dp_dv we could instead
  // convert the bump map into a normal map during construction so that we could
  // always return a value from this function without falling back on the
  // surface normal.
  if (!texture_coordinates.differentials || !differentials) {
    return surface_normal;
  }

  auto [displacement0, displacement1] =
      ComputeNormal(bumps_, texture_coordinates, *differentials);
  Vector dn_dx = differentials->dp.first + surface_normal * displacement0;
  Vector dn_dy = differentials->dp.second + surface_normal * displacement1;
  Vector shading_normal = CrossProduct(dn_dy, dn_dx);

  return shading_normal.AlignWith(surface_normal);
}

}  // namespace

ReferenceCounted<NormalMap> MakeBumpNormalMap(
    ReferenceCounted<FloatTexture> bumps) {
  if (!bumps) {
    return ReferenceCounted<NormalMap>();
  }

  return MakeReferenceCounted<BumpNormalMap>(std::move(bumps));
}

}  // namespace normal_maps
}  // namespace iris
