#include "frontends/pbrt/texture_manager.h"

#include <cstdlib>
#include <iostream>
#include <string>

#include "frontends/pbrt/spectrum_manager.h"
#include "iris/reference_counted.h"
#include "iris/reflector.h"
#include "iris/spectral_allocator.h"
#include "iris/textures/constant_texture.h"
#include "iris/textures/texture2d.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {

using ::iris::textures::ConstantPointerTexture2D;
using ::iris::textures::ConstantValueTexture2D;
using ::pbrt_proto::v3::FloatTextureParameter;
using ::pbrt_proto::v3::Spectrum;
using ::pbrt_proto::v3::SpectrumTextureParameter;

TextureManager::FloatTexturePtr TextureManager::AllocateFloatTexture(
    visual value) {
  if (value == static_cast<visual>(0.0)) {
    return TextureManager::FloatTexturePtr();
  }

  FloatTexturePtr& texture = constant_float_textures_[value];
  if (!texture) {
    texture = MakeReferenceCounted<ConstantValueTexture2D<visual>>(value);
  }

  return texture;
}

TextureManager::FloatTexturePtr TextureManager::AllocateFloatTexture(
    const FloatTextureParameter& parameter) {
  TextureManager::FloatTexturePtr result;
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

TextureManager::ReflectorTexturePtr TextureManager::AllocateReflectorTexture(
    visual value) {
  Spectrum spectrum;
  spectrum.set_uniform_spectrum(value);
  return AllocateReflectorTexture(spectrum);
}

TextureManager::ReflectorTexturePtr TextureManager::AllocateReflectorTexture(
    const Spectrum& spectrum) {
  ReferenceCounted<Reflector> reflector =
      spectrum_manager_.AllocateReflector(spectrum);
  if (!reflector) {
    return TextureManager::ReflectorTexturePtr();
  }

  ReflectorTexturePtr& texture = constant_reflector_textures_[reflector.Get()];
  if (!texture) {
    texture = MakeReferenceCounted<
        ConstantPointerTexture2D<Reflector, SpectralAllocator>>(
        std::move(reflector));
  }

  return texture;
}

TextureManager::ReflectorTexturePtr TextureManager::AllocateReflectorTexture(
    const SpectrumTextureParameter& parameter) {
  TextureManager::ReflectorTexturePtr result;
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

TextureManager::FloatTexturePtr TextureManager::GetFloatTexture(
    const std::string& name) const {
  if (auto iter = float_textures_.find(name); iter != float_textures_.end()) {
    return iter->second;
  }

  std::cerr << "ERROR: No float texture defined with name: \"" << name << "\""
            << std::endl;
  exit(EXIT_FAILURE);
}

TextureManager::ReflectorTexturePtr TextureManager::GetReflectorTexture(
    const std::string& name) const {
  if (auto iter = reflector_textures_.find(name);
      iter != reflector_textures_.end()) {
    return iter->second;
  }

  std::cerr << "ERROR: No spectrum texture defined with name: \"" << name
            << "\"" << std::endl;
  exit(EXIT_FAILURE);
}

void TextureManager::Put(const std::string& name, FloatTexturePtr texture) {
  float_textures_[name] = std::move(texture);
}

void TextureManager::Put(const std::string& name, ReflectorTexturePtr texture) {
  reflector_textures_[name] = std::move(texture);
}

}  // namespace pbrt_frontend
}  // namespace iris
