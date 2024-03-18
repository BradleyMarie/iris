#include "frontends/pbrt/shapes/sphere.h"

#include "iris/geometry/sphere.h"

namespace iris::pbrt_frontend::shapes {
namespace {

static const std::unordered_map<std::string_view, Parameter::Type>
    g_parameters = {
        {"radius", Parameter::FLOAT},
};

class SphereBuilder
    : public ObjectBuilder<
          std::pair<std::vector<ReferenceCounted<Geometry>>, Matrix>,
          const ReferenceCounted<iris::Material>&,
          const ReferenceCounted<iris::Material>&,
          const ReferenceCounted<iris::NormalMap>&,
          const ReferenceCounted<iris::NormalMap>&,
          const ReferenceCounted<EmissiveMaterial>&,
          const ReferenceCounted<EmissiveMaterial>&, const Matrix&> {
 public:
  SphereBuilder() : ObjectBuilder(g_parameters) {}

  std::pair<std::vector<ReferenceCounted<Geometry>>, Matrix> Build(
      const std::unordered_map<std::string_view, Parameter>& parameters,
      const ReferenceCounted<iris::Material>& front_material,
      const ReferenceCounted<iris::Material>& back_material,
      const ReferenceCounted<iris::NormalMap>& front_normal_map,
      const ReferenceCounted<iris::NormalMap>& back_normal_map,
      const ReferenceCounted<EmissiveMaterial>& front_emissive_material,
      const ReferenceCounted<EmissiveMaterial>& back_emissive_material,
      const Matrix& model_to_world) const override;
};

std::pair<std::vector<ReferenceCounted<Geometry>>, Matrix> SphereBuilder::Build(
    const std::unordered_map<std::string_view, Parameter>& parameters,
    const ReferenceCounted<iris::Material>& front_material,
    const ReferenceCounted<iris::Material>& back_material,
    const ReferenceCounted<iris::NormalMap>& front_normal_map,
    const ReferenceCounted<iris::NormalMap>& back_normal_map,
    const ReferenceCounted<EmissiveMaterial>& front_emissive_material,
    const ReferenceCounted<EmissiveMaterial>& back_emissive_material,
    const Matrix& model_to_world) const {
  geometric radius = 1.0;

  auto radius_iter = parameters.find("radius");
  if (radius_iter != parameters.end()) {
    radius = radius_iter->second.GetFloatValues().front();
    if (!std::isfinite(radius)) {
      std::cerr << "ERROR: Out of range value for parameter: radius"
                << std::endl;
      exit(EXIT_FAILURE);
    }
  }

  auto sphere = iris::geometry::AllocateSphere(
      Point(0.0, 0.0, 0.0), radius, front_material, back_material,
      front_emissive_material, back_emissive_material, front_normal_map,
      back_normal_map);

  return std::make_pair(std::vector<ReferenceCounted<Geometry>>({sphere}),
                        model_to_world);
}

};  // namespace

extern const std::unique_ptr<const ObjectBuilder<
    std::pair<std::vector<ReferenceCounted<Geometry>>, Matrix>,
    const ReferenceCounted<iris::Material>&,
    const ReferenceCounted<iris::Material>&,
    const ReferenceCounted<iris::NormalMap>&,
    const ReferenceCounted<iris::NormalMap>&,
    const ReferenceCounted<EmissiveMaterial>&,
    const ReferenceCounted<EmissiveMaterial>&, const Matrix&>>
    g_sphere_builder = std::make_unique<SphereBuilder>();

}  // namespace iris::pbrt_frontend::shapes