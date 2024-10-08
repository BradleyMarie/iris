#include "frontends/pbrt/parameter.h"

#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <unordered_map>

#include "libspd/readers/emissive_spd_reader.h"
#include "libspd/readers/reflective_spd_reader.h"

namespace iris::pbrt_frontend {
namespace {

using libspd::ReadEmissiveSpdFrom;
using libspd::ReadReflectiveSpdFrom;

template <ParameterList::Type type, typename StoredType>
void ParseSimpleType(
    const ParameterList& parameter_list,
    const std::vector<StoredType>& (ParameterList::*get_source)() const,
    std::vector<StoredType>& destination) {
  if (parameter_list.GetType() != type) {
    std::cerr << "ERROR: Wrong type for parameter list: "
              << parameter_list.GetName() << std::endl;
    exit(EXIT_FAILURE);
  }

  destination.clear();
  for (const auto& entry : (parameter_list.*get_source)()) {
    destination.emplace_back(entry);
  }
}

template <Parameter::Type type, typename ReturnType>
const std::vector<ReturnType>& GetValues(const Parameter& parameter,
                                         const std::vector<ReturnType>& result,
                                         size_t max_num_values,
                                         size_t min_num_values) {
  assert(type == parameter.GetType());
  if (result.size() < min_num_values) {
    std::cerr << "ERROR: Too few values in parameter list: "
              << parameter.GetName() << std::endl;
    exit(EXIT_FAILURE);
  }

  if (max_num_values != 0 && result.size() > max_num_values) {
    std::cerr << "ERROR: Too many values in parameter list: "
              << parameter.GetName() << std::endl;
    exit(EXIT_FAILURE);
  }

  return result;
}

template <typename Type>
void ParseSpectralType(
    const std::variant<const std::map<visual, visual>*,
                       const std::vector<std::string_view>*>& var,
    std::string_view parameter_list_name,
    const std::filesystem::path& search_path, bool reflective,
    SpectrumManager& spectrum_manager,
    ReferenceCounted<Type> (SpectrumManager::*impl)(
        const std::map<visual, visual>&),
    std::vector<ReferenceCounted<Type>>& result) {
  if (std::holds_alternative<const std::map<visual, visual>*>(var)) {
    const std::map<visual, visual>& value_map =
        *std::get<const std::map<visual, visual>*>(var);

    if (reflective) {
      for (const auto& entry : value_map) {
        if (entry.second > static_cast<visual>(1.0)) {
          std::cerr << "ERROR: Out of range value in parameter list: "
                    << parameter_list_name << std::endl;
          exit(EXIT_FAILURE);
        }
      }
    }

    result.push_back((spectrum_manager.*impl)(value_map));
    return;
  }

  const std::vector<std::string_view>& file_paths =
      *std::get<const std::vector<std::string_view>*>(var);
  std::filesystem::path file_path;
  for (const auto& str : file_paths) {
    file_path = str;
    if (file_path.is_relative()) {
      file_path = search_path / file_path;
    }

    if (!std::filesystem::is_regular_file(file_path)) {
      std::cerr << "ERROR: Could not find file specified in parameter list: "
                << parameter_list_name << std::endl;
      exit(EXIT_FAILURE);
    }

    std::ifstream file(std::filesystem::weakly_canonical(file_path),
                       std::ios::in | std::ios::binary);

    std::expected<std::map<visual, visual>, std::string> values;
    if (reflective) {
      values = ReadReflectiveSpdFrom<visual>(file);
    } else {
      values = ReadEmissiveSpdFrom<visual>(file);
    }

    if (!values) {
      std::cerr << "ERROR: SPD file parsing failed with error: "
                << values.error() << std::endl;
      exit(EXIT_FAILURE);
    }

    result.push_back((spectrum_manager.*impl)(values.value()));
  }
}

}  // namespace

std::string_view Parameter::GetName() const { return name_.value(); }

Parameter::Type Parameter::GetType() const { return type_.value(); }

const std::vector<bool>& Parameter::GetBoolValues(size_t max_num_values,
                                                  size_t min_num_values) const {
  return GetValues<BOOL>(*this, bools_, max_num_values, min_num_values);
}

const std::vector<std::filesystem::path>& Parameter::GetFilePaths(
    size_t max_num_values, size_t min_num_values) const {
  return GetValues<FILE_PATH>(*this, file_paths_, max_num_values,
                              min_num_values);
}

const std::vector<long double>& Parameter::GetFloatValues(
    size_t max_num_values, size_t min_num_values) const {
  return GetValues<FLOAT>(*this, floats_, max_num_values, min_num_values);
}

const std::vector<ReferenceCounted<textures::ValueTexture2D<visual>>>&
Parameter::GetFloatTextures(size_t max_num_values,
                            size_t min_num_values) const {
  return GetValues<FLOAT_TEXTURE>(*this, float_textures_, max_num_values,
                                  min_num_values);
}

const std::vector<int64_t>& Parameter::GetIntegerValues(
    size_t max_num_values, size_t min_num_values) const {
  return GetValues<INTEGER>(*this, integers_, max_num_values, min_num_values);
}

const std::vector<Vector>& Parameter::GetNormalValues(
    size_t max_num_values, size_t min_num_values) const {
  return GetValues<NORMAL>(*this, vectors_, max_num_values, min_num_values);
}

const std::vector<Point>& Parameter::GetPoint3Values(
    size_t max_num_values, size_t min_num_values) const {
  return GetValues<POINT3>(*this, points_, max_num_values, min_num_values);
}

const std::vector<ReferenceCounted<Reflector>>& Parameter::GetReflectors(
    size_t max_num_values, size_t min_num_values) const {
  return GetValues<REFLECTOR>(*this, reflectors_, max_num_values,
                              min_num_values);
}

const std::vector<
    ReferenceCounted<textures::PointerTexture2D<Reflector, SpectralAllocator>>>&
Parameter::GetReflectorTextures(size_t max_num_values,
                                size_t min_num_values) const {
  return GetValues<REFLECTOR_TEXTURE>(*this, reflector_textures_,
                                      max_num_values, min_num_values);
}

const std::vector<std::string>& Parameter::GetStringValues(
    size_t max_num_values, size_t min_num_values) const {
  return GetValues<STRING>(*this, strings_, max_num_values, min_num_values);
}

const std::vector<iris::ReferenceCounted<Spectrum>>& Parameter::GetSpectra(
    size_t max_num_values, size_t min_num_values) const {
  return GetValues<SPECTRUM>(*this, spectra_, max_num_values, min_num_values);
}

const std::vector<Vector>& Parameter::GetVector3Values(
    size_t max_num_values, size_t min_num_values) const {
  return GetValues<VECTOR3>(*this, vectors_, max_num_values, min_num_values);
}

void Parameter::ParseBool(const ParameterList& parameter_list,
                          const std::filesystem::path& search_path,
                          SpectrumManager& spectrum_manager,
                          TextureManager& texture_manager) {
  ParseSimpleType<ParameterList::BOOL>(parameter_list,
                                       &ParameterList::GetBoolValues, bools_);
}

void Parameter::ParseFilePath(const ParameterList& parameter_list,
                              const std::filesystem::path& search_path,
                              SpectrumManager& spectrum_manager,
                              TextureManager& texture_manager) {
  file_paths_.clear();

  if (parameter_list.GetType() != ParameterList::STRING) {
    std::cerr << "ERROR: Wrong type for parameter list: "
              << parameter_list.GetName() << std::endl;
    exit(EXIT_FAILURE);
  }

  for (const auto& string : parameter_list.GetStringValues()) {
    file_paths_.emplace_back(string);
    if (file_paths_.back().is_relative()) {
      file_paths_.back() = search_path / file_paths_.back();
    }

    if (!std::filesystem::is_regular_file(file_paths_.back())) {
      std::cerr << "ERROR: Could not find file specified in parameter list: "
                << parameter_list.GetName() << std::endl;
      exit(EXIT_FAILURE);
    }

    file_paths_.back() = std::filesystem::weakly_canonical(file_paths_.back());
  }
}

void Parameter::ParseFloat(const ParameterList& parameter_list,
                           const std::filesystem::path& search_path,
                           SpectrumManager& spectrum_manager,
                           TextureManager& texture_manager) {
  ParseSimpleType<ParameterList::FLOAT>(
      parameter_list, &ParameterList::GetFloatValues, floats_);
}

void Parameter::ParseFloatTexture(const ParameterList& parameter_list,
                                  const std::filesystem::path& search_path,
                                  SpectrumManager& spectrum_manager,
                                  TextureManager& texture_manager) {
  float_textures_.clear();
  if (parameter_list.GetType() == ParameterList::FLOAT) {
    for (const auto& entry : parameter_list.GetFloatValues()) {
      visual as_visual = static_cast<visual>(entry);
      if (!std::isfinite(as_visual)) {
        std::cerr << "ERROR: Out of range value in parameter list: "
                  << GetName() << std::endl;
        exit(EXIT_FAILURE);
      }

      float_textures_.push_back(
          texture_manager.AllocateUniformFloatTexture(as_visual));
    }
    return;
  }

  if (parameter_list.GetType() == ParameterList::TEXTURE) {
    for (const auto& entry : parameter_list.GetTextureValues()) {
      float_textures_.push_back(texture_manager.GetFloatTexture(entry));
    }
    return;
  }

  std::cerr << "ERROR: Wrong type for parameter list: "
            << parameter_list.GetName() << std::endl;
  exit(EXIT_FAILURE);
}

void Parameter::ParseInteger(const ParameterList& parameter_list,
                             const std::filesystem::path& search_path,
                             SpectrumManager& spectrum_manager,
                             TextureManager& texture_manager) {
  ParseSimpleType<ParameterList::INTEGER>(
      parameter_list, &ParameterList::GetIntegerValues, integers_);
}

void Parameter::ParseNormal(const ParameterList& parameter_list,
                            const std::filesystem::path& search_path,
                            SpectrumManager& spectrum_manager,
                            TextureManager& texture_manager) {
  ParseSimpleType<ParameterList::NORMAL>(
      parameter_list, &ParameterList::GetNormalValues, vectors_);
}

void Parameter::ParsePoint3(const ParameterList& parameter_list,
                            const std::filesystem::path& search_path,
                            SpectrumManager& spectrum_manager,
                            TextureManager& texture_manager) {
  ParseSimpleType<ParameterList::POINT3>(
      parameter_list, &ParameterList::GetPoint3Values, points_);
}

void Parameter::ParseReflector(const ParameterList& parameter_list,
                               const std::filesystem::path& search_path,
                               SpectrumManager& spectrum_manager,
                               TextureManager& texture_manager) {
  reflectors_.clear();
  if (parameter_list.GetType() == ParameterList::FLOAT) {
    for (const auto& entry : parameter_list.GetFloatValues()) {
      if (entry < 0.0 || entry > 1.0) {
        std::cerr << "ERROR: Out of range value in parameter list: "
                  << GetName() << std::endl;
        exit(EXIT_FAILURE);
      }

      reflectors_.push_back(
          texture_manager.AllocateUniformReflector(static_cast<visual>(entry)));
    }
    return;
  }

  if (parameter_list.GetType() == ParameterList::COLOR) {
    for (const auto& entry : parameter_list.GetColorValues()) {
      if (entry.values[0] > 1.0 || entry.values[1] > 1.0 ||
          entry.values[2] > 1.0) {
        std::cerr << "ERROR: Out of range value in parameter list: "
                  << GetName() << std::endl;
        exit(EXIT_FAILURE);
      }

      reflectors_.push_back(spectrum_manager.AllocateReflector(entry));
    }
    return;
  }

  if (parameter_list.GetType() == ParameterList::SPECTRUM) {
    ParseSpectralType(parameter_list.GetSpectrumValues(), GetName(),
                      search_path, true, spectrum_manager,
                      &SpectrumManager::AllocateReflector, reflectors_);
    return;
  }

  std::cerr << "ERROR: Wrong type for parameter list: "
            << parameter_list.GetName() << std::endl;
  exit(EXIT_FAILURE);
}

void Parameter::ParseReflectorTexture(const ParameterList& parameter_list,
                                      const std::filesystem::path& search_path,
                                      SpectrumManager& spectrum_manager,
                                      TextureManager& texture_manager) {
  reflector_textures_.clear();
  if (parameter_list.GetType() == ParameterList::FLOAT) {
    for (const auto& entry : parameter_list.GetFloatValues()) {
      if (entry < 0.0 || entry > 1.0) {
        std::cerr << "ERROR: Out of range value in parameter list: "
                  << GetName() << std::endl;
        exit(EXIT_FAILURE);
      }

      reflector_textures_.push_back(
          texture_manager.AllocateUniformReflectorTexture(entry));
    }
    return;
  }

  if (parameter_list.GetType() == ParameterList::COLOR) {
    for (const auto& entry : parameter_list.GetColorValues()) {
      if (entry.values[0] > 1.0 || entry.values[1] > 1.0 ||
          entry.values[2] > 1.0) {
        std::cerr << "ERROR: Out of range value in parameter list: "
                  << GetName() << std::endl;
        exit(EXIT_FAILURE);
      }

      reflector_textures_.push_back(
          texture_manager.AllocateUniformReflectorTexture(
              spectrum_manager.AllocateReflector(entry)));
    }
    return;
  }

  if (parameter_list.GetType() == ParameterList::SPECTRUM) {
    ParseSpectralType(parameter_list.GetSpectrumValues(), GetName(),
                      search_path, true, spectrum_manager,
                      &SpectrumManager::AllocateReflector, reflectors_);
    for (auto& reflector : reflectors_) {
      reflector_textures_.push_back(
          texture_manager.AllocateUniformReflectorTexture(
              std::move(reflector)));
    }
    return;
  }

  if (parameter_list.GetType() == ParameterList::TEXTURE) {
    for (const auto& entry : parameter_list.GetTextureValues()) {
      reflector_textures_.push_back(texture_manager.GetReflectorTexture(entry));
    }
    return;
  }

  std::cerr << "ERROR: Wrong type for parameter list: "
            << parameter_list.GetName() << std::endl;
  exit(EXIT_FAILURE);
}

void Parameter::ParseSpectrum(const ParameterList& parameter_list,
                              const std::filesystem::path& search_path,
                              SpectrumManager& spectrum_manager,
                              TextureManager& texture_manager) {
  spectra_.clear();
  if (parameter_list.GetType() == ParameterList::COLOR) {
    for (const auto& entry : parameter_list.GetColorValues()) {
      spectra_.push_back(spectrum_manager.AllocateSpectrum(entry));
    }
    return;
  }

  if (parameter_list.GetType() == ParameterList::SPECTRUM) {
    ParseSpectralType(parameter_list.GetSpectrumValues(), GetName(),
                      search_path, false, spectrum_manager,
                      &SpectrumManager::AllocateSpectrum, spectra_);
    return;
  }

  std::cerr << "ERROR: Wrong type for parameter list: "
            << parameter_list.GetName() << std::endl;
  exit(EXIT_FAILURE);
}

void Parameter::ParseString(const ParameterList& parameter_list,
                            const std::filesystem::path& search_path,
                            SpectrumManager& spectrum_manager,
                            TextureManager& texture_manager) {
  strings_.clear();

  if (parameter_list.GetType() != ParameterList::STRING) {
    std::cerr << "ERROR: Wrong type for parameter list: "
              << parameter_list.GetName() << std::endl;
    exit(EXIT_FAILURE);
  }

  for (const auto& string : parameter_list.GetStringValues()) {
    strings_.emplace_back(string);
  }
}

void Parameter::ParseVector3(const ParameterList& parameter_list,
                             const std::filesystem::path& search_path,
                             SpectrumManager& spectrum_manager,
                             TextureManager& texture_manager) {
  ParseSimpleType<ParameterList::VECTOR3>(
      parameter_list, &ParameterList::GetVector3Values, vectors_);
}

void Parameter::LoadFrom(const ParameterList& parameter_list,
                         const std::filesystem::path& search_path, Type type,
                         SpectrumManager& spectrum_manager,
                         TextureManager& texture_manager) {
  static std::unordered_map<Type, void (Parameter::*)(
                                      const ParameterList&,
                                      const std::filesystem::path&,
                                      SpectrumManager&, TextureManager&)>
      callbacks = {
          {BOOL, &Parameter::ParseBool},
          {FILE_PATH, &Parameter::ParseFilePath},
          {FLOAT, &Parameter::ParseFloat},
          {FLOAT_TEXTURE, &Parameter::ParseFloatTexture},
          {INTEGER, &Parameter::ParseInteger},
          {NORMAL, &Parameter::ParseNormal},
          {POINT3, &Parameter::ParsePoint3},
          {REFLECTOR, &Parameter::ParseReflector},
          {REFLECTOR_TEXTURE, &Parameter::ParseReflectorTexture},
          {SPECTRUM, &Parameter::ParseSpectrum},
          {STRING, &Parameter::ParseString},
          {VECTOR3, &Parameter::ParseVector3},
      };

  name_ = parameter_list.GetName();
  type_ = type;

  auto callback = callbacks.at(type);
  (this->*callback)(parameter_list, search_path, spectrum_manager,
                    texture_manager);
}

}  // namespace iris::pbrt_frontend