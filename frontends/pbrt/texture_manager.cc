#include "frontends/pbrt/texture_manager.h"

#include <cstdlib>
#include <iostream>
#include <string>

#include "frontends/pbrt/spectrum_manager.h"
#include "iris/reference_counted.h"
#include "iris/reflector.h"
#include "iris/spectral_allocator.h"
#include "iris/textures/constant_texture.h"
#include "iris/textures/float_texture.h"
#include "iris/textures/reflector_texture.h"
#include "pbrt_proto/v3/v3.pb.h"

namespace iris {
namespace pbrt_frontend {

using ::iris::textures::MakeConstantTexture;
using ::pbrt_proto::v3::FloatTextureParameter;
using ::pbrt_proto::v3::Spectrum;
using ::pbrt_proto::v3::SpectrumTextureParameter;

void TextureManager::AttributeBegin() {
  float_textures_.push(float_textures_.top());
  reflector_textures_.push(reflector_textures_.top());
}

void TextureManager::AttributeEnd() {
  if (float_textures_.size() == 1 || reflector_textures_.size() == 1) {
    std::cerr << "ERROR: Mismatched AttributeBegin and AttributeEnd directives"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  float_textures_.pop();
  reflector_textures_.pop();
}

ReferenceCounted<textures::FloatTexture> TextureManager::AllocateFloatTexture(
    visual value) {
  if (value == static_cast<visual>(0.0)) {
    return ReferenceCounted<textures::FloatTexture>();
  }

  ReferenceCounted<textures::FloatTexture>& texture =
      constant_float_textures_[value];
  if (!texture) {
    texture = MakeConstantTexture(value);
  }

  return texture;
}

ReferenceCounted<textures::FloatTexture> TextureManager::AllocateFloatTexture(
    const FloatTextureParameter& parameter) {
  ReferenceCounted<textures::FloatTexture> result;
  switch (parameter.float_texture_parameter_type_case()) {
    case FloatTextureParameter::kFloatValue:
      result = AllocateFloatTexture(parameter.float_value());
      break;
    case FloatTextureParameter::kFloatTextureName:
      result = GetFloatTexture(parameter.float_texture_name());
      break;
    case FloatTextureParameter::FLOAT_TEXTURE_PARAMETER_TYPE_NOT_SET:
      break;
  }

  return result;
}

ReferenceCounted<textures::ReflectorTexture>
TextureManager::AllocateReflectorTexture(visual value) {
  Spectrum spectrum;
  spectrum.set_uniform_spectrum(value);
  return AllocateReflectorTexture(spectrum);
}

ReferenceCounted<textures::ReflectorTexture>
TextureManager::AllocateReflectorTexture(const Spectrum& spectrum) {
  ReferenceCounted<Reflector> reflector =
      spectrum_manager_.AllocateReflector(spectrum);
  if (!reflector) {
    return ReferenceCounted<textures::ReflectorTexture>();
  }

  ReferenceCounted<textures::ReflectorTexture>& texture =
      constant_reflector_textures_[reflector.Get()];
  if (!texture) {
    texture = MakeConstantTexture(std::move(reflector));
  }

  return texture;
}

ReferenceCounted<textures::ReflectorTexture>
TextureManager::AllocateReflectorTexture(
    const SpectrumTextureParameter& parameter) {
  ReferenceCounted<textures::ReflectorTexture> result;
  Spectrum spectrum;
  switch (parameter.spectrum_texture_parameter_type_case()) {
    case SpectrumTextureParameter::kBlackbodySpectrum:
      *spectrum.mutable_blackbody_spectrum() = parameter.blackbody_spectrum();
      result = AllocateReflectorTexture(spectrum);
      break;
    case SpectrumTextureParameter::kRgbSpectrum:
      *spectrum.mutable_rgb_spectrum() = parameter.rgb_spectrum();
      result = AllocateReflectorTexture(spectrum);
      break;
    case SpectrumTextureParameter::kSampledSpectrum:
      *spectrum.mutable_sampled_spectrum() = parameter.sampled_spectrum();
      result = AllocateReflectorTexture(spectrum);
      break;
    case SpectrumTextureParameter::kSampledSpectrumFilename:
      spectrum.set_sampled_spectrum_filename(
          parameter.sampled_spectrum_filename());
      result = AllocateReflectorTexture(spectrum);
      break;
    case SpectrumTextureParameter::kSpectrumTextureName:
      result = GetReflectorTexture(parameter.spectrum_texture_name());
      break;
    case SpectrumTextureParameter::kUniformSpectrum:
      spectrum.set_uniform_spectrum(parameter.uniform_spectrum());
      result = AllocateReflectorTexture(spectrum);
      break;
    case SpectrumTextureParameter::kXyzSpectrum:
      *spectrum.mutable_xyz_spectrum() = parameter.xyz_spectrum();
      result = AllocateReflectorTexture(spectrum);
      break;
    case SpectrumTextureParameter::SPECTRUM_TEXTURE_PARAMETER_TYPE_NOT_SET:
      break;
  }

  return result;
}

ReferenceCounted<textures::FloatTexture> TextureManager::GetFloatTexture(
    const std::string& name) const {
  if (auto iter = float_textures_.top().find(name);
      iter != float_textures_.top().end()) {
    return iter->second;
  }

  std::cerr << "ERROR: No float texture defined with name: \"" << name << "\""
            << std::endl;
  exit(EXIT_FAILURE);
}

ReferenceCounted<textures::ReflectorTexture>
TextureManager::GetReflectorTexture(const std::string& name) const {
  if (auto iter = reflector_textures_.top().find(name);
      iter != reflector_textures_.top().end()) {
    return iter->second;
  }

  std::cerr << "ERROR: No spectrum texture defined with name: \"" << name
            << "\"" << std::endl;
  exit(EXIT_FAILURE);
}

void TextureManager::Put(const std::string& name,
                         ReferenceCounted<textures::FloatTexture> texture) {
  float_textures_.top()[name] = std::move(texture);
}

void TextureManager::Put(const std::string& name,
                         ReferenceCounted<textures::ReflectorTexture> texture) {
  reflector_textures_.top()[name] = std::move(texture);
}

}  // namespace pbrt_frontend
}  // namespace iris
