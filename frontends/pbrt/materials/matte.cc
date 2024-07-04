#include "frontends/pbrt/materials/matte.h"

#include "frontends/pbrt/materials/bumpmap.h"
#include "iris/materials/matte_material.h"
#include "iris/reflectors/uniform_reflector.h"
#include "iris/textures/constant_texture.h"

namespace iris::pbrt_frontend::materials {
namespace {

static const iris::visual kDefaultReflectance = 0.5;
static const iris::visual kDefaultSigma = 0.0;

static const std::unordered_map<std::string_view, Parameter::Type>
    g_parameters = {
        {"bumpmap", Parameter::FLOAT_TEXTURE},
        {"Kd", Parameter::REFLECTOR_TEXTURE},
        {"sigma", Parameter::FLOAT_TEXTURE},
};

class MatteObjectBuilder
    : public ObjectBuilder<
          std::shared_ptr<ObjectBuilder<
              std::tuple<ReferenceCounted<Material>, ReferenceCounted<Material>,
                         ReferenceCounted<NormalMap>,
                         ReferenceCounted<NormalMap>>,
              TextureManager&>>,
          TextureManager&> {
 public:
  MatteObjectBuilder() noexcept : ObjectBuilder(g_parameters) {}

  std::shared_ptr<ObjectBuilder<
      std::tuple<ReferenceCounted<Material>, ReferenceCounted<Material>,
                 ReferenceCounted<NormalMap>, ReferenceCounted<NormalMap>>,
      TextureManager&>>
  Build(const std::unordered_map<std::string_view, Parameter>& parameters,
        TextureManager& texture_manager) const override;
};

class NestedMatteObjectBuilder
    : public ObjectBuilder<
          std::tuple<ReferenceCounted<Material>, ReferenceCounted<Material>,
                     ReferenceCounted<NormalMap>, ReferenceCounted<NormalMap>>,
          TextureManager&> {
 public:
  NestedMatteObjectBuilder(
      iris::ReferenceCounted<iris::textures::PointerTexture2D<
          iris::Reflector, iris::SpectralAllocator>>
          diffuse,
      iris::ReferenceCounted<iris::textures::ValueTexture2D<iris::visual>>
          sigma,
      iris::ReferenceCounted<iris::NormalMap> front_normal_map,
      iris::ReferenceCounted<iris::NormalMap> back_normal_map)
      : ObjectBuilder(g_parameters),
        diffuse_(std::move(diffuse)),
        sigma_(std::move(sigma)),
        default_(std::make_tuple(
            iris::MakeReferenceCounted<iris::materials::MatteMaterial>(diffuse_,
                                                                       sigma_),
            std::move(front_normal_map), std::move(back_normal_map))) {}

  std::tuple<ReferenceCounted<Material>, ReferenceCounted<Material>,
             ReferenceCounted<NormalMap>, ReferenceCounted<NormalMap>>
  Build(const std::unordered_map<std::string_view, Parameter>& parameters,
        TextureManager& texture_manager) const override;

 private:
  const iris::ReferenceCounted<iris::textures::PointerTexture2D<
      iris::Reflector, iris::SpectralAllocator>>
      diffuse_;
  const iris::ReferenceCounted<iris::textures::ValueTexture2D<iris::visual>>
      sigma_;
  const std::tuple<ReferenceCounted<Material>, ReferenceCounted<NormalMap>,
                   ReferenceCounted<NormalMap>>
      default_;
};

std::shared_ptr<ObjectBuilder<
    std::tuple<ReferenceCounted<Material>, ReferenceCounted<Material>,
               ReferenceCounted<NormalMap>, ReferenceCounted<NormalMap>>,
    TextureManager&>>
MatteObjectBuilder::Build(
    const std::unordered_map<std::string_view, Parameter>& parameters,
    TextureManager& texture_manager) const {
  ReferenceCounted<NormalMap> front_normal_map;
  ReferenceCounted<NormalMap> back_normal_map;
  auto diffuse_texture =
      texture_manager.AllocateUniformReflectorTexture(kDefaultReflectance);
  auto sigma_texture =
      texture_manager.AllocateUniformFloatTexture(kDefaultSigma);

  auto kd = parameters.find("Kd");
  if (kd != parameters.end()) {
    diffuse_texture = kd->second.GetReflectorTextures(1).front();
  }

  auto sigma = parameters.find("sigma");
  if (sigma != parameters.end()) {
    sigma_texture = sigma->second.GetFloatTextures(1).front();
  }

  auto bump = parameters.find("bumpmap");
  if (bump != parameters.end()) {
    auto normal_maps = MakeBumpMap(bump->second.GetFloatTextures(1).front());
    front_normal_map = normal_maps.first;
    back_normal_map = normal_maps.second;
  }

  return std::make_unique<NestedMatteObjectBuilder>(
      std::move(diffuse_texture), std::move(sigma_texture),
      std::move(front_normal_map), std::move(back_normal_map));
}

std::tuple<ReferenceCounted<Material>, ReferenceCounted<Material>,
           ReferenceCounted<NormalMap>, ReferenceCounted<NormalMap>>
NestedMatteObjectBuilder::Build(
    const std::unordered_map<std::string_view, Parameter>& parameters,
    TextureManager& texture_manager) const {
  if (parameters.empty()) {
    return std::make_tuple(std::get<0>(default_), std::get<0>(default_),
                           std::get<1>(default_), std::get<2>(default_));
  }

  auto diffuse_texture = diffuse_;
  auto sigma_texture = sigma_;
  auto front_normal_map = std::get<1>(default_);
  auto back_normal_map = std::get<2>(default_);

  auto kd = parameters.find("Kd");
  if (kd != parameters.end()) {
    diffuse_texture = kd->second.GetReflectorTextures(1).front();
  }

  auto sigma = parameters.find("sigma");
  if (sigma != parameters.end()) {
    sigma_texture = sigma->second.GetFloatTextures(1).front();
  }

  auto bump = parameters.find("bumpmap");
  if (bump != parameters.end()) {
    auto normal_maps = MakeBumpMap(bump->second.GetFloatTextures(1).front());
    front_normal_map = normal_maps.first;
    back_normal_map = normal_maps.second;
  }

  auto material = iris::MakeReferenceCounted<iris::materials::MatteMaterial>(
      std::move(diffuse_texture), std::move(sigma_texture));

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
    g_matte_builder = std::make_unique<MatteObjectBuilder>();

}  // namespace iris::pbrt_frontend::materials