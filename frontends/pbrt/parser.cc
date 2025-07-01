#include "frontends/pbrt/parser.h"

#include <cstdlib>
#include <filesystem>
#include <functional>
#include <iostream>
#include <memory>
#include <stack>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

#include "frontends/pbrt/area_lights/parse.h"
#include "frontends/pbrt/cameras/parse.h"
#include "frontends/pbrt/defaults.h"
#include "frontends/pbrt/file.h"
#include "frontends/pbrt/film/parse.h"
#include "frontends/pbrt/film/result.h"
#include "frontends/pbrt/image_manager.h"
#include "frontends/pbrt/integrators/parse.h"
#include "frontends/pbrt/integrators/result.h"
#include "frontends/pbrt/lights/parse.h"
#include "frontends/pbrt/material_manager.h"
#include "frontends/pbrt/materials/parse.h"
#include "frontends/pbrt/materials/result.h"
#include "frontends/pbrt/matrix_manager.h"
#include "frontends/pbrt/renderable.h"
#include "frontends/pbrt/samplers/parse.h"
#include "frontends/pbrt/shapes/parse.h"
#include "frontends/pbrt/spectrum_managers/color_spectrum_manager.h"
#include "frontends/pbrt/texture_manager.h"
#include "frontends/pbrt/textures/parse.h"
#include "iris/camera.h"
#include "iris/emissive_material.h"
#include "iris/environmental_light.h"
#include "iris/float.h"
#include "iris/geometry/bvh_aggregate.h"
#include "iris/image_sampler.h"
#include "iris/light.h"
#include "iris/material.h"
#include "iris/reference_counted.h"
#include "iris/renderer.h"
#include "iris/scene.h"
#include "iris/scene_objects.h"
#include "iris/scenes/bvh_scene.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace {

using ::iris::geometry::AllocateBVHAggregate;
using ::iris::pbrt_frontend::spectrum_managers::ColorAlbedoMatcher;
using ::iris::pbrt_frontend::spectrum_managers::ColorColorMatcher;
using ::iris::pbrt_frontend::spectrum_managers::ColorPowerMatcher;
using ::iris::pbrt_frontend::spectrum_managers::ColorSpectrumManager;
using ::iris::scenes::MakeBVHSceneBuilder;
using ::pbrt_proto::v3::ActiveTransform;
using ::pbrt_proto::v3::Directive;
using ::pbrt_proto::v3::Shape;

struct GraphicsState {
  std::array<ReferenceCounted<EmissiveMaterial>, 2> emissive_materials;
  std::pair<pbrt_proto::v3::Material, MaterialResult> material;
  bool reverse_orientation = false;
};

struct State {
  State(Directives& directives, const std::filesystem::path& search_root,
        bool always_reflective)
      : spectrum_manager(search_root, always_reflective),
        texture_manager(spectrum_manager),
        image_manager(search_root, texture_manager, spectrum_manager),
        directives_(directives) {
    graphics.emplace();
    graphics.top().material.first = Defaults().default_material();
    graphics.top().material.second =
        ParseMaterial(Defaults().default_material(),
                      Shape::MaterialOverrides::default_instance(), search_root,
                      material_manager, texture_manager, spectrum_manager);
  }

  void Include(const std::filesystem::path& search_root,
               const std::string& path);
  void LightSource(const pbrt_proto::v3::LightSource& light_source,
                   const std::filesystem::path& search_root);
  void Shape(const pbrt_proto::v3::Shape& shape,
             const std::filesystem::path& search_root);
  ParsingResult WorldEnd();

  // Various Managers
  ColorSpectrumManager spectrum_manager;
  TextureManager texture_manager;
  ImageManager image_manager;
  MatrixManager matrix_manager;
  MaterialManager material_manager;
  SceneObjects::Builder objects;

  // Object Instancing
  std::vector<ReferenceCounted<Geometry>> current_instance;
  std::string current_instance_name;
  std::unordered_map<std::string, ReferenceCounted<Geometry>> instances;
  bool build_instance = false;

  // Other State
  std::stack<GraphicsState> graphics;
  std::function<std::unique_ptr<Camera>(const std::pair<size_t, size_t>&)>
      camera;
  std::unique_ptr<FilmResult> film;
  std::unique_ptr<IntegratorResult> integrator;
  std::unique_ptr<ImageSampler> sampler;

  // Parsing State
  bool world_begin_encountered = false;

