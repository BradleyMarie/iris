#include "frontends/pbrt/materials/bumpmap.h"

#include "iris/normal_maps/bump_normal_map.h"

namespace iris::pbrt_frontend::materials {

class PbrtFrontBumpMap final : public NormalMap {
 public:
  PbrtFrontBumpMap(ReferenceCounted<NormalMap> bump_map)
      : bump_map_(std::move(bump_map)) {}

 private:
  static std::optional<Differentials> MakeDifferentials(
      const std::optional<Differentials>& differentials,
      const Vector& surface_normal) {
    if (!differentials) {
      return std::nullopt;
    }

    Vector dp_dy = CrossProduct(differentials->dp.first, surface_normal);
    Vector dp_dx = CrossProduct(dp_dy, surface_normal);

    return Differentials{differentials->type,
                         {dp_dx.AlignWith(differentials->dp.first),
                          dp_dy.AlignWith(differentials->dp.second)}};
  }

 public:
  Vector Evaluate(const TextureCoordinates& texture_coordinates,
                  const std::optional<Differentials>& differentials,
                  const Vector& surface_normal) const override {
    return bump_map_->Evaluate(texture_coordinates,
                               MakeDifferentials(differentials, surface_normal),
                               surface_normal);
  }

  ReferenceCounted<NormalMap> bump_map_;
};

class PbrtBackBumpMap final : public NormalMap {
 public:
  PbrtBackBumpMap(ReferenceCounted<NormalMap> front_bump_map)
      : front_bump_map_(std::move(front_bump_map)) {}

  Vector Evaluate(const TextureCoordinates& texture_coordinates,
                  const std::optional<Differentials>& differentials,
                  const Vector& surface_normal) const override {
    return -front_bump_map_->Evaluate(texture_coordinates, differentials,
                                      -surface_normal);
  }

 private:
  ReferenceCounted<NormalMap> front_bump_map_;
};

std::pair<ReferenceCounted<NormalMap>, ReferenceCounted<NormalMap>> MakeBumpMap(
    const ReferenceCounted<textures::ValueTexture2D<visual>>& texture) {
  auto bump_map = iris::MakeReferenceCounted<normals::BumpNormalMap>(texture);
  auto front_bump_map =
      iris::MakeReferenceCounted<PbrtFrontBumpMap>(std::move(bump_map));
  auto back_bump_map =
      iris::MakeReferenceCounted<PbrtBackBumpMap>(front_bump_map);
  return std::make_pair(std::move(front_bump_map), std::move(back_bump_map));
}

}  // namespace iris::pbrt_frontend::materials