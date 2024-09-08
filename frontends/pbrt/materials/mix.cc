#include "frontends/pbrt/materials/mix.h"

#include "frontends/pbrt/material_manager.h"
#include "frontends/pbrt/materials/bumpmap.h"
#include "iris/materials/mix_material.h"

namespace iris {
namespace pbrt_frontend {
namespace materials {
namespace {

using ::iris::materials::MixMaterial;
using ::iris::textures::ValueTexture2D;

constexpr visual kDefaultAmount = 0.5;

static const std::unordered_map<std::string_view, Parameter::Type>
    g_parameters = {
        {"amount", Parameter::FLOAT_TEXTURE},
        {"bumpmap", Parameter::FLOAT_TEXTURE},
        {"namedmaterial1", Parameter::STRING},
        {"namedmaterial2", Parameter::STRING},
};

class MixObjectBuilder : public MaterialBuilder {
 public:
  MixObjectBuilder() noexcept : ObjectBuilder(g_parameters) {}

  std::shared_ptr<NestedMaterialBuilder> Build(
      const std::unordered_map<std::string_view, Parameter>& parameters,
      const MaterialManager&, TextureManager& texture_manager) const override;
};

class NestedMixObjectBuilder : public NestedMaterialBuilder {
 public:
  NestedMixObjectBuilder(ReferenceCounted<Material> m1_front,
                         ReferenceCounted<Material> m1_back,
                         ReferenceCounted<Material> m2_front,
                         ReferenceCounted<Material> m2_back,
                         ReferenceCounted<ValueTexture2D<visual>> amount,
                         ReferenceCounted<NormalMap> front_normal_map,
                         ReferenceCounted<NormalMap> back_normal_map)
      : ObjectBuilder(g_parameters),
        m1_front_(std::move(m1_front)),
        m1_back_(std::move(m1_back)),
        m2_front_(std::move(m2_front)),
        m2_back_(std::move(m2_back)),
        amount_(std::move(amount)),
        default_(std::make_tuple(
            MakeReferenceCounted<MixMaterial>(m1_front_, m2_front_, amount_),
            MakeReferenceCounted<MixMaterial>(m1_back_, m2_back_, amount_),
            std::move(front_normal_map), std::move(back_normal_map))) {}

  MaterialBuilderResult Build(
      const std::unordered_map<std::string_view, Parameter>& parameters,
      const MaterialManager&, TextureManager& texture_manager) const override;