 private:
  Directives& directives_;
};

void State::Include(const std::filesystem::path& search_root,
                    const std::string& path) {
  auto [proto, file_path] = LoadFile(search_root, path);
  directives_.Include(std::move(proto), std::move(file_path));
}

void State::LightSource(const pbrt_proto::v3::LightSource& light_source,
                        const std::filesystem::path& search_root) {
  std::variant<ReferenceCounted<Light>, ReferenceCounted<EnvironmentalLight>>
      light = ParseLightSource(light_source, search_root,
                               matrix_manager.Get().start, spectrum_manager);
  if (std::holds_alternative<ReferenceCounted<EnvironmentalLight>>(light)) {
    // TODO: Merge environmental lights
    objects.Set(std::get<ReferenceCounted<EnvironmentalLight>>(light));
  } else {
    objects.Add(std::get<ReferenceCounted<Light>>(light));
  }
}

void State::Shape(const pbrt_proto::v3::Shape& shape,
                  const std::filesystem::path& search_root) {
  auto [shapes, model_to_world] = ParseShape(
      shape, matrix_manager.Get().start, graphics.top().reverse_orientation,
      graphics.top().material.first, graphics.top().material.second,
      build_instance ? std::array<ReferenceCounted<EmissiveMaterial>, 2>()
                     : graphics.top().emissive_materials,
      search_root, material_manager, texture_manager, spectrum_manager);

  if (build_instance) {
    current_instance.insert(current_instance.end(), shapes.begin(),
                            shapes.end());
  } else {
    for (auto& shape : shapes) {
      objects.Add(std::move(shape), model_to_world);
    }
  }
}

ParsingResult State::WorldEnd() {
  std::unique_ptr<Scene::Builder> scene_builder = MakeBVHSceneBuilder();
  Renderer renderer(*scene_builder, *integrator->light_scene_builder,
                    objects.Build(), ColorPowerMatcher());

  Renderable renderable(
      std::move(renderer), camera(film->resolution), std::move(sampler),
      std::move(integrator->integrator), std::make_unique<ColorAlbedoMatcher>(),
      std::make_unique<ColorColorMatcher>(), film->resolution);

  return ParsingResult{std::move(renderable), film->skip_pixel_function,
                       film->filename, film->write_function,
                       film->max_sample_luminance};
}

MatrixManager::ActiveTransformation ToActiveTransform(
    const ActiveTransform& active_transform) {
  MatrixManager::ActiveTransformation result =
      MatrixManager::ActiveTransformation::ALL;
  switch (active_transform.active()) {
    case ActiveTransform::ALL:
      result = MatrixManager::ActiveTransformation::ALL;
      break;
    case ActiveTransform::START_TIME:
      result = MatrixManager::ActiveTransformation::START;
      break;
    case ActiveTransform::END_TIME:
      result = MatrixManager::ActiveTransformation::END;
      break;
  }
  return result;
}

