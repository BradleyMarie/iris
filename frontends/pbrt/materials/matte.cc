#include "frontends/pbrt/materials/matte.h"

#include "iris/materials/matte_material.h"
#include "iris/normal_maps/bump_normal_map.h"
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
          std::shared_ptr<ObjectBuilder<std::pair<ReferenceCounted<Material>,
                                                  ReferenceCounted<NormalMap>>,
                                        TextureManager&>>,
          TextureManager&> {
 public:
  MatteObjectBuilder() noexcept : ObjectBuilder(g_parameters) {}

  std::shared_ptr<ObjectBuilder<
      std::pair<ReferenceCounted<Material>, ReferenceCounted<NormalMap>>,
      TextureManager&>>
  Build(const std::unordered_map<std::string_view, Parameter>& parameters,
        TextureManager& texture_manager) const override;
};

class NestedMatteObjectBuilder
    : public ObjectBuilder<
          std::pair<ReferenceCounted<Material>, ReferenceCounted<NormalMap>>,
          TextureManager&> {
 public:
  NestedMatteObjectBuilder(
      iris::ReferenceCounted<iris::textures::PointerTexture2D<
          iris::Reflector, iris::SpectralAllocator>>
          diffuse,
      iris::ReferenceCounted<iris::textures::ValueTexture2D<iris::visual>>
          sigma,
      iris::ReferenceCounted<iris::NormalMap> bump)
      : ObjectBuilder(g_parameters),
        diffuse_(std::move(diffuse)),
        sigma_(std::move(sigma)),
        default_(std::make_pair(
            iris::MakeReferenceCounted<iris::materials::MatteMaterial>(diffuse_,
                                                                       sigma_),
            bump)) {}

  std::pair<ReferenceCounted<Material>, ReferenceCounted<NormalMap>> Build(
      const std::unordered_map<std::string_view, Parameter>& parameters,
      TextureManager& texture_manager) const override;

 private:
  iris::ReferenceCounted<iris::textures::PointerTexture2D<
      iris::Reflector, iris::SpectralAllocator>>
      diffuse_;
  iris::ReferenceCounted<iris::textures::ValueTexture2D<iris::visual>> sigma_;
  std::pair<ReferenceCounted<Material>, ReferenceCounted<NormalMap>> default_;
};

std::shared_ptr<ObjectBuilder<
    std::pair<ReferenceCounted<Material>, ReferenceCounted<NormalMap>>,
    TextureManager&>>
MatteObjectBuilder::Build(
    const std::unordered_map<std::string_view, Parameter>& parameters,
    TextureManager& texture_manager) const {
  ReferenceCounted<NormalMap> normal_map;
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
    normal_map = iris::MakeReferenceCounted<normals::BumpNormalMap>(
        bump->second.GetFloatTextures(1).front());
  }

  return std::make_unique<NestedMatteObjectBuilder>(std::move(diffuse_texture),
                                                    std::move(sigma_texture),
                                                    std::move(normal_map));
}

std::pair<ReferenceCounted<Material>, ReferenceCounted<NormalMap>>
NestedMatteObjectBuilder::Build(
    const std::unordered_map<std::string_view, Parameter>& parameters,
    TextureManager& texture_manager) const {
  if (parameters.empty()) {
    return default_;
  }

  auto diffuse_texture = diffuse_;
  auto sigma_texture = sigma_;
  auto normal_map = default_.second;

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
    normal_map = iris::MakeReferenceCounted<normals::BumpNormalMap>(
        bump->second.GetFloatTextures(1).front());
  }

  return std::make_pair(
      iris::MakeReferenceCounted<iris::materials::MatteMaterial>(
          std::move(diffuse_texture), std::move(sigma_texture)),
      std::move(normal_map));
}

}  // namespace

const std::unique_ptr<const ObjectBuilder<
    std::shared_ptr<ObjectBuilder<
        std::pair<ReferenceCounted<Material>, ReferenceCounted<NormalMap>>,
        TextureManager&>>,
    TextureManager&>>
    g_matte_builder = std::make_unique<MatteObjectBuilder>();

}  // namespace iris::pbrt_frontend::materials