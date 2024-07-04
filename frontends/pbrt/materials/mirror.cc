#include "frontends/pbrt/materials/mirror.h"

#include "frontends/pbrt/materials/bumpmap.h"
#include "iris/materials/mirror_material.h"
#include "iris/reflectors/uniform_reflector.h"
#include "iris/textures/constant_texture.h"

namespace iris::pbrt_frontend::materials {
namespace {

static const iris::visual kDefaultReflectance = 0.9;

static const std::unordered_map<std::string_view, Parameter::Type>
    g_parameters = {
        {"bumpmap", Parameter::FLOAT_TEXTURE},
        {"Kr", Parameter::REFLECTOR_TEXTURE},
};

class MirrorObjectBuilder
    : public ObjectBuilder<
          std::shared_ptr<ObjectBuilder<
              std::tuple<ReferenceCounted<Material>, ReferenceCounted<Material>,
                         ReferenceCounted<NormalMap>,
                         ReferenceCounted<NormalMap>>,
              TextureManager&>>,
          TextureManager&> {
 public:
  MirrorObjectBuilder() noexcept : ObjectBuilder(g_parameters) {}

  std::shared_ptr<ObjectBuilder<
      std::tuple<ReferenceCounted<Material>, ReferenceCounted<Material>,
                 ReferenceCounted<NormalMap>, ReferenceCounted<NormalMap>>,
      TextureManager&>>
  Build(const std::unordered_map<std::string_view, Parameter>& parameters,
        TextureManager& texture_manager) const override;
};

class NestedMirrorObjectBuilder
    : public ObjectBuilder<
          std::tuple<ReferenceCounted<Material>, ReferenceCounted<Material>,
                     ReferenceCounted<NormalMap>, ReferenceCounted<NormalMap>>,
          TextureManager&> {
 public:
  NestedMirrorObjectBuilder(
      iris::ReferenceCounted<iris::textures::PointerTexture2D<
          iris::Reflector, iris::SpectralAllocator>>
          reflectance,
      iris::ReferenceCounted<iris::NormalMap> front_normal_map,
      iris::ReferenceCounted<iris::NormalMap> back_normal_map)
      : ObjectBuilder(g_parameters),
        reflectance_(std::move(reflectance)),
        default_(std::make_tuple(
            iris::MakeReferenceCounted<iris::materials::MirrorMaterial>(
                reflectance_),
            std::move(front_normal_map), std::move(back_normal_map))) {}

  std::tuple<ReferenceCounted<Material>, ReferenceCounted<Material>,
             ReferenceCounted<NormalMap>, ReferenceCounted<NormalMap>>
  Build(const std::unordered_map<std::string_view, Parameter>& parameters,
        TextureManager& texture_manager) const override;

 private:
  const iris::ReferenceCounted<iris::textures::PointerTexture2D<
      iris::Reflector, iris::SpectralAllocator>>
      reflectance_;
  const std::tuple<ReferenceCounted<Material>, ReferenceCounted<NormalMap>,
                   ReferenceCounted<NormalMap>>
      default_;
};

std::shared_ptr<ObjectBuilder<
    std::tuple<ReferenceCounted<Material>, ReferenceCounted<Material>,
               ReferenceCounted<NormalMap>, ReferenceCounted<NormalMap>>,
    TextureManager&>>
MirrorObjectBuilder::Build(
    const std::unordered_map<std::string_view, Parameter>& parameters,
    TextureManager& texture_manager) const {
  ReferenceCounted<NormalMap> front_normal_map;
  ReferenceCounted<NormalMap> back_normal_map;
  auto reflectance_texture =
      texture_manager.AllocateUniformReflectorTexture(kDefaultReflectance);

  auto kr = parameters.find("Kr");
  if (kr != parameters.end()) {
    reflectance_texture = kr->second.GetReflectorTextures(1).front();
  }

  auto bump = parameters.find("bumpmap");
  if (bump != parameters.end()) {
    auto normal_maps = MakeBumpMap(bump->second.GetFloatTextures(1).front());
    front_normal_map = normal_maps.first;
    back_normal_map = normal_maps.second;
  }

  return std::make_unique<NestedMirrorObjectBuilder>(
      std::move(reflectance_texture), std::move(front_normal_map),
      std::move(back_normal_map));
}

std::tuple<ReferenceCounted<Material>, ReferenceCounted<Material>,
           ReferenceCounted<NormalMap>, ReferenceCounted<NormalMap>>
NestedMirrorObjectBuilder::Build(
    const std::unordered_map<std::string_view, Parameter>& parameters,
    TextureManager& texture_manager) const {
  if (parameters.empty()) {
    return std::make_tuple(std::get<0>(default_), std::get<0>(default_),
                           std::get<1>(default_), std::get<2>(default_));
  }

  auto reflectance_texture = reflectance_;
  auto front_normal_map = std::get<1>(default_);
  auto back_normal_map = std::get<2>(default_);

  auto kr = parameters.find("Kr");
  if (kr != parameters.end()) {
    reflectance_texture = kr->second.GetReflectorTextures(1).front();
  }

  auto bump = parameters.find("bumpmap");
  if (bump != parameters.end()) {
    auto normal_maps = MakeBumpMap(bump->second.GetFloatTextures(1).front());
    front_normal_map = normal_maps.first;
    back_normal_map = normal_maps.second;
  }

  auto material = iris::MakeReferenceCounted<iris::materials::MirrorMaterial>(
      std::move(reflectance_texture));

  return std::make_tuple(material, material, std::move(front_normal_map),
                         std::move(back_normal_map));
}

}  // namespace

const std::unique_ptr<const ObjectBuilder<
    std::shared_ptr<ObjectBuilder<
        std::tuple<ReferenceCounted<Material>, ReferenceCounted<Material>,
                   ReferenceCounted<NormalMap>, ReferenceCounted<NormalMap>>,
        TextureManager&>>,
    TextureManager&>>
    g_mirror_builder = std::make_unique<MirrorObjectBuilder>();

}  // namespace iris::pbrt_frontend::materials