std::optional<ParsingResult> ParseDirective(
    State& state, const Directive& directive,
    const std::filesystem::path& search_root) {
  switch (directive.directive_type_case()) {
    case Directive::kAccelerator:
      if (state.world_begin_encountered) {
        std::cerr
            << "ERROR: Directive cannot be specified between WorldBegin and "
               "WorldEnd: Accelerator"
            << std::endl;
        exit(EXIT_FAILURE);
      }

      // Ignored
      break;
    case Directive::kActiveTransform:
      state.matrix_manager.ActiveTransform(
          ToActiveTransform(directive.active_transform()));
      break;
    case Directive::kAreaLightSource:
      if (!state.world_begin_encountered) {
        std::cerr << "ERROR: Directive cannot be specified before WorldBegin: "
                     "AreaLightSource"
                  << std::endl;
        exit(EXIT_FAILURE);
      }

      state.graphics.top().emissive_materials = ParseAreaLightSource(
          directive.area_light_source(), state.spectrum_manager);
      break;
    case Directive::kAttributeBegin:
      if (!state.world_begin_encountered) {
        std::cerr << "ERROR: Directive cannot be specified before WorldBegin: "
                     "AttributeBegin"
                  << std::endl;
        exit(EXIT_FAILURE);
      }

      state.texture_manager.AttributeBegin();
      state.material_manager.AttributeBegin();
      state.matrix_manager.AttributeBegin();
      state.graphics.push(state.graphics.top());
      break;
    case Directive::kAttributeEnd:
      if (!state.world_begin_encountered) {
        std::cerr << "ERROR: Directive cannot be specified before WorldBegin: "
                     "AttributeEnd"
                  << std::endl;
        exit(EXIT_FAILURE);
      }

      state.texture_manager.AttributeEnd();
      state.material_manager.AttributeEnd();
      state.matrix_manager.AttributeEnd();
      state.graphics.pop();
      break;
    case Directive::kCamera:
      if (state.world_begin_encountered) {
        std::cerr
            << "ERROR: Directive cannot be specified between WorldBegin and "
               "WorldEnd: Camera"
            << std::endl;
        exit(EXIT_FAILURE);
      }

      state.camera =
          ParseCamera(directive.camera(), state.matrix_manager.Get());
      break;
    case Directive::kConcatTransform:
      state.matrix_manager.ConcatTransform(directive.concat_transform().m00(),
                                           directive.concat_transform().m01(),
                                           directive.concat_transform().m02(),
                                           directive.concat_transform().m03(),
                                           directive.concat_transform().m10(),
                                           directive.concat_transform().m11(),
                                           directive.concat_transform().m12(),
                                           directive.concat_transform().m13(),
                                           directive.concat_transform().m20(),
                                           directive.concat_transform().m21(),
                                           directive.concat_transform().m22(),
                                           directive.concat_transform().m23(),
                                           directive.concat_transform().m30(),
                                           directive.concat_transform().m31(),
                                           directive.concat_transform().m32(),
                                           directive.concat_transform().m33());
      break;
    case Directive::kCoordinateSystem:
      state.matrix_manager.CoordinateSystem(
          directive.coordinate_system().name());
      break;
    case Directive::kCoordSysTransform:
      state.matrix_manager.CoordSysTransform(
          directive.coord_sys_transform().name());
      break;
    case Directive::kFilm:
      if (state.world_begin_encountered) {
        std::cerr
            << "ERROR: Directive cannot be specified between WorldBegin and "
               "WorldEnd: Film"
            << std::endl;
        exit(EXIT_FAILURE);
      }

      state.film = ParseFilm(directive.film());
      break;
    case Directive::kFloatTexture:
      if (!state.world_begin_encountered) {
        std::cerr << "ERROR: Directive cannot be specified before WorldBegin: "
                     "Texture"
                  << std::endl;
        exit(EXIT_FAILURE);
      }

      ParseFloatTexture(directive.float_texture(), state.image_manager,
                        state.texture_manager);
      break;
    case Directive::kIdentity:
      state.matrix_manager.Identity();
      break;
    case Directive::kInclude:
      state.Include(search_root, directive.include().path());
      break;
    case Directive::kIntegrator:
      if (state.world_begin_encountered) {
        std::cerr
            << "ERROR: Directive cannot be specified between WorldBegin and "
               "WorldEnd: Integrator"
            << std::endl;
        exit(EXIT_FAILURE);
      }

      state.integrator = ParseIntegrator(directive.integrator());
      break;
    case Directive::kLightSource:
      if (!state.world_begin_encountered) {
        std::cerr << "ERROR: Directive cannot be specified before WorldBegin: "
                     "LightSource"
                  << std::endl;
        exit(EXIT_FAILURE);
      }

      state.LightSource(directive.light_source(), search_root);
      break;
    case Directive::kLookAt:
      state.matrix_manager.LookAt(
          directive.look_at().eye_x(), directive.look_at().eye_y(),
          directive.look_at().eye_z(), directive.look_at().look_x(),
          directive.look_at().look_y(), directive.look_at().look_z(),
          directive.look_at().up_x(), directive.look_at().up_y(),
          directive.look_at().up_z());
      break;
    case Directive::kMakeNamedMaterial:
      if (!state.world_begin_encountered) {
        std::cerr << "ERROR: Directive cannot be specified before WorldBegin: "
                     "MakeNamedMaterial"
                  << std::endl;
        exit(EXIT_FAILURE);
      }

      ParseMakeNamedMaterial(directive.make_named_material(), search_root,
                             state.material_manager, state.texture_manager,
                             state.spectrum_manager);
      break;
    case Directive::kMakeNamedMedium:
      std::cerr << "ERROR: Directive is not implemented: MakeNamedMedium"
                << std::endl;
      exit(EXIT_FAILURE);
      break;
    case Directive::kMaterial:
      if (!state.world_begin_encountered) {
        std::cerr << "ERROR: Directive cannot be specified before WorldBegin: "
                     "Material"
                  << std::endl;
        exit(EXIT_FAILURE);
      }

      state.graphics.top().material.second = ParseMaterial(
          directive.material(), Shape::MaterialOverrides::default_instance(),
          search_root, state.material_manager, state.texture_manager,
          state.spectrum_manager);
      state.graphics.top().material.first = directive.material();
      break;
    case Directive::kMediumInterface:
      std::cerr << "ERROR: Directive is not implemented: MediumInterface"
                << std::endl;
      exit(EXIT_FAILURE);
      break;
    case Directive::kNamedMaterial:
      if (!state.world_begin_encountered) {
        std::cerr << "ERROR: Directive cannot be specified before WorldBegin: "
                     "NamedMaterial"
                  << std::endl;
        exit(EXIT_FAILURE);
      }

      state.graphics.top().material =
          state.material_manager.Get(directive.named_material().name());
      break;
    case Directive::kObjectBegin:
      if (!state.world_begin_encountered) {
        std::cerr << "ERROR: Directive cannot be specified before WorldBegin: "
                     "ObjectBegin"
                  << std::endl;
        exit(EXIT_FAILURE);
      }

      if (state.build_instance) {
        std::cerr << "ERROR: Mismatched ObjectBegin and ObjectEnd directives"
                  << std::endl;
        exit(EXIT_FAILURE);
      }

      state.current_instance_name = directive.object_begin().name();
      state.build_instance = true;
      break;
    case Directive::kObjectEnd:
      if (!state.world_begin_encountered) {
        std::cerr << "ERROR: Directive cannot be specified before WorldBegin: "
                     "ObjectEnd"
                  << std::endl;
        exit(EXIT_FAILURE);
      }

      if (!state.build_instance) {
        std::cerr << "ERROR: Mismatched ObjectBegin and ObjectEnd directives"
                  << std::endl;
        exit(EXIT_FAILURE);
      }

      state.instances[std::move(state.current_instance_name)] =
          AllocateBVHAggregate(std::move(state.current_instance));
      state.current_instance_name.clear();
      state.current_instance.clear();
      state.build_instance = false;
      break;
    case Directive::kObjectInstance:
      if (!state.world_begin_encountered) {
        std::cerr << "ERROR: Directive cannot be specified before WorldBegin: "
                     "ObjectInstance"
                  << std::endl;
        exit(EXIT_FAILURE);
      }

      if (state.build_instance) {
        std::cerr << "ERROR: ObjectInstance cannot be specified between "
                     "ObjectBegin and ObjectEnd"
                  << std::endl;
        exit(EXIT_FAILURE);
      }

      if (auto iter = state.instances.find(directive.object_instance().name());
          iter != state.instances.end()) {
        state.objects.Add(iter->second, state.matrix_manager.Get().start);
      } else {
        std::cerr << "ERROR: ObjectInstance referred to an unknown object: "
                  << directive.object_instance().name() << std::endl;
        exit(EXIT_FAILURE);
      }
      break;
    case Directive::kPixelFilter:
      if (state.world_begin_encountered) {
        std::cerr
            << "ERROR: Directive cannot be specified between WorldBegin and "
               "WorldEnd: PixelFilter"
            << std::endl;
        exit(EXIT_FAILURE);
      }

      // Ignored
      break;
    case Directive::kReverseOrientation:
      if (!state.world_begin_encountered) {
        std::cerr << "ERROR: Directive cannot be specified before WorldBegin: "
                     "ReverseOrientation"
                  << std::endl;
        exit(EXIT_FAILURE);
      }

      state.graphics.top().reverse_orientation =
          !state.graphics.top().reverse_orientation;
      break;
    case Directive::kRotate:
      state.matrix_manager.Rotate(
          directive.rotate().angle(), directive.rotate().x(),
          directive.rotate().y(), directive.rotate().z());
      break;
    case Directive::kSampler:
      if (state.world_begin_encountered) {
        std::cerr
            << "ERROR: Directive cannot be specified between WorldBegin and "
               "WorldEnd: Sampler"
            << std::endl;
        exit(EXIT_FAILURE);
      }

      state.sampler = ParseSampler(directive.sampler());
      break;
    case Directive::kShape:
      if (!state.world_begin_encountered) {
        std::cerr << "ERROR: Directive cannot be specified before WorldBegin: "
                     "Shape"
                  << std::endl;
        exit(EXIT_FAILURE);
      }

      state.Shape(directive.shape(), search_root);
      break;
    case Directive::kScale:
      state.matrix_manager.Scale(directive.scale().x(), directive.scale().y(),
                                 directive.scale().z());
      break;
    case Directive::kSpectrumTexture:
      if (!state.world_begin_encountered) {
        std::cerr << "ERROR: Directive cannot be specified before WorldBegin: "
                     "Texture"
                  << std::endl;
        exit(EXIT_FAILURE);
      }

      ParseSpectrumTexture(directive.spectrum_texture(), state.image_manager,
                           state.texture_manager);
      break;
    case Directive::kTransform:
      state.matrix_manager.Transform(
          directive.transform().m00(), directive.transform().m01(),
          directive.transform().m02(), directive.transform().m03(),
          directive.transform().m10(), directive.transform().m11(),
          directive.transform().m12(), directive.transform().m13(),
          directive.transform().m20(), directive.transform().m21(),
          directive.transform().m22(), directive.transform().m23(),
          directive.transform().m30(), directive.transform().m31(),
          directive.transform().m32(), directive.transform().m33());
      break;
    case Directive::kTransformBegin:
      if (!state.world_begin_encountered) {
        std::cerr << "ERROR: Directive cannot be specified before WorldBegin: "
                     "TransformBegin"
                  << std::endl;
        exit(EXIT_FAILURE);
      }

      state.matrix_manager.TransformBegin();
      break;
    case Directive::kTransformEnd:
      if (!state.world_begin_encountered) {
        std::cerr << "ERROR: Directive cannot be specified before WorldBegin: "
                     "TransformEnd"
                  << std::endl;
        exit(EXIT_FAILURE);
      }

      state.matrix_manager.TransformEnd();
      break;
    case Directive::kTransformTimes:
      std::cerr << "ERROR: Directive is not implemented: TransformTimes"
                << std::endl;
      exit(EXIT_FAILURE);
      break;
    case Directive::kTranslate:
      state.matrix_manager.Translate(directive.translate().x(),
                                     directive.translate().y(),
                                     directive.translate().z());
      break;
    case Directive::kWorldBegin:
      if (state.world_begin_encountered) {
        std::cerr << "ERROR: Invalid WorldBegin directive" << std::endl;
        exit(EXIT_FAILURE);
      }

      if (!state.camera) {
        std::cerr << "ERROR: The scene specified an invalid Camera"
                  << std::endl;
        exit(EXIT_FAILURE);
      }

      if (!state.film) {
        std::cerr << "ERROR: The scene specified an invalid Film" << std::endl;
        exit(EXIT_FAILURE);
      }

      if (!state.integrator) {
        std::cerr << "ERROR: The scene specified an invalid Integrator"
                  << std::endl;
        exit(EXIT_FAILURE);
      }

      if (!state.sampler) {
        std::cerr << "ERROR: The scene specified an invalid Sampler"
                  << std::endl;
        exit(EXIT_FAILURE);
      }

      state.world_begin_encountered = true;
      state.matrix_manager.ActiveTransform(
          MatrixManager::ActiveTransformation::ALL);
      state.matrix_manager.Identity();
      state.matrix_manager.CoordinateSystem("world");
      break;
    case Directive::kWorldEnd:
      if (!state.world_begin_encountered) {
        std::cerr << "ERROR: Invalid WorldEnd directive" << std::endl;
        exit(EXIT_FAILURE);
      }

      return state.WorldEnd();
    case Directive::DIRECTIVE_TYPE_NOT_SET:
      break;
  }

  return std::nullopt;
}

}  // namespace

std::optional<ParsingResult> ParseScene(
    Directives& directives, const Options& options,
    const std::filesystem::path& search_root) {
  State state(directives, search_root, options.always_reflective);

  directives.Include(Defaults().global_defaults(),
                     std::filesystem::current_path());

  for (const Directive* next = directives.Next(); next != nullptr;
       next = directives.Next()) {
    if (std::optional<ParsingResult> result =
            ParseDirective(state, *next, search_root);
        result) {
      return result;
    }
  }

  if (state.world_begin_encountered) {
    std::cerr << "ERROR: Final directive should be WorldEnd" << std::endl;
    exit(EXIT_FAILURE);
  }

  return std::nullopt;
}

}  // namespace pbrt_frontend
}  // namespace iris
