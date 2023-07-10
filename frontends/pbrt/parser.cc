#include "frontends/pbrt/parser.h"

#include <cstdlib>
#include <iostream>
#include <unordered_map>

#include "frontends/pbrt/area_lights/parse.h"
#include "frontends/pbrt/build_objects.h"
#include "frontends/pbrt/cameras/parse.h"
#include "frontends/pbrt/film/parse.h"
#include "frontends/pbrt/integrators/parse.h"
#include "frontends/pbrt/lights/parse.h"
#include "frontends/pbrt/materials/parse.h"
#include "frontends/pbrt/quoted_string.h"
#include "frontends/pbrt/samplers/parse.h"
#include "frontends/pbrt/shapes/parse.h"
#include "frontends/pbrt/textures/parse.h"
#include "iris/scenes/bvh_scene.h"

namespace iris::pbrt_frontend {

bool Parser::AreaLightSource() {
  if (!world_begin_encountered_) {
    std::cerr << "ERROR: Directive cannot be specified before WorldBegin: "
                 "AreaLightSource"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  const auto& builder = area_lights::Parse(*tokenizers_.back().tokenizer);
  attributes_.back().emissive_material =
      BuildObject(builder, *tokenizers_.back().tokenizer, *spectrum_manager_,
                  *texture_manager_, *spectrum_manager_);

  return true;
}

bool Parser::AttributeBegin() {
  if (!world_begin_encountered_) {
    std::cerr << "ERROR: Directive cannot be specified before WorldBegin: "
                 "AttributeBegin"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  attributes_.push_back(attributes_.back());
  matrix_manager_->Push();

  return true;
}

bool Parser::AttributeEnd() {
  if (!world_begin_encountered_) {
    std::cerr << "ERROR: Directive cannot be specified before WorldBegin: "
                 "AttributeEnd"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  attributes_.pop_back();
  matrix_manager_->Pop();

  return true;
}

bool Parser::Camera() {
  if (world_begin_encountered_) {
    std::cerr << "ERROR: Directive cannot be specified between WorldBegin and "
                 "WorldEnd: Camera"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  if (camera_encountered_) {
    std::cerr << "ERROR: Directive specified twice for a render: Camera"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  const auto& builder = cameras::Parse(*tokenizers_.back().tokenizer);
  camera_ =
      BuildObject(builder, *tokenizers_.back().tokenizer, *spectrum_manager_,
                  *texture_manager_, matrix_manager_->Get());

  camera_encountered_ = true;

  return true;
}

bool Parser::Film() {
  if (world_begin_encountered_) {
    std::cerr << "ERROR: Directive cannot be specified between WorldBegin and "
                 "WorldEnd: Film"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  if (film_encountered_) {
    std::cerr << "ERROR: Directive specified twice for a render: Film"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  const auto& builder = film::Parse(*tokenizers_.back().tokenizer);
  auto result = BuildObject(builder, *tokenizers_.back().tokenizer,
                            *spectrum_manager_, *texture_manager_);

  image_dimensions_ = result.resolution;
  output_filename_ = result.filename;
  write_function_ = result.write_function;

  film_encountered_ = true;

  return true;
}

bool Parser::Include() {
  auto next = tokenizers_.back().tokenizer->Next();
  if (!next) {
    std::cerr << "ERROR: Too few parameters to directive: Include" << std::endl;
    exit(EXIT_FAILURE);
  }

  auto unquoted = Unquote(*next);
  if (!unquoted) {
    std::cerr << "ERROR: Parameter to Include must be a string" << std::endl;
    exit(EXIT_FAILURE);
  }

  std::filesystem::path file_path(*unquoted);
  if (file_path.is_relative()) {
    file_path = tokenizers_.back().tokenizer->SearchRoot() / file_path;
  }

  file_path = std::filesystem::weakly_canonical(file_path);

  for (const auto& entry : tokenizers_) {
    if (entry.tokenizer->FilePath().has_value() &&
        file_path == *entry.tokenizer->FilePath()) {
      std::cerr << "ERROR: Detected cyclic Include of file: " << *unquoted
                << std::endl;
      exit(EXIT_FAILURE);
    }
  }

  std::filesystem::path search_path = file_path;
  search_path.remove_filename();

  auto file = std::make_unique<std::ifstream>(file_path);
  if (file->fail()) {
    std::cerr << "ERROR: Failed to open file: " << *unquoted << std::endl;
    exit(EXIT_FAILURE);
  }

  auto tokenizer = std::make_unique<Tokenizer>(*file, file_path);

  tokenizers_.emplace_back(tokenizer.get(), std::move(tokenizer),
                           std::move(file));

  return true;
}

bool Parser::Integrator() {
  if (world_begin_encountered_) {
    std::cerr << "ERROR: Directive cannot be specified between WorldBegin and "
                 "WorldEnd: Integrator"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  if (integrator_encountered_) {
    std::cerr << "ERROR: Directive specified twice for a render: Integrator"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  const auto& builder = integrators::Parse(*tokenizers_.back().tokenizer);
  auto object = BuildObject(builder, *tokenizers_.back().tokenizer,
                            *spectrum_manager_, *texture_manager_);
  integrator_ = std::move(object.integrator);
  light_scene_builder_ = std::move(object.light_scene_builder);

  integrator_encountered_ = true;

  return true;
}

bool Parser::LightSource() {
  if (!world_begin_encountered_) {
    std::cerr
        << "ERROR: Directive cannot be specified before WorldBegin: LightSource"
        << std::endl;
    exit(EXIT_FAILURE);
  }

  const auto& builder = lights::Parse(*tokenizers_.back().tokenizer);
  auto light = BuildObject(
      builder, *tokenizers_.back().tokenizer, *spectrum_manager_,
      *texture_manager_, *spectrum_manager_,
      current_object_ ? Matrix::Identity() : matrix_manager_->Get().start);

  if (std::holds_alternative<ReferenceCounted<EnvironmentalLight>>(light)) {
    // TODO: Merge environmental lights
    scene_objects_builder_.Set(
        std::get<ReferenceCounted<EnvironmentalLight>>(light));
  } else {
    scene_objects_builder_.Add(std::get<ReferenceCounted<iris::Light>>(light));
  }

  return true;
}

bool Parser::MakeNamedMaterial() {
  materials::ParseNamed(*tokenizers_.back().tokenizer, *material_manager_,
                        *spectrum_manager_, *texture_manager_);
  return true;
}

bool Parser::Material() {
  if (!world_begin_encountered_) {
    std::cerr
        << "ERROR: Directive cannot be specified before WorldBegin: Material"
        << std::endl;
    exit(EXIT_FAILURE);
  }

  const auto& builder = materials::Parse(*tokenizers_.back().tokenizer);
  attributes_.back().material =
      BuildObject(builder, *tokenizers_.back().tokenizer, *spectrum_manager_,
                  *texture_manager_, *texture_manager_);

  return true;
}

bool Parser::NamedMaterial() {
  if (!world_begin_encountered_) {
    std::cerr << "ERROR: Directive cannot be specified before WorldBegin: "
                 "NamedMaterial"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  auto next = tokenizers_.back().tokenizer->Next();
  if (!next) {
    std::cerr << "ERROR: Too few parameters to directive: NamedMaterial"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  auto unquoted = Unquote(*next);
  if (!unquoted) {
    std::cerr << "ERROR: Parameter to NamedMaterial must be a string"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  attributes_.back().material = material_manager_->Get(*unquoted);

  return true;
}

bool Parser::ObjectBegin() {
  if (!world_begin_encountered_) {
    std::cerr
        << "ERROR: Directive cannot be specified before WorldBegin: ObjectBegin"
        << std::endl;
    exit(EXIT_FAILURE);
  }

  if (current_object_) {
    std::cerr << "ERROR: Mismatched ObjectBegin and ObjectEnd directives"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  auto next = tokenizers_.back().tokenizer->Next();
  if (!next) {
    std::cerr << "ERROR: Too few parameters to directive: ObjectBegin"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  auto unquoted = Unquote(*next);
  if (!unquoted) {
    std::cerr << "ERROR: Parameter to ObjectBegin must be a string"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  current_object_ = *unquoted;

  return true;
}

bool Parser::ObjectEnd() {
  if (!world_begin_encountered_) {
    std::cerr
        << "ERROR: Directive cannot be specified before WorldBegin: ObjectEnd"
        << std::endl;
    exit(EXIT_FAILURE);
  }

  if (!current_object_) {
    std::cerr << "ERROR: Mismatched ObjectBegin and ObjectEnd directives"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  current_object_.reset();

  return true;
}

bool Parser::ObjectInstance() {
  if (!world_begin_encountered_) {
    std::cerr << "ERROR: Directive cannot be specified before WorldBegin: "
                 "ObjectInstance"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  if (current_object_) {
    std::cerr << "ERROR: ObjectInstance cannot be specified between "
                 "ObjectBegin and ObjectEnd"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  auto next = tokenizers_.back().tokenizer->Next();
  if (!next) {
    std::cerr << "ERROR: Too few parameters to directive: ObjectInstance"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  auto unquoted = Unquote(*next);
  if (!unquoted) {
    std::cerr << "ERROR: Parameter to ObjectInstance must be a string"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  auto iter = objects_.find(std::string(*unquoted));
  if (iter == objects_.end()) {
    std::cerr << "ERROR: ObjectInstance referred to an unknown object: "
              << *unquoted << std::endl;
    exit(EXIT_FAILURE);
  }

  for (const auto& geometry : iter->second) {
    scene_objects_builder_.Add(geometry, matrix_manager_->Get().start);
  }

  return true;
}

bool Parser::PixelFilter() {
  if (world_begin_encountered_) {
    std::cerr << "ERROR: Directive cannot be specified between WorldBegin and "
                 "WorldEnd: PixelFilter"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  if (pixel_filter_encountered_) {
    std::cerr << "ERROR: Directive specified twice for a render: PixelFilter"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  auto next = tokenizers_.back().tokenizer->Next();
  if (!next) {
    std::cerr << "ERROR: Too few parameters to directive: PixelFilter"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  auto unquoted = Unquote(*next);
  if (!unquoted) {
    std::cerr << "ERROR: Parameter to PixelFilter must be a string"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  if (*unquoted != "box") {
    std::cerr << "ERROR: Unsupported type for directive PixelFilter: "
              << *unquoted << std::endl;
    exit(EXIT_FAILURE);
  }

  pixel_filter_encountered_ = true;

  return true;
}

bool Parser::ReverseOrientation() {
  attributes_.back().reverse_orientation =
      !attributes_.back().reverse_orientation;
  return true;
}

bool Parser::Sampler() {
  if (world_begin_encountered_) {
    std::cerr << "ERROR: Directive cannot be specified between WorldBegin and "
                 "WorldEnd: Sampler"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  if (sampler_encountered_) {
    std::cerr << "ERROR: Directive specified twice for a render: Sampler"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  const auto& builder = samplers::Parse(*tokenizers_.back().tokenizer);
  image_sampler_ = BuildObject(builder, *tokenizers_.back().tokenizer,
                               *spectrum_manager_, *texture_manager_);

  sampler_encountered_ = true;

  return true;
}

bool Parser::Shape() {
  if (!world_begin_encountered_) {
    std::cerr << "ERROR: Directive cannot be specified before WorldBegin: Shape"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  auto [shapes, transform] = shapes::Parse(
      *tokenizers_.back().tokenizer, *spectrum_manager_, *texture_manager_,
      attributes_.back().material, attributes_.back().emissive_material.first,
      attributes_.back().emissive_material.second,
      current_object_ ? Matrix::Identity() : matrix_manager_->Get().start,
      attributes_.back().reverse_orientation);

  if (current_object_) {
    auto& objects = objects_[*current_object_];
    objects.insert(objects.end(), shapes.begin(), shapes.end());
  } else {
    for (const auto& geometry : shapes) {
      scene_objects_builder_.Add(geometry, transform);
    }
  }

  return true;
}

bool Parser::Texture() {
  if (!world_begin_encountered_) {
    std::cerr
        << "ERROR: Directive cannot be specified before WorldBegin: Texture"
        << std::endl;
    exit(EXIT_FAILURE);
  }

  const auto& builder =
      textures::Parse(*tokenizers_.back().tokenizer, texture_name_);
  BuildObject(builder, *tokenizers_.back().tokenizer, *spectrum_manager_,
              *texture_manager_, *texture_manager_, *spectrum_manager_,
              static_cast<const std::string&>(texture_name_));

  return true;
}

bool Parser::WorldBegin() {
  if (world_begin_encountered_) {
    std::cerr << "ERROR: Invalid WorldBegin directive" << std::endl;
    exit(EXIT_FAILURE);
  }

  matrix_manager_ = std::make_unique<MatrixManager>();

  world_begin_encountered_ = true;
  return true;
}

bool Parser::WorldEnd() {
  if (!world_begin_encountered_) {
    std::cerr << "ERROR: Invalid WorldEnd directive" << std::endl;
    exit(EXIT_FAILURE);
  }

  return false;
}

std::optional<std::string_view> Parser::PeekToken() {
  while (!tokenizers_.empty()) {
    if (tokenizers_.back().tokenizer->Peek()) {
      return tokenizers_.back().tokenizer->Peek();
    }

    tokenizers_.pop_back();
  }

  return std::nullopt;
}

std::string_view Parser::NextToken() {
  return tokenizers_.back().tokenizer->Next().value();
}

void Parser::InitializeDefault() {
  std::stringstream empty("");
  iris::pbrt_frontend::Tokenizer empty_tokenizer(empty);

  auto default_film = BuildObject(film::Default(), empty_tokenizer,
                                  *spectrum_manager_, *texture_manager_);
  image_dimensions_ = default_film.resolution;
  output_filename_ = default_film.filename;
  write_function_ = std::move(default_film.write_function);

  auto default_integrator = BuildObject(integrators::Default(), empty_tokenizer,
                                        *spectrum_manager_, *texture_manager_);
  integrator_ = std::move(default_integrator.integrator);
  light_scene_builder_ = std::move(default_integrator.light_scene_builder);

  image_sampler_ = BuildObject(samplers::Default(), empty_tokenizer,
                               *spectrum_manager_, *texture_manager_);

  auto default_camera =
      BuildObject(cameras::Default(), empty_tokenizer, *spectrum_manager_,
                  *texture_manager_, matrix_manager_->Get());
  camera_ = std::move(default_camera);

  auto default_material =
      BuildObject(materials::Default(), empty_tokenizer, *spectrum_manager_,
                  *texture_manager_, *texture_manager_);
  attributes_.emplace_back(
      default_material,
      std::pair<iris::ReferenceCounted<iris::EmissiveMaterial>,
                iris::ReferenceCounted<iris::EmissiveMaterial>>(),
      false);
}

std::optional<Parser::Result> Parser::ParseFrom(Tokenizer& tokenizer) {
  material_manager_ = std::make_unique<MaterialManager>();
  matrix_manager_ = std::make_unique<MatrixManager>();
  texture_manager_ = std::make_unique<TextureManager>();

  camera_encountered_ = false;
  film_encountered_ = false;
  integrator_encountered_ = false;
  pixel_filter_encountered_ = false;
  sampler_encountered_ = false;
  world_begin_encountered_ = false;

  static const std::unordered_map<std::string_view, bool (Parser::*)()>
      callbacks = {
          {"AreaLightSource", &Parser::AreaLightSource},
          {"AttributeBegin", &Parser::AttributeBegin},
          {"AttributeEnd", &Parser::AttributeEnd},
          {"Camera", &Parser::Camera},
          {"Film", &Parser::Film},
          {"Include", &Parser::Include},
          {"Integrator", &Parser::Integrator},
          {"LightSource", &Parser::LightSource},
          {"MakeNamedMaterial", &Parser::MakeNamedMaterial},
          {"Material", &Parser::Material},
          {"NamedMaterial", &Parser::NamedMaterial},
          {"ObjectBegin", &Parser::ObjectBegin},
          {"ObjectEnd", &Parser::ObjectEnd},
          {"ObjectInstance", &Parser::ObjectInstance},
          {"PixelFilter", &Parser::PixelFilter},
          {"ReverseOrientation", &Parser::ReverseOrientation},
          {"Sampler", &Parser::Sampler},
          {"Shape", &Parser::Shape},
          {"Texture", &Parser::Texture},
          {"WorldBegin", &Parser::WorldBegin},
          {"WorldEnd", &Parser::WorldEnd},
      };

  tokenizers_.emplace_back(&tokenizer, nullptr, nullptr);

  InitializeDefault();

  bool tokens_parsed = false;
  for (;;) {
    auto peeked_token = PeekToken();
    if (!peeked_token.has_value()) {
      if (tokens_parsed) {
        std::cerr << "ERROR: Final directive should be WorldEnd" << std::endl;
        exit(EXIT_FAILURE);
      }

      return std::nullopt;
    }

    tokens_parsed = true;

    if (matrix_manager_->TryParse(*tokenizers_.back().tokenizer)) {
      continue;
    }

    auto token = NextToken();

    auto iter = callbacks.find(token);
    if (iter == callbacks.end()) {
      std::cerr << "ERROR: Invalid directive: " << token << std::endl;
      exit(EXIT_FAILURE);
    }

    if (!(this->*iter->second)()) {
      break;
    }
  }

  Renderer renderer(scenes::BVHScene::Builder(), *light_scene_builder_,
                    scene_objects_builder_.Build());
  Renderable renderable(std::move(renderer), camera_(image_dimensions_),
                        std::move(image_sampler_), std::move(integrator_),
                        image_dimensions_);

  camera_ = nullptr;
  attributes_.clear();
  objects_.clear();
  spectrum_manager_->Clear();
  current_object_.reset();
  material_manager_.reset();
  matrix_manager_.reset();
  texture_manager_.reset();

  return Parser::Result{std::move(renderable), std::move(output_filename_),
                        std::move(write_function_)};
}

}  // namespace iris::pbrt_frontend