 private:
  const ReferenceCounted<Material> m1_front_;
  const ReferenceCounted<Material> m1_back_;
  const ReferenceCounted<Material> m2_front_;
  const ReferenceCounted<Material> m2_back_;
  const ReferenceCounted<ValueTexture2D<visual>> amount_;
  const MaterialBuilderResult default_;
};

std::shared_ptr<NestedMaterialBuilder> MixObjectBuilder::Build(
    const std::unordered_map<std::string_view, Parameter>& parameters,
    const MaterialManager& material_manager,
    TextureManager& texture_manager) const {
  ReferenceCounted<NormalMap> front_normal_map;
  ReferenceCounted<NormalMap> back_normal_map;
  ReferenceCounted<ValueTexture2D<visual>> amount_texture =
      texture_manager.AllocateUniformFloatTexture(kDefaultAmount);

  auto name1 = parameters.find("namedmaterial1");
  if (name1 == parameters.end()) {
    std::cerr << "ERROR: Missing required mix parameter: namedmaterial1"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  auto name2 = parameters.find("namedmaterial2");
  if (name2 == parameters.end()) {
    std::cerr << "ERROR: Missing required mix parameter: namedmaterial2"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  std::shared_ptr<NestedMaterialBuilder> material1_builder =
      material_manager.Get(name1->second.GetStringValues(1).front());
  MaterialBuilderResult material1 =
      material1_builder->Build({}, material_manager, texture_manager);

  std::shared_ptr<NestedMaterialBuilder> material2_builder =
      material_manager.Get(name2->second.GetStringValues(1).front());
  MaterialBuilderResult material2 =
      material2_builder->Build({}, material_manager, texture_manager);

  auto amount = parameters.find("amount");
  if (amount != parameters.end()) {
    amount_texture = amount->second.GetFloatTextures(1).front();
  }

  auto bump = parameters.find("bumpmap");
  if (bump != parameters.end()) {
    std::pair<ReferenceCounted<NormalMap>, ReferenceCounted<NormalMap>>
        normal_maps = MakeBumpMap(bump->second.GetFloatTextures(1).front());
    front_normal_map = normal_maps.first;
    back_normal_map = normal_maps.second;
  }

  return std::make_unique<NestedMixObjectBuilder>(
      std::move(std::get<0>(material1)), std::move(std::get<0>(material2)),
      std::move(std::get<1>(material1)), std::move(std::get<1>(material2)),
      std::move(amount_texture), std::move(front_normal_map),
      std::move(back_normal_map));
}

MaterialBuilderResult NestedMixObjectBuilder::Build(
    const std::unordered_map<std::string_view, Parameter>& parameters,
    const MaterialManager& material_manager,
    TextureManager& texture_manager) const {
  if (parameters.empty()) {
    return std::make_tuple(std::get<0>(default_), std::get<1>(default_),
                           std::get<2>(default_), std::get<3>(default_));
  }

  ReferenceCounted<Material> material1_front = m1_front_;
  ReferenceCounted<Material> material1_back = m1_back_;
  ReferenceCounted<Material> material2_front = m2_front_;
  ReferenceCounted<Material> material2_back = m2_back_;
  ReferenceCounted<ValueTexture2D<visual>> amount_texture = amount_;
  ReferenceCounted<NormalMap> front_normal_map = std::get<2>(default_);
  ReferenceCounted<NormalMap> back_normal_map = std::get<3>(default_);

  auto name1 = parameters.find("namedmaterial1");
  if (name1 != parameters.end()) {
    std::shared_ptr<NestedMaterialBuilder> material1_builder =
        material_manager.Get(name1->second.GetStringValues(1).front());
    MaterialBuilderResult material1 =
        material1_builder->Build({}, material_manager, texture_manager);
    material1_front = std::move(std::get<0>(material1));
    material1_back = std::move(std::get<1>(material1));
  }

  auto name2 = parameters.find("namedmaterial2");
  if (name2 != parameters.end()) {
    std::shared_ptr<NestedMaterialBuilder> material2_builder =
        material_manager.Get(name2->second.GetStringValues(1).front());
    MaterialBuilderResult material2 =
        material2_builder->Build({}, material_manager, texture_manager);
    material2_front = std::move(std::get<0>(material2));
    material2_back = std::move(std::get<1>(material2));
  }

  auto amount = parameters.find("amount");
  if (amount != parameters.end()) {
    amount_texture = amount->second.GetFloatTextures(1).front();
  }

  auto bump = parameters.find("bumpmap");
  if (bump != parameters.end()) {
    std::pair<ReferenceCounted<NormalMap>, ReferenceCounted<NormalMap>>
        normal_maps = MakeBumpMap(bump->second.GetFloatTextures(1).front());
    front_normal_map = normal_maps.first;
    back_normal_map = normal_maps.second;
  }

  ReferenceCounted<Material> front_material = MakeReferenceCounted<MixMaterial>(
      std::move(material1_front), std::move(material2_front), amount_texture);
  ReferenceCounted<Material> back_material = MakeReferenceCounted<MixMaterial>(
      std::move(material1_back), std::move(material2_back),
      std::move(amount_texture));

  return std::make_tuple(std::move(front_material), std::move(back_material),
                         std::move(front_normal_map),
                         std::move(back_normal_map));
}

}  // namespace

const std::unique_ptr<const MaterialBuilder> g_mix_builder =
    std::make_unique<MixObjectBuilder>();

}  // namespace materials
}  // namespace pbrt_frontend
}  // namespace